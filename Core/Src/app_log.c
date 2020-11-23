/*
 * app_log.c
 *
 *  Created on: Nov 13, 2020
 *      Author: Ilde
 */


#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <inttypes.h>
#include <string.h>
#include "app_log.h"
#include "main.h"


#if(LOG_USE_ANSI_COLORS)
#define LOG_DEFAULT_COLOR		"\e[0m"
#define LOG_BRIGHT_WHITE		"\e[97m"
static const char lvl_colors[6][6] = {"\e[94m", "\e[36m", "\e[32m", "\e[33m", "\e[31m", "\e[35m"};
#endif

static const char lvl_letters[5][7] = {"DEBUG ", "INFO  ", "NOTIF ", "WARN  ", "ERROR "};

static log_time_callback m_time_callback = NULL;
static log_out_callback  m_out_callback  = NULL;


// This function assumes that the output buffer will be long enough
static size_t print_header(enum log_lvl lvl, char* output)
{
	size_t nChars = 0;

#if(LOG_USE_TIMESTAMP)
	if(m_time_callback != NULL)
	{
		uint32_t ms = m_time_callback();
		uint32_t h = (ms/3600000)%100;		// Limit to 99h
		uint32_t m = (ms%3600000)/60000;
		uint32_t s = (ms%60000)/1000;
		ms = ms%1000;

		output[0]  = '0' + h/10;
		output[1]  = '0' + h%10;
		output[2]  = ':';
		output[3]  = '0' + m/10;
		output[4]  = '0' + m%10;
		output[5]  = ':';
		output[6]  = '0' + s/10;
		output[7]  = '0' + s%10;
		output[8]  = '.';
		output[9]  = '0' + ms/100;
		output[10] = '0' + (ms%100)/10;
		output[11] = '0' + ms%10;
		output[12] = ' ';
		nChars += 13;
	}
#endif
#if(LOG_USE_ANSI_COLORS)
	memcpy(output+nChars, lvl_colors[lvl], strlen(lvl_colors[0]));
	nChars += strlen(lvl_colors[0]);
#endif
	memcpy(output+nChars, lvl_letters[lvl], strlen(lvl_letters[0]));
	nChars += strlen(lvl_letters[0]);
#if(LOG_USE_ANSI_COLORS)
	memcpy(output+nChars, LOG_BRIGHT_WHITE, strlen(LOG_BRIGHT_WHITE));
	nChars += strlen(LOG_BRIGHT_WHITE);
#endif
	*(output+nChars) = '\0';
	return nChars;
}


// Testing shows that log_logstr takes 58% less time than log_log for a 39 chars string
// No optimizations: 9.4us instead of 22.4us	(STM32G474RE at 160MHz)
// Opt. for speed:   6.4us instead of 15.4us
// Note: these times are approximated and do not include the m_out_callback() exec time
void log_logstr(enum log_lvl lvl, const char* fmt, size_t fmtlen)
{
	char m_buffer[LOG_INTERNAL_BUFFER];			// Not static to avoid problems with interrupts
	char* p_str = m_buffer;
	size_t remaining = LOG_INTERNAL_BUFFER - strlen("\r\n") - strlen(LOG_DEFAULT_COLOR)*LOG_USE_ANSI_COLORS - 1;

	if(m_out_callback != NULL && lvl >= LOG_LEVEL_THRESHOLD)
	{
		size_t tmp = print_header(lvl, m_buffer);
		p_str += tmp;
		remaining -= tmp;

		// Copy passed string until \0 or buffer limit
		size_t cpylen = (fmtlen < remaining) ? fmtlen : remaining;
		memcpy(p_str, fmt, cpylen);
		p_str += cpylen;

#if LOG_USE_ANSI_COLORS
		memcpy(p_str, LOG_DEFAULT_COLOR, strlen(LOG_DEFAULT_COLOR));
		p_str += strlen(LOG_DEFAULT_COLOR);
#endif
		memcpy(p_str, "\r\n", strlen("\r\n")+1);
		m_out_callback(m_buffer);
	}
}


void log_log(enum log_lvl lvl, const char* fmt, ...)
{
	char m_buffer[LOG_INTERNAL_BUFFER];
	int endingElems = strlen("\r\n") + strlen(LOG_DEFAULT_COLOR)*LOG_USE_ANSI_COLORS + 1;

	if(m_out_callback != NULL && lvl >= LOG_LEVEL_THRESHOLD)
	{
		print_header(lvl, m_buffer);

		va_list va;
		va_start(va, fmt);
		vsnprintf(strchr(m_buffer, '\0'), LOG_INTERNAL_BUFFER - strlen(m_buffer) - endingElems, fmt, va);	// Account for ending elements
		va_end(va);

#if LOG_USE_ANSI_COLORS
		strcat(m_buffer, LOG_DEFAULT_COLOR);
#endif
		strcat(m_buffer, "\r\n");
		m_out_callback(m_buffer);
	}
}


void log_init(log_time_callback time_callback, log_out_callback out_callback)
{
	m_time_callback = time_callback;
	m_out_callback = out_callback;
}
