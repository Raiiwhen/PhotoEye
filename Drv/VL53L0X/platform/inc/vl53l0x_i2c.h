#ifndef __VL53L0_I2C_H
#define __VL53L0_I2C_H

#include "main.h"
#include "stm32f1xx.h"

#define VL_IIC_SCL VL_SCL
#define VL_IIC_SDA VL_SDA_WR
#define VL_READ_SDA VL_SDA_RD
#define VL_SDA_IN() {GPIOB->CRL &= 0xff0fffff; GPIOB->CRL |= 0x00400000; }
#define VL_SDA_OUT(){GPIOB->CRL &= 0xff0fffff; GPIOB->CRL |= 0x00100000; }

//状态
#define STATUS_OK       0x00
#define STATUS_FAIL     0x01

//IIC操作函数
void VL53L0X_i2c_init(void);//初始化IIC的IO口

uint8_t VL53L0X_write_byte(uint8_t address,uint8_t index,uint8_t data);              //IIC写一个8位数据
uint8_t VL53L0X_write_word(uint8_t address,uint8_t index,uint16_t data);             //IIC写一个16位数据
uint8_t VL53L0X_write_dword(uint8_t address,uint8_t index,uint32_t data);            //IIC写一个32位数据
uint8_t VL53L0X_write_multi(uint8_t address, uint8_t index,uint8_t *pdata,uint16_t count);//IIC连续写

uint8_t VL53L0X_read_byte(uint8_t address,uint8_t index,uint8_t *pdata);             //IIC读一个8位数据
uint8_t VL53L0X_read_word(uint8_t address,uint8_t index,uint16_t *pdata);            //IIC读一个16位数据
uint8_t VL53L0X_read_dword(uint8_t address,uint8_t index,uint32_t *pdata);           //IIC读一个32位数据
uint8_t VL53L0X_read_multi(uint8_t address,uint8_t index,uint8_t *pdata,uint16_t count);  //IIC连续读


#endif 


