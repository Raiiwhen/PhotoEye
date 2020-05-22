/*
 * Copyright (c) 2006-2020, TJ Team
 *
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-03-17     ÌÆÎÄ¾ü       the first version
 */
#ifndef DELAY_H_
#define DELAY_H_

#include "stm32f1xx_hal.h"

void hw_us_delay(uint32_t us);
void hw_ms_delay(uint32_t ms);

#endif /* DELAY_H_ */
