/*
 * Copyright (c) 2006-2020, TJ Team
 *
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-03-17     ÌÆÎÄ¾ü       the first version
 */
#include "ads7924.h"
#include "main.h"
#include "i2c.h"

#define DBG_TAG		"ads7924"
#define DBG_LEVEL	DBG_LOG
#include "debug.h"

uint8_t ads7924_data_ready_flag = 0;
uint16_t ads7924_data_12bits[4] = { 0 };
static uint16_t ads7924_cali = 0;
/*	ads7924 rigisters' value	*/
ads7924_regs_t ads7924_status =
{
	ADS7924_MODE_IDLE | ADS7924_CHANNEL_0,
	0x00,
	
	0x00,									/* DATA0_U */
	0x00,									/* DATA0_L */
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,									/* DATA3_U */
	0x00,									/* DATA3_L */
	
	0xFF,									/* ULR0 */
	0x00,									/* LLR0 */
	0xFF,
	0x00,
	0xFF,
	0x00,
	0xFF,									/* ULR3 */
	0x00,									/* ULR3 */
	
	ADS7924_READY_4 | ADS7924_INTPOL | ADS7924_INTTRIG,
	0x00,
	ADS7924_ACQTIME,
	ADS7924_PWRUPTIME,
	ADS7924_ADDR
};

/*	ads7924 us delay tool function,implement it by yourself	*/
static void ads7924_delay_us(uint32_t us)
{
	hw_us_delay(us);
}

/*	Read Multiple Registers	*/
static ads7924_err_t ads7924_readRegs(uint8_t startAddr, uint8_t len, uint8_t *buf)
{
    i2c_msg_t msgs[3] = { 0 };
	
	startAddr |= (uint8_t)ADS7924_RDWR_MUL;
	
    msgs[0].addr = ADS7924_ADDR;
    msgs[0].flags = RT_I2C_WR;
    msgs[0].buf = &startAddr;
    msgs[0].len = 1;
	
	msgs[1].addr = ADS7924_ADDR;
	msgs[1].flags = RT_I2C_RD;
	msgs[1].buf = buf;
	msgs[1].len = len - 1;

	msgs[2].addr = ADS7924_ADDR;
	msgs[2].flags = RT_I2C_RD | RT_I2C_NO_START | RT_I2C_NO_READ_ACK;
	msgs[2].buf = &buf[len - 1];
	msgs[2].len = 1;

    if(I2C_OK != i2c_xfer(&i2c1_bus_device, msgs, 3))
    {
        LOG_E("ads7924_readRegs : 0x%X->%d byte(s) error!",startAddr, len);
        return ADS7924_ERROR;
    }
    LOG_I("ads7924_readRegs : %d byte(s) success!",len);
    return ADS7924_OK;
}
/*	Write Multiple Registers	*/
static ads7924_err_t ads7924_writeRegs(uint8_t startAddr, uint8_t len, uint8_t *buf)
{
	i2c_msg_t msgs[2] = { 0 };
	
	startAddr |= (uint8_t)ADS7924_RDWR_MUL;
	
    msgs[0].addr = ADS7924_ADDR;
    msgs[0].flags = RT_I2C_WR;
    msgs[0].buf = &startAddr;
    msgs[0].len = 1;
	
	msgs[1].addr = ADS7924_ADDR;
	msgs[1].flags = RT_I2C_WR | RT_I2C_NO_START;
	msgs[1].buf = buf;
	msgs[1].len = len;
	
	if(I2C_OK != i2c_xfer(&i2c1_bus_device, msgs, 2))
    {
        LOG_E("ads7924_writeRegs : 0x%X->%d byte(s) error!",startAddr, len);
        return ADS7924_ERROR;
    }
    LOG_I("ads7924_writeRegs : %d byte(s) success!",len);
    return ADS7924_OK;
}

/*	ads7924 reset function	*/
static ads7924_err_t ads7924_reset(void)
{
	uint8_t reset = (uint8_t)ADS7924_RESET;
	if(ADS7924_OK != ads7924_writeRegs(0x16, 1, &reset))
	{
		LOG_E("ads7924_reset : reset failed!");
		return ADS7924_ERROR;
	}
	ads7924_delay_us(100);
	return ADS7924_OK;
}
/*	ads7924 refresh rigisters function	*/
static ads7924_err_t ads7924_refreshRegs(void)
{
	uint8_t buf[8] = { 0 };
	buf[0] = ads7924_status.MODECNTRL;
	buf[1] = ads7924_status.INTCNTRL;
	if(ADS7924_OK != ads7924_writeRegs(0x00, 2, buf))
	{
		LOG_E("ads7924_refreshRegs : refresh 0x01~0x02 reg failed!");
		goto ads7924_err;
	}
	buf[0] = ADS7924_ULR;	buf[1] = ADS7924_LLR;
	buf[2] = ADS7924_ULR;	buf[3] = ADS7924_LLR;
	buf[4] = ADS7924_ULR;	buf[5] = ADS7924_LLR;
	buf[6] = ADS7924_ULR;	buf[7] = ADS7924_LLR;
	if(ADS7924_OK != ads7924_writeRegs(0x0A, 8, buf))
	{
		LOG_E("ads7924_refreshRegs : refresh 0x0A~0x11 reg failed!");
		goto ads7924_err;
	}
	buf[0] = ads7924_status.INTCONFIG;	buf[1] = ads7924_status.SLPCONFIG;
	buf[2] = ads7924_status.ACQCONFIG;	buf[3] = ads7924_status.PWRCONFIG;
	if(ADS7924_OK != ads7924_writeRegs(0x12, 4, buf))
	{
		LOG_E("ads7924_refreshRegs : refresh 0x12~0x15 reg failed!");
		goto ads7924_err;
	}
	return ADS7924_OK;
ads7924_err:
	LOG_E("ads7924_refreshRegs : ads7924 refresh rigisters failed!");
	return ADS7924_ERROR;
}
/*	ads7924 enter awake mode function	*/
ads7924_err_t ads7924_enterAwake(void)
{
	ads7924_status.MODECNTRL &= 0x03;
	ads7924_status.MODECNTRL |= ADS7924_MODE_AWAKE;
	if(ADS7924_OK != ads7924_writeRegs(0x00, 1, &(ads7924_status.MODECNTRL)))
	{
		LOG_E("ads7924_enterAwake : enter awake mode failed!");
		return ADS7924_ERROR;
	}	
	return ADS7924_OK;	
}
/*	ads7924 start conversion function	*/
ads7924_err_t ads7924_startScan(void)
{
	ads7924_status.MODECNTRL &= 0x03;
	ads7924_status.MODECNTRL |= ADS7924_MODE_MAN_SCAN;
	if(ADS7924_OK != ads7924_writeRegs(0x00, 1, &(ads7924_status.MODECNTRL)))
	{
		LOG_E("ads7924_startScan : start conversion failed!");
		return ADS7924_ERROR;
	}	
	return ADS7924_OK;	
}

/*	ads7924 enter idle mode function	*/
ads7924_err_t ads7924_enterIdle(void)
{
	ads7924_status.MODECNTRL &= 0x03;
	ads7924_status.MODECNTRL |= ADS7924_MODE_IDLE;
	if(ADS7924_OK != ads7924_writeRegs(0x00, 1, &(ads7924_status.MODECNTRL)))
	{
		LOG_E("ads7924_enterIdle : enter idle mode failed!");
		return ADS7924_ERROR;
	}	
	return ADS7924_OK;	
}

