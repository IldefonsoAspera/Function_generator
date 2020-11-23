/*
 * app_log.h
 *
 *  Created on: Nov 13, 2020
 *      Author: Ilde
 */

#ifndef INC_APP_LOG_H_
#define INC_APP_LOG_H_

#include "stdint.h"

/********* USER CONFIGURABLE DEFINES **********/
#define LOG_LEVEL_THRESHOLD		LOG_LVL_DEBUG	// Lowest priority that will be accepted. Must be an item from enum log_lvl
#define LOG_USE_ANSI_COLORS		1
#define LOG_USE_TIMESTAMP		1
#define LOG_INTERNAL_BUFFER		100				// Limit to how long a string can be. Minimum 36 chars when using timestamps and colors
/******* END USER CONFIGURABLE DEFINES ********/


enum log_lvl{LOG_LVL_DEBUG, LOG_LVL_INFO, LOG_LVL_NOTIF, LOG_LVL_WARNING, LOG_LVL_ERROR};

typedef uint32_t (*log_time_callback)(void);	// Callback to get the number of milliseconds since start
typedef void (*log_out_callback)(char* str);	// Callback that will print the formatted string

// Redirects to optimized function if called with only a string
// Limits function call to maximum 6 arguments, but should be good enough
#define _LOG_GET_MACRO(_1,_2,_3,_4,_5,_6,NAME,...) NAME
#define log_debug(...) _LOG_GET_MACRO(__VA_ARGS__, log_log, log_log, log_log, log_log, log_log, log_string)(LOG_LVL_DEBUG, __VA_ARGS__)
#define log_notif(...) _LOG_GET_MACRO(__VA_ARGS__, log_log, log_log, log_log, log_log, log_log, log_string)(LOG_LVL_NOTIF, __VA_ARGS__)
#define log_warn(...)  _LOG_GET_MACRO(__VA_ARGS__, log_log, log_log, log_log, log_log, log_log, log_string)(LOG_LVL_WARNING, __VA_ARGS__)
#define log_error(...) _LOG_GET_MACRO(__VA_ARGS__, log_log, log_log, log_log, log_log, log_log, log_string)(LOG_LVL_ERROR, __VA_ARGS__)

#define log_string(lvl, fmt)	log_logstr(lvl, fmt, sizeof(fmt)-1)

void log_logstr(enum log_lvl lvl, const char* fmt, size_t fmtlen);
void log_log(enum log_lvl lvl, const char* fmt, ...);
void log_init(log_time_callback time_callback, log_out_callback out_callback);

#endif /* INC_APP_LOG_H_ */
