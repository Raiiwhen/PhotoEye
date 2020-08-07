/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

/*LEDs*/
#define LED_R BIT_ADDR(GPIOA_BASE+12,15)
//#define LED_G BIT_ADDR(GPIOB_BASE+12,3)
#define LED_B BIT_ADDR(GPIOA_BASE+12,12)

#define Beep  BIT_ADDR(GPIOA_BASE+12,11)
#define Laser BIT_ADDR(GPIOB_BASE+12,9)
#define BK		BIT_ADDR(GPIOA_BASE+12,10)

#define VL_SDA_WR	BIT_ADDR(GPIOB_BASE+12,5)
#define VL_SDA_RD	BIT_ADDR(GPIOB_BASE+8 ,5)
#define VL_SCL		BIT_ADDR(GPIOB_BASE+12,4)
#define VL_GPIO1	BIT_ADDR(GPIOB_BASE+12,6)
#define VL_XSHUT	BIT_ADDR(GPIOB_BASE+12,7)

#define LCD_RS		BIT_ADDR(GPIOA_BASE+12,8)
#define LCD_CS		BIT_ADDR(GPIOB_BASE+12,12)
#define LCD_RST		BIT_ADDR(GPIOA_BASE+12,9)

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
