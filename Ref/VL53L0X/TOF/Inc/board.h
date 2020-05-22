/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-10-26     ChenYong     first version
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include "stm32f1xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------- UART CONFIG BEGIN --------------------------*/

/** After configuring corresponding UART or UART DMA, you can use it.
  *
  * STEP 1, define macro define related to the serial port opening based on the serial port number
  *                 such as     #define BSP_USING_UATR1
  *
  * STEP 2, according to the corresponding pin of serial port, modify the related serial port information
  *                 such as     #define UART1_TX_PORT            GPIOX   ->   GPIOA
  *                             #define UART1_RX_PORT            GPIOX   ->   GPIOA
  *                             #define UART1_TX_PIN        GPIO_PIN_X   ->   GPIO_PIN_9
  *                             #define UART1_RX_PIN        GPIO_PIN_X   ->   GPIO_PIN_10
  *
  * STEP 3, if you want using SERIAL DMA, you must open it in the RT-Thread Setting.
  *                 RT-Thread Setting -> Components -> Device Drivers -> Serial Device Drivers -> Enable Serial DMA Mode
  *
  * STEP 4, according to serial port number to define serial port tx/rx DMA function in the board.h file
  *                 such as     #define BSP_UART1_RX_USING_DMA
  *
  */
  
typedef long                base_t;      /**< Nbit CPU related date type */
#define __STM32_PORT(port)  GPIO##port##_BASE
#define GET_PIN(PORTx,PIN) (base_t)((16 * ( ((base_t)__STM32_PORT(PORTx) - (base_t)GPIOA_BASE)/(0x0400UL) )) + PIN)
  

//#define BSP_USING_UART1

#if defined(BSP_USING_UART1)
#define UART1_TX_PORT       GPIOB
#define UART1_RX_PORT       GPIOB
#define UART1_TX_PIN        GPIO_PIN_6
#define UART1_RX_PIN        GPIO_PIN_7
#endif
#if defined(BSP_USING_UART2)
#define UART2_TX_PORT       GPIOX
#define UART2_RX_PORT       GPIOX
#define UART2_TX_PIN        GPIO_PIN_X
#define UART2_RX_PIN        GPIO_PIN_X
#endif
#if defined(BSP_USING_UART3)
#define UART3_TX_PORT       GPIOX
#define UART3_RX_PORT       GPIOX
#define UART3_TX_PIN        GPIO_PIN_X
#define UART3_RX_PIN        GPIO_PIN_X
#endif
#if defined(BSP_USING_UART4)
#define UART4_TX_PORT       GPIOX
#define UART4_RX_PORT       GPIOX
#define UART4_TX_PIN        GPIO_PIN_X
#define UART4_RX_PIN        GPIO_PIN_X
#endif
#if defined(BSP_USING_UART5)
#define UART5_TX_PORT       GPIOX
#define UART5_RX_PORT       GPIOX
#define UART5_TX_PIN        GPIO_PIN_X
#define UART5_RX_PIN        GPIO_PIN_X
#endif
#if defined(BSP_USING_UART6)
#define UART6_TX_PORT       GPIOX
#define UART6_RX_PORT       GPIOX
#define UART6_TX_PIN        GPIO_PIN_X
#define UART6_RX_PIN        GPIO_PIN_X
#endif
#if defined(BSP_USING_UART7)
#define UART7_TX_PORT       GPIOX
#define UART7_RX_PORT       GPIOX
#define UART7_TX_PIN        GPIO_PIN_X
#define UART7_RX_PIN        GPIO_PIN_X
#endif
#if defined(BSP_USING_UART8)
#define UART8_TX_PORT       GPIOX
#define UART8_RX_PORT       GPIOX
#define UART8_TX_PIN        GPIO_PIN_X
#define UART8_RX_PIN        GPIO_PIN_X
#endif
#if defined(BSP_USING_LPUART1)
#define LPUART1_TX_PORT     GPIOX
#define LPUART1_RX_PORT     GPIOX
#define LPUART1_TX_PIN      GPIO_PIN_X
#define LPUART1_RX_PIN      GPIO_PIN_X
#endif
/*-------------------------- UART CONFIG END --------------------------*/

/*-------------------------- SPI CONFIG BEGIN --------------------------*/

#if defined(BSP_USING_SPI1)
#define SPI1_SCK_PORT       GPIOA
#define SPI1_MOSI_PORT      GPIOA
#define SPI1_MISO_PORT      GPIOA
#define SPI1_SCK_PIN        GPIO_PIN_5
#define SPI1_MOSI_PIN       GPIO_PIN_7
#define SPI1_MISO_PIN       GPIO_PIN_6
#endif

//#define BSP_USING_SPI4

#if defined(BSP_USING_SPI4)
#define SPI4_SCK_PORT       GPIOE
#define SPI4_MOSI_PORT      GPIOE
#define SPI4_MISO_PORT      GPIOE
#define SPI4_SCK_PIN        GPIO_PIN_2
#define SPI4_MOSI_PIN       GPIO_PIN_6
#define SPI4_MISO_PIN       GPIO_PIN_5
#endif

#if defined(BSP_USING_SPI5)
#define SPI5_SCK_PORT       GPIOF
#define SPI5_MOSI_PORT      GPIOF
#define SPI5_MISO_PORT      GPIOF
#define SPI5_SCK_PIN        GPIO_PIN_7
#define SPI5_MOSI_PIN       GPIO_PIN_9
#define SPI5_MISO_PIN       GPIO_PIN_8
#endif

/*-------------------------- SPI CONFIG END --------------------------*/

/*-------------------------- I2C CONFIG BEGIN --------------------------*/
//#define BSP_USING_I2C1
//#define BSP_USING_I2C2
//#define BSP_USING_I2C3

#if defined(BSP_USING_I2C1)

#define I2C1_SCL_PORT       GPIOJ
#define I2C1_SDA_PORT       GPIOJ
#define I2C1_SCL_PIN        GPIO_PIN_8
#define I2C1_SDA_PIN        GPIO_PIN_10

#define I2C1_SCL            GET_PIN(J,  8)
#define I2C1_SDA            GET_PIN(J,  10)

#endif

#if defined(BSP_USING_I2C2)

#define I2C2_SCL_PORT       GPIOB
#define I2C2_SDA_PORT       GPIOB
#define I2C2_SCL_PIN        GPIO_PIN_14
#define I2C2_SDA_PIN        GPIO_PIN_11

#define I2C2_SCL            GET_PIN(B,  14)
#define I2C2_SDA            GET_PIN(B,  11)

#endif

#if defined(BSP_USING_I2C3)

#define I2C3_SCL_PORT       GPIOJ
#define I2C3_SDA_PORT       GPIOJ
#define I2C3_SCL_PIN        GPIO_PIN_13
#define I2C3_SDA_PIN        GPIO_PIN_12

#define I2C3_SCL            GET_PIN(J,  13)
#define I2C3_SDA            GET_PIN(J,  12)

#endif

/*-------------------------- I2C CONFIG END --------------------------*/


#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */

