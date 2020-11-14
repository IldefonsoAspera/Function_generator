/*
 * app_log.h
 *
 *  Created on: Nov 13, 2020
 *      Author: Ilde
 */

#ifndef INC_APP_LOG_H_
#define INC_APP_LOG_H_

#include "stdint.h"

#define LOG_LEVEL_THRESHOLD		LOG_LVL_DEBUG
#define LOG_USE_ANSI_COLORS		1
#define LOG_USE_RUNTIME			1

enum log_lvl{LOG_LVL_DEBUG, LOG_LVL_INFO, LOG_LVL_NOTIF, LOG_LVL_WARNING, LOG_LVL_ERROR};

typedef uint32_t (*log_time_callback)(void);		// Returned value is expected to be ms
typedef void (*log_out_callback)(char* str);		// Callback that will print the formatted string

#define log_debug(...)		log_log(LOG_LVL_DEBUG,   __VA_ARGS__)
#define log_info(...)		log_log(LOG_LVL_INFO,    __VA_ARGS__)
#define log_notif(...)		log_log(LOG_LVL_NOTIF,   __VA_ARGS__)
#define log_warn(...)		log_log(LOG_LVL_WARNING, __VA_ARGS__)
#define log_error(...)		log_log(LOG_LVL_ERROR,   __VA_ARGS__)

void log_log(enum log_lvl lvl, const char* fmt, ...);
void log_init();

#endif /* INC_APP_LOG_H_ */
