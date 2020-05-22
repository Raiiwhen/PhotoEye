/*
 * Copyright (c) 2006-2020, TJ Team
 *
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-03-17     ÌÆÎÄ¾ü       the first version
 */
#ifndef REPORT_H_
#define REPORT_H_

#include "stm32f1xx_hal.h"

typedef enum
{
	REPORT_START	=	0x01,
	REPORT_SUCCESS	=	0x02,
	REPORT_FAIL		=	0x03
}report_info_t;

typedef enum
{
	REPORT_OK		=	0x00,
	REPORT_ERROR	=	0x01
}report_err_t;

typedef enum
{
	REPORT_LOW  	= 	0,
	REPORT_HIGH 	= 	1,
}report_polarity_t;

typedef enum
{
	REPORT_OUT 		= 	0,
	REPORT_IN  		= 	1
}report_mode_t;

report_err_t report_send_info(report_info_t info);

#endif

