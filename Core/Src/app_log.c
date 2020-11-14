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
#include "app_log.h"
#include "app_uartTx.h"


#define LOG_INTERNAL_BUFFER		128		// Limit to how long a string can be

const char log_letters[5] = {'D', 'I', 'N', 'W', 'E'};


static char m_buffer[LOG_INTERNAL_BUFFER];
static log_time_callback m_time_callback = NULL;
static log_out_callback m_out_callback   = NULL;


void log_insert(enum log_lvl lvl, const char* fmt, ...)
{
	if(/*m_out_callback != NULL &&*/ lvl >= LOG_LEVEL_THRESHOLD)
	{
		char *p_str = m_buffer;
		if(m_time_callback != NULL)
		{
			uint32_t ms = m_time_callback();
			uint32_t h = ms/3600000;
			uint32_t m = (ms%3600000)/60000;
			uint32_t s = (ms%60000)/1000;
			ms = ms%1000;
			p_str += snprintf(m_buffer, sizeof(m_buffer), "%02lu:%02lu:%02lu.%03lu ", h, m, s, ms);
		}

		*p_str++ = log_letters[lvl];
		*p_str++ = ' ';

		va_list va;
		va_start(va, fmt);
		vsnprintf(p_str, LOG_INTERNAL_BUFFER - (p_str - m_buffer), fmt, va);
		va_end(va);
		m_out_callback(m_buffer);
	}
}


void log_init(log_time_callback time_callback, log_out_callback out_callback)
{
	m_time_callback = time_callback;
	m_out_callback = out_callback;
}
