/*
 * Copyright (c) 2006-2020, TJ Team
 *
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-03-17     ÌÆÎÄ¾ü       the first version
 */
#include "report.h"
#include "main.h"
 
#define DBG_TAG		"report"
#define DBG_LEVEL	DBG_LOG
#include "debug.h"

#define REPORT_DELAY	100

/*	report tool functions,implement it by yourself	*/
static void report_us_delay(uint32_t us)
{
	hw_us_delay(us);
}

static void report_io_out(report_polarity_t polar)
{
	/* PA1 --> REPORT_IO */
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, (polar == REPORT_HIGH) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static report_polarity_t report_io_in(void)
{
	/* PA1 --> REPORT_IO */
	return (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == GPIO_PIN_SET) ? REPORT_HIGH : REPORT_LOW;
}

static void report_io_mode(report_mode_t mode)
{
	/* PA1 --> REPORT_IO */
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/*Configure GPIO pin : PA1 */
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	if(mode == REPORT_IN)
	{
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	}
	else
	{
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	}
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
}
/*	report tool functions,implement it by yourself	*/



/*	if use OS , this function should be protected	*/
/*	it is suggest that this function not be interrupted	*/
report_err_t report_send_info(report_info_t info)
{
	report_io_mode(REPORT_OUT);
	report_io_out(REPORT_LOW);
	report_us_delay(REPORT_DELAY / 4);
	report_io_mode(REPORT_IN);
	report_us_delay(3 * REPORT_DELAY / 4);
	if(REPORT_HIGH != report_io_in())
	{
		LOG_E("report_send_info : get master ack failed!");
		report_io_mode(REPORT_OUT);
		return REPORT_ERROR;
	}
	report_io_mode(REPORT_OUT);
	report_io_out((report_polarity_t)((info&0x02)>>1));
	report_us_delay(REPORT_DELAY);
	report_io_out((report_polarity_t)(info&0x01));
	report_us_delay(REPORT_DELAY);	
	report_io_out(REPORT_HIGH);	
	return REPORT_OK;
}