ads7924_err_t ads7924_readData(void)
{
	uint8_t buf[8] = { 0 };
	ads7924_data_ready_flag = 0;
	if(ADS7924_OK != ads7924_readRegs(0x02, 8, buf))
	{
		LOG_E("ads7924_readData : readData failed!");
		return ADS7924_ERROR;
	}
	ads7924_status.DATA0_U = buf[0];	ads7924_status.DATA0_L = buf[1];
	ads7924_status.DATA1_U = buf[2];	ads7924_status.DATA1_L = buf[3];
	ads7924_status.DATA2_U = buf[4];	ads7924_status.DATA2_L = buf[5];
	ads7924_status.DATA3_U = buf[6];	ads7924_status.DATA3_L = buf[7];
	
	return ADS7924_OK;
}
/* convert read data to calibrated 12bits data	*/
ads7924_err_t ads7924_readDataTo12bitsCali(void)
{
	ads7924_data_12bits[0] = (uint16_t)(ads7924_status.DATA0_L>>4)|
								(((uint16_t)ads7924_status.DATA0_U)<<4);
	ads7924_data_12bits[1] = (uint16_t)(ads7924_status.DATA1_L>>4)|
								(((uint16_t)ads7924_status.DATA1_U)<<4);
	ads7924_data_12bits[2] = (uint16_t)(ads7924_status.DATA2_L>>4)|
								(((uint16_t)ads7924_status.DATA2_U)<<4);
	ads7924_data_12bits[3] = (uint16_t)(ads7924_status.DATA3_L>>4)|
								(((uint16_t)ads7924_status.DATA3_U)<<4);
	
	for(uint8_t i = 0;i < 4;i++)
	{
		ads7924_data_12bits[i] = (ads7924_data_12bits[i] >= ads7924_cali) ?
									ads7924_data_12bits[i] - ads7924_cali : 0;	
	}
	return ADS7924_OK;
}

ads7924_err_t ads7924_Calibration(void)
{
	uint8_t count = 0xFF;
	/* config mode Awake Channel 3	*/
	ads7924_status.MODECNTRL = ADS7924_MODE_AWAKE | ADS7924_CHANNEL_3;
	if(ADS7924_OK != ads7924_writeRegs(0x00, 1, &(ads7924_status.MODECNTRL)))
	{
		LOG_E("ads7924_Calibration : config mode Awake Channel3 failed!");
		goto ads7924_err;
	}	
	/* config Calibration mode	*/
	ads7924_status.PWRCONFIG = ADS7924_CALCNTL_EN | ADS7924_PWRUPTIME;
	if(ADS7924_OK != ads7924_writeRegs(0x15, 1, &(ads7924_status.PWRCONFIG)))
	{
		LOG_E("ads7924_Calibration : config Calibration mode failed!");
		goto ads7924_err;
	}
	/*	start conversion	*/
	if(ADS7924_OK != ads7924_startScan())
	{
		LOG_E("ads7924_Calibration : start conversion failed!");
		goto ads7924_err;
	}
	/*	wait for conversion complete	*/
	while(count--)
	{
		if(ads7924_data_ready_flag)
			break;
		if(!count)
		{
			LOG_E("ads7924_Calibration : timeout!");
			goto ads7924_err;
		}
		ads7924_delay_us(4);
	}
	/*	read data	*/
	if(ADS7924_OK != ads7924_readData())
	{
		LOG_E("ads7924_Calibration : ads7924_readData failed!");
		goto ads7924_err;
	}
	ads7924_cali = (uint16_t)(ads7924_status.DATA3_L>>4)|
							(((uint16_t)ads7924_status.DATA3_U)<<4);
	return ADS7924_OK;	
ads7924_err:
	LOG_E("ads7924_Calibration : ads7924 refresh rigisters failed!");
	return ADS7924_ERROR;	
}

ads7924_err_t ads7924_init(void)
{
	/* first reset ads7924	*/
	if(ADS7924_OK != ads7924_reset())
		goto ads7924_err;
	LOG_I("ads7924_init : ads7924_reset!");
	/* second refresh ads7924 settings	*/
	if(ADS7924_OK != ads7924_refreshRegs())
		goto ads7924_err;
	LOG_I("ads7924_init : ads7924_refreshRegs!");
	/* thrid ads7924 Calibration	*/
	if(ADS7924_OK != ads7924_Calibration())
		goto ads7924_err;
	LOG_I("ads7924_init : ads7924_Calibration!");
	/* forth recover ads7924 settings	*/
	ads7924_status.MODECNTRL = ADS7924_MODE_IDLE | ADS7924_CHANNEL_0,
	ads7924_status.PWRCONFIG = ADS7924_CALCNTL_DIS | ADS7924_PWRUPTIME;
	if(ADS7924_OK != ads7924_refreshRegs())
		goto ads7924_err;
	LOG_I("ads7924_init : settings_recovered!");
	LOG_I("ads7924_init : init finished!");
	return ADS7924_OK;
ads7924_err:
	LOG_E("ads7924_init : ads7924 init failed!");
	return ADS7924_ERROR;
}


