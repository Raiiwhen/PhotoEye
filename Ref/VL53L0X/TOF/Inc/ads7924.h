/*
 * Copyright (c) 2006-2020, TJ Team
 *
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-03-17     ÌÆÎÄ¾ü       the first version
 */
#ifndef ADS7924_H_
#define ADS7924_H_

#include "stm32f1xx_hal.h"

#define ADS7924_ADDR			0x48

#define ADS7924_LLR				0x00
#define ADS7924_ULR				0xFF
#define ADS7924_RESET			0xAA	//reset device,write to reg 0x0F
#define ADS7924_INTPOL			0x00	//0x02 refer to datasheet
#define ADS7924_INTTRIG			0x00	//0x01 refer to datasheet
#define ADS7924_ACQTIME			0x02	//refer to datasheet
#define ADS7924_PWRUPTIME		0x05	//refer to datasheet
#define ADS7924_RDWR_MUL		0x80	//Reading/Writing Multiple Registers Timing Diagram

/*	Calibration control	*/
typedef enum
{
	ADS7924_CALCNTL_DIS		=	0x00,
	ADS7924_CALCNTL_EN		=	0x80
}ads7924_cali_t;
/*	Mode control	*/
typedef enum
{
	ADS7924_MODE_IDLE 		= 	0x00,
	ADS7924_MODE_AWAKE		=	0x80,
	ADS7924_MODE_MAN_SCAN	=	0xC0
}ads7924_mode_t;
/*	Channel control	*/
typedef enum
{
	ADS7924_CHANNEL_0 		= 	0x00,
	ADS7924_CHANNEL_1		=	0x01,
	ADS7924_CHANNEL_2		=	0x02,
	ADS7924_CHANNEL_3		=	0x03
}ads7924_channel_t;
/*	INT pin config control	*/
typedef enum
{
	ADS7924_READY_1 		= 	0x08,
	ADS7924_BUSY_1			=	0xC0,
	ADS7924_READY_4			=	0x18,
	ADS7924_BUSY_4			=	0x1C
}ads7924_INT_config_t;


typedef struct
{
	uint8_t		MODECNTRL;		/*	addr : 0x00	*/
	uint8_t		INTCNTRL;		/*	addr : 0x01	*/
	uint8_t		DATA0_U;		/*	addr : ...	*/
	uint8_t		DATA0_L;
	uint8_t		DATA1_U;
	uint8_t		DATA1_L;
	uint8_t		DATA2_U;
	uint8_t		DATA2_L;
	uint8_t		DATA3_U;
	uint8_t		DATA3_L;
	uint8_t		ULR0;
	uint8_t		LLR0;
	uint8_t		ULR1;
	uint8_t		LLR1;
	uint8_t		ULR2;
	uint8_t		LLR2;
	uint8_t		ULR3;
	uint8_t		LLR3;
	uint8_t		INTCONFIG;
	uint8_t		SLPCONFIG;
	uint8_t		ACQCONFIG;
	uint8_t		PWRCONFIG;
	uint8_t		Reset;			/*	addr : 0x16	*/
} ads7924_regs_t;

typedef enum
{
	ADS7924_OK		=	0x00,
	ADS7924_ERROR	=	0x01
}ads7924_err_t;

/*	ads7924 extern value	*/
extern ads7924_regs_t ads7924_status;
extern uint8_t ads7924_data_ready_flag;
extern uint16_t ads7924_data_12bits[4];

ads7924_err_t ads7924_enterAwake(void);
ads7924_err_t ads7924_startScan(void);
ads7924_err_t ads7924_enterIdle(void);
ads7924_err_t ads7924_readData(void);
ads7924_err_t ads7924_readDataTo12bitsCali(void);
ads7924_err_t ads7924_Calibration(void);
ads7924_err_t ads7924_init(void);

#endif /* ADS7924_H_ */

