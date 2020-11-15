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
static const char lvl_colors[6][8] = {"\e[94m", "\e[36m", "\e[32m", "\e[33m", "\e[31m", "\e[35m"};
#endif

static const char lvl_letters[5][7] = {"DEBUG ", "INFO  ", "NOTIF ", "WARN  ", "ERROR "};

static log_time_callback m_time_callback = NULL;
static log_out_callback  m_out_callback  = NULL;


// This function assumes that the output buffer will be long enough
static void print_header(enum log_lvl lvl, char* output)
{
#if(LOG_USE_TIMESTAMP)
	if(m_time_callback != NULL)
	{
		uint32_t ms = m_time_callback();
		uint32_t h = ms/3600000;
		uint32_t m = (ms%3600000)/60000;
		uint32_t s = (ms%60000)/1000;
		ms = ms%1000;
		sprintf(output, "%02lu:%02lu:%02lu.%03lu ", h, m, s, ms);
	}
#endif

#if(LOG_USE_ANSI_COLORS)
    strcat(output, lvl_colors[lvl]);
#endif
    strcat(output, lvl_letters[lvl]);
#if(LOG_USE_ANSI_COLORS)
    strcat(output, LOG_BRIGHT_WHITE);
#endif

}


// Testing shows that log_string is 10% faster than log_log,
// but further optimizations will be performed in the future
void log_string(enum log_lvl lvl, const char* fmt)
{
	char m_buffer[LOG_INTERNAL_BUFFER];
	int endingElems = sizeof("\r\n") + strlen(LOG_DEFAULT_COLOR)*LOG_USE_ANSI_COLORS;

	if(m_out_callback != NULL && lvl >= LOG_LEVEL_THRESHOLD)
	{
		m_buffer[0] = '\0';
		print_header(lvl, m_buffer);

		strncat(m_buffer, fmt, LOG_INTERNAL_BUFFER - strlen(m_buffer) - endingElems);
#if LOG_USE_ANSI_COLORS
		strcat(m_buffer, LOG_DEFAULT_COLOR);
#endif
		strcat(m_buffer, "\r\n");
		m_out_callback(m_buffer);
	}
}


void log_log(enum log_lvl lvl, const char* fmt, ...)
{
	char m_buffer[LOG_INTERNAL_BUFFER];
	int endingElems = sizeof("\r\n") + strlen(LOG_DEFAULT_COLOR)*LOG_USE_ANSI_COLORS;

	if(m_out_callback != NULL && lvl >= LOG_LEVEL_THRESHOLD)
	{
		m_buffer[0] = '\0';
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
