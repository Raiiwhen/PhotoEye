/*
 * Copyright (c) 2006-2020, TJ Team
 *
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-03-17     唐文军       the first version
 */

#include "vl53l0x_i2c.h"
#include "I2C_Soft.h"


/* write n byte(s) to vl53l0x device */
uint8_t VL_IIC_Write_nByte(uint8_t SlaveAddress, uint8_t REG_Address,uint16_t len, uint8_t *buf)
{
	IIC1_Start();
	IIC1_Send_Byte(SlaveAddress);//发写命令
	if(IIC1_Wait_Ack()) 
	{
		IIC1_Stop();//释放总线
		return 1;//没应答则退出
	}
	IIC1_Send_Byte(REG_Address);
	IIC1_Wait_Ack();
	while(len--)
	{
		IIC1_Send_Byte(*buf++);//发送buff的数据
		IIC1_Wait_Ack();	
	}
	IIC1_Stop();//释放总线

	return 0;
}

/* read n byte(s) from vl53l0x device */
uint8_t VL_IIC_Read_nByte(uint8_t SlaveAddress, uint8_t REG_Address,uint16_t len,uint8_t *buf)
{
	IIC1_Start();
	IIC1_Send_Byte(SlaveAddress);//发写命令
	if(IIC1_Wait_Ack()) 
	{
		IIC1_Stop();//释放总线
		return 1;//没应答则退出
	}
	IIC1_Send_Byte(REG_Address);
	IIC1_Wait_Ack();

	IIC1_Start();
	IIC1_Send_Byte(SlaveAddress|0x01);//发读命令
	IIC1_Wait_Ack();
	while(len)
	{
		if(len==1)
		{
			*buf = IIC1_Read_Byte(0);
		}
		else
		{
			*buf = IIC1_Read_Byte(1);
		}
		buf++;
		len--;
	}
	IIC1_Stop();//释放总线

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
