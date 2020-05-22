/*
 * Copyright (c) 2006-2020, TJ Team
 *
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-03-17     ÌÆÎÄ¾ü       the first version
 */
#ifndef DEBUG_H_
#define DEBUG_H_


/* DEBUG level */
#define DBG_ERROR           0
#define DBG_WARNING         1
#define DBG_INFO            2
#define DBG_LOG             3

#include <stdio.h>

#ifndef DBG_TAG
#define DBG_TAG "DBG_TAG"
#warning "please define the DEBUG_TAG before using DEBUG"
#endif
#ifndef DBG_LEVEL
#define DBG_LEVEL DBG_LOG
#warning "please define the DEBUG_LEVEL before using DEBUG"
#endif

#if (DBG_LEVEL >= DBG_ERROR)	
#define LOG_E(fmt, ...)									\
		do												\
		{												\
			printf("[E/");								\
			printf(DBG_TAG);							\
			printf("] ");								\
			printf(fmt, ##__VA_ARGS__);					\
			printf("\r\n");								\
		}												\
		while(0)
#else
#define LOG_E(...)
#endif				
		
#if (DBG_LEVEL >= DBG_WARNING)				
#define LOG_W(fmt, ...)									\
		do												\
		{												\
			printf("[W/");								\
			printf(DBG_TAG);							\
			printf("] ");								\
			printf(fmt, ##__VA_ARGS__);					\
			printf("\r\n");								\
		}												\
		while(0)
#else
#define LOG_W(...)
#endif		
		
#if (DBG_LEVEL >= DBG_INFO)			
#define LOG_I(fmt, ...)									\
		do												\
		{												\
			printf("[I/");								\
			printf(DBG_TAG);							\
			printf("] ");								\
			printf(fmt, ##__VA_ARGS__);					\
			printf("\r\n");								\
		}												\
		while(0)	
#else
#define LOG_I(...)
#endif
		
#if (DBG_LEVEL >= DBG_LOG)			
#define LOG_D(fmt, ...)									\
		do												\
		{												\
			printf("[I/");								\
			printf(DBG_TAG);							\
			printf("] ");								\
			printf(fmt, ##__VA_ARGS__);					\
			printf("\r\n");								\
		}												\
		while(0)	
#else
#define LOG_D(...)
#endif

#endif /* DEBUG_H_ */

