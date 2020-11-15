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

#define LOG_INTERNAL_BUFFER		128		// Limit to how long a string can be
#define LOG_DEFAULT_COLOR		"\x1b[0m"

static const char lvl_letters[5][7] = {"DEBUG ", "INFO  ", "NOTIF ", "WARN  ", "ERROR "};
static const char lvl_colors[6][8] = {"\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"};

static log_time_callback m_time_callback = NULL;
static log_out_callback  m_out_callback  = NULL;


void log_log(enum log_lvl lvl, const char* fmt, ...)
{
	static char m_buffer[LOG_INTERNAL_BUFFER];

#if(LOG_USE_RUNTIME)
	if(m_out_callback != NULL && lvl >= LOG_LEVEL_THRESHOLD)
	{
		m_buffer[0] = '\0';
		if(m_time_callback != NULL)
		{
			uint32_t ms = m_time_callback();
			uint32_t h = ms/3600000;
			uint32_t m = (ms%3600000)/60000;
			uint32_t s = (ms%60000)/1000;
			ms = ms%1000;
			snprintf(m_buffer, sizeof(m_buffer), "%02lu:%02lu:%02lu.%03lu ", h, m, s, ms);
		}
#endif
#if(LOG_USE_ANSI_COLORS)
		strcat(m_buffer, lvl_colors[lvl]);
		strcat(m_buffer, lvl_letters[lvl]);
		strcat(m_buffer, "\x1b[97m");			// Bright white
#endif

		va_list va;
		va_start(va, fmt);
		vsnprintf(strchr(m_buffer, '\0'), LOG_INTERNAL_BUFFER - strlen(m_buffer) - 7, fmt, va);	// Account for elements
		va_end(va);

		strcat(m_buffer, "\x1b[0m\r\n");
		m_out_callback(m_buffer);
	}
}


void log_init(log_time_callback time_callback, log_out_callback out_callback)
{
	m_time_callback = time_callback;
	m_out_callback = out_callback;
}
