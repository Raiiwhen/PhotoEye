/*
 * Copyright (c) 2006-2020, TJ Team
 *
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-03-17     唐文军       the first version
 */

#include "vl53l0x_i2c.h"
#include "i2c.h"
#include "delay.h"

#define DBG_TAG		"vl53l0x_i2c"
#define DBG_LEVEL	DBG_ERROR
#include "debug.h"

/* write n byte(s) to vl53l0x device */
uint8_t VL_IIC_Write_nByte(uint8_t SlaveAddress, uint8_t REG_Address,uint16_t len, uint8_t *buf)
{
	i2c_msg_t msgs[2] = { 0 };
	
	SlaveAddress = SlaveAddress >> 1;
	
    msgs[0].addr = SlaveAddress;
    msgs[0].flags = RT_I2C_WR;
    msgs[0].buf = &REG_Address;
    msgs[0].len = 1;
	
	msgs[1].addr = SlaveAddress;
	msgs[1].flags = RT_I2C_WR | RT_I2C_NO_START;
	msgs[1].buf = buf;
	msgs[1].len = len;
	
	if(I2C_OK != i2c_xfer(&i2c1_bus_device, msgs, 2))
    {
        LOG_E("VL_IIC_Write_nByte : 0x%X->%d byte(s) error!",REG_Address, len);
        return 1;
    }
    LOG_I("VL_IIC_Write_nByte : %d byte(s) success!",len);
    return 0;
	
}

/* read n byte(s) from vl53l0x device */
uint8_t VL_IIC_Read_nByte(uint8_t SlaveAddress, uint8_t REG_Address,uint16_t len,uint8_t *buf)
{
    i2c_msg_t msgs[3] = { 0 };
	
	SlaveAddress = SlaveAddress >> 1;
	
    msgs[0].addr = SlaveAddress;
    msgs[0].flags = RT_I2C_WR;
    msgs[0].buf = &REG_Address;
    msgs[0].len = 1;
	
	msgs[1].addr = SlaveAddress;
	msgs[1].flags = RT_I2C_RD;
	msgs[1].buf = buf;
	msgs[1].len = len - 1;

	msgs[2].addr = SlaveAddress;
	msgs[2].flags = RT_I2C_RD | RT_I2C_NO_START | RT_I2C_NO_READ_ACK;
	msgs[2].buf = &buf[len - 1];
	msgs[2].len = 1;

    if(I2C_OK != i2c_xfer(&i2c1_bus_device, msgs, 3))
    {
        LOG_E("VL_IIC_Read_nByte : 0x%X->%d byte(s) error!",REG_Address, len);
        return 1;
    }
    LOG_I("VL_IIC_Read_nByte : %d byte(s) success!",len);
    return 0;
	
}

/* write some byte(s) to vl53l0x device */
uint8_t VL53L0X_write_multi(uint8_t address, uint8_t index,uint8_t *pdata,uint16_t count)
{
	uint8_t status = STATUS_OK;

	if(VL_IIC_Write_nByte(address,index,count,pdata))
	{
	   status  = STATUS_FAIL;

	}

	return status;
}


/* read some byte(s) from vl53l0x device */
uint8_t VL53L0X_read_multi(uint8_t address,uint8_t index,uint8_t *pdata,uint16_t count)
{
	uint8_t status = STATUS_OK;

	if(VL_IIC_Read_nByte(address,index,count,pdata))
	{
	  status  = STATUS_FAIL;
	}

	return status;
}

/* write 1 byte to vl53l0x device */
uint8_t VL53L0X_write_byte(uint8_t address,uint8_t index,uint8_t data)
{
	uint8_t status = STATUS_OK;

	status = VL53L0X_write_multi(address,index,&data,1);

	return status;
}

/* write 1 word(2 bytes) to vl53l0x device */
uint8_t VL53L0X_write_word(uint8_t address,uint8_t index,uint16_t data)
{
	uint8_t status = STATUS_OK;
	
	uint8_t buffer[2];

	buffer[0] = (uint8_t)(data>>8);		// MSB
	buffer[1] = (uint8_t)(data&0xff);	// LSB
	
	if(index%2==1)
	{  
		status = VL53L0X_write_multi(address,index,&buffer[0],1);
		status = VL53L0X_write_multi(address,index,&buffer[0],1);
	}else
	{
		status = VL53L0X_write_multi(address,index,buffer,2);
	}
	
	return status;
}

/* write 1 word(4 bytes) to vl53l0x device */
uint8_t VL53L0X_write_dword(uint8_t address,uint8_t index,uint32_t data)
{
	
    uint8_t status = STATUS_OK;

    uint8_t buffer[4];	
	
	//将32位数据拆分成8位
	buffer[0] = (uint8_t)(data>>24);
	buffer[1] = (uint8_t)((data&0xff0000)>>16);
	buffer[2] = (uint8_t)((data&0xff00)>>8);
	buffer[3] = (uint8_t)(data&0xff);
	
	status = VL53L0X_write_multi(address,index,buffer,4);
	
	return status;
	
}

/* read 1 byte from vl53l0x device */
uint8_t VL53L0X_read_byte(uint8_t address,uint8_t index,uint8_t *pdata)
{
	uint8_t status = STATUS_OK;
	 
	status = VL53L0X_read_multi(address,index,pdata,1);
	
	return status;
	 
}

/* read 1 word(2 bytes) from vl53l0x device */
uint8_t VL53L0X_read_word(uint8_t address,uint8_t index,uint16_t *pdata)
{
	uint8_t status = STATUS_OK;
	
	uint8_t buffer[2];
	
	status = VL53L0X_read_multi(address,index,buffer,2);
	
	*pdata = ((uint16_t)buffer[0]<<8)+(uint16_t)buffer[1];
	
	return status;
	
}

/* read 1 word(4 bytes) from vl53l0x device */
uint8_t VL53L0X_read_dword(uint8_t address,uint8_t index,uint32_t *pdata)
{
	uint8_t status = STATUS_OK;
	
	uint8_t buffer[4];
	
	status = VL53L0X_read_multi(address,index,buffer,4);
	
	*pdata = ((uint32_t)buffer[0]<<24)+((uint32_t)buffer[1]<<16)+((uint32_t)buffer[2]<<8)+((uint32_t)buffer[3]);
	
	return status;
	
}
