/*
 * Copyright (c) 2006-2020, TJ Team
 *
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-03-17     唐文军       the first version
 */
#include "main.h"
#include "i2c.h"

#define DBG_TAG		"i2c"
#define DBG_LEVEL	DBG_LOG
#include "debug.h"

#ifndef	LOG_E
#define LOG_E(fmt, ...)
#endif

#ifndef	LOG_W
#define LOG_W(fmt, ...)
#endif

#ifndef	LOG_I
#define LOG_I(fmt, ...)
#endif

#ifdef BSP_USING_I2C1
i2c_bus_device_t i2c1_bus_device;
#endif

#ifdef BSP_USING_I2C2
i2c_bus_device_t i2c2_bus_device;
#endif


/*产生IIC起始信号*/
static void I2C_Start(const i2c_bus_ops_t * i2c_ops)
{ 
    i2c_ops->i2c_sda_mode(I2C_OUT);              //sda线输出
    i2c_ops->i2c_sda_out(I2C_HIGH);
    i2c_ops->i2c_scl_out(I2C_HIGH);
    i2c_ops->i2c_delay_us(2);
    i2c_ops->i2c_sda_out(I2C_LOW);               //START:when CLK is high,DATA change form high to low
    i2c_ops->i2c_delay_us(2);
    i2c_ops->i2c_scl_out(I2C_LOW);               //钳住I2C总线，准备发送或接收数据
}
/*产生IIC停止信号*/
static void I2C_Stop(const i2c_bus_ops_t * i2c_ops)
{
    i2c_ops->i2c_sda_mode(I2C_OUT);              //sda线输出
    i2c_ops->i2c_scl_out(I2C_LOW);
    i2c_ops->i2c_sda_out(I2C_LOW);               //STOP:when CLK is high DATA change form low to high
    i2c_ops->i2c_delay_us(4);
    i2c_ops->i2c_scl_out(I2C_HIGH);
    i2c_ops->i2c_sda_out(I2C_HIGH);              //发送I2C总线结束信号
    i2c_ops->i2c_delay_us(4);
}
/*
  *等待应答信号到来
  *返回值：-I2C_ERROR，接收应答失败
 *       I2C_OK，     接收应答成功
*/
static i2c_err_t I2C_Wait_Ack(const i2c_bus_ops_t * i2c_ops,uint32_t timeout)
{
    uint32_t ucErrTime=0;
    i2c_ops->i2c_sda_mode(I2C_IN);          	//SDA设置为输入
    i2c_ops->i2c_sda_out(I2C_HIGH);    i2c_ops->i2c_delay_us(1);
    i2c_ops->i2c_scl_out(I2C_HIGH);    i2c_ops->i2c_delay_us(1);
    while(i2c_ops->i2c_sda_in())
    {
        ucErrTime++;
        if(ucErrTime>timeout)
        {
            I2C_Stop(i2c_ops);
            LOG_E("I2C miss ACK!");
            return I2C_ERROR;
        }
    }
    i2c_ops->i2c_scl_out(I2C_LOW);              //时钟输出0
    return I2C_OK;
}
/*产生ACK应答*/
static void I2C_Ack(const i2c_bus_ops_t * i2c_ops)
{
    i2c_ops->i2c_scl_out(I2C_LOW);
    i2c_ops->i2c_sda_mode(I2C_OUT);
    i2c_ops->i2c_sda_out(I2C_LOW);
    i2c_ops->i2c_delay_us(2);
    i2c_ops->i2c_scl_out(I2C_HIGH);
    i2c_ops->i2c_delay_us(2);
    i2c_ops->i2c_scl_out(I2C_LOW);
}
/*不产生ACK应答*/
static void I2C_NAck(const i2c_bus_ops_t * i2c_ops)
{
    i2c_ops->i2c_scl_out(I2C_LOW);
    i2c_ops->i2c_sda_mode(I2C_OUT);
    i2c_ops->i2c_sda_out(I2C_HIGH);
    i2c_ops->i2c_delay_us(2);
    i2c_ops->i2c_scl_out(I2C_HIGH);
    i2c_ops->i2c_delay_us(2);
    i2c_ops->i2c_scl_out(I2C_LOW);
}
/*
 * IIC发送一个字节
  *  返回从机有无应答
 * 1，有应答
 * 0，无应答
*/
static void I2C_Send_Byte(const i2c_bus_ops_t * i2c_ops,uint8_t txd)
{
    uint8_t t;
    i2c_ops->i2c_sda_mode(I2C_OUT);
    i2c_ops->i2c_scl_out(I2C_LOW);			//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {
        i2c_ops->i2c_sda_out((i2c_polarity_t)((txd&0x80)>>7));
        txd<<=1;
        i2c_ops->i2c_delay_us(2);
        i2c_ops->i2c_scl_out(I2C_HIGH);
        i2c_ops->i2c_delay_us(2);
        i2c_ops->i2c_scl_out(I2C_LOW);
        i2c_ops->i2c_delay_us(2);
    }
}
/*读1个字节，ack=1时，发送ACK，ack=0，发送nACK*/
static uint8_t I2C_Read_Byte(const i2c_bus_ops_t * i2c_ops,unsigned char ack)
{
    uint8_t i,receive=0;
    i2c_ops->i2c_sda_mode(I2C_IN);  		//SDA设置为输入
    for(i=0;i<8;i++ )
    {
        i2c_ops->i2c_scl_out(I2C_LOW);
        i2c_ops->i2c_delay_us(2);
        i2c_ops->i2c_scl_out(I2C_HIGH);
        receive<<=1;
        if(i2c_ops->i2c_sda_in())
            receive++;
        i2c_ops->i2c_delay_us(1);
    }
    if (!ack)
        I2C_NAck(i2c_ops);//发送nACK
    else
        I2C_Ack(i2c_ops); //发送ACK
    return receive;
}

i2c_err_t i2c_xfer(	i2c_bus_device_t *bus,
					i2c_msg_t msgs[],
					uint32_t num)
{
	if(NULL == bus || NULL == msgs || NULL == bus->i2c_ops)
	{
		LOG_E("i2c bus input NULL pointer!");
		return I2C_ERROR;
	}	
	
    for(uint32_t i = 0; i < num; i ++)
    {
        /* Send Start Signal */
        if(!(msgs[i].flags&RT_I2C_NO_START))
        {
            I2C_Start(bus->i2c_ops);

            /* try to send device address */
            uint8_t addr_sent = 0;
            uint8_t addrM_sent = 0xF0,addrL_sent = 0;

            if(msgs[i].flags&RT_I2C_ADDR_10BIT)
            {
                /* modify 10bit sent addr */
                if(msgs[i].flags&RT_I2C_RD)
                    addrM_sent |= (uint8_t)(((msgs[i].addr&0x300)>>7)|RT_I2C_RD);
                else
                    addrM_sent |= (uint8_t)((msgs[i].addr&0x300)>>7);
                addrL_sent = (uint8_t)msgs[i].addr;
            }else {
                /* modify 7bit sent addr */
                if(msgs[i].flags&RT_I2C_RD)
                    addr_sent = (uint8_t)((msgs[i].addr<<1)|RT_I2C_RD);
                else
                    addr_sent = (uint8_t)(msgs[i].addr<<1);
            }
            for(uint32_t j = 0; j < bus->retries; j ++)
            {
                if(msgs[i].flags&RT_I2C_ADDR_10BIT)
                {
                    I2C_Send_Byte(bus->i2c_ops,addrM_sent);
                    I2C_Wait_Ack(bus->i2c_ops,bus->timeout);
                    I2C_Send_Byte(bus->i2c_ops,addrL_sent);
                }else {
                    I2C_Send_Byte(bus->i2c_ops,addr_sent);
                }
                /* check iic bus ACK */
                if(I2C_Wait_Ack(bus->i2c_ops,bus->timeout)){
                    if(j == (bus->retries - 1))
                    {
                        LOG_E("i2c bus connect i2c device 0x%X time out!",msgs[i].addr);
                        goto err;
                    }
                    LOG_W("i2c bus connect i2c device 0x%X miss ACK: time(%d)!",msgs[i].addr,j);
                    continue;
                }else {
                    break;
                }
            }
            /* try to send device address */
        }
        /* Send Start Signal */

        /* read or write data */
        uint16_t count = 0;

        if(msgs[i].flags&RT_I2C_RD)
        {
            if(msgs[i].flags&RT_I2C_NO_READ_ACK)                //  Read with no ACK
                while(count < msgs[i].len)
                    msgs[i].buf[count++] = I2C_Read_Byte(bus->i2c_ops, 0);
            else                                                //  Read with ACK
                while(count < msgs[i].len)
                    msgs[i].buf[count++] = I2C_Read_Byte(bus->i2c_ops, 1);
        }else {
            if(msgs[i].flags&RT_I2C_IGNORE_NACK)                //  Write and ignore ACK
                while(count < msgs[i].len)
                    I2C_Send_Byte(bus->i2c_ops, msgs[i].buf[count++]);
            else                                                //  Write
                while(count < msgs[i].len)
                {
                    I2C_Send_Byte(bus->i2c_ops, msgs[i].buf[count++]);
                    if(I2C_Wait_Ack(bus->i2c_ops,bus->timeout))
                    {
                        LOG_E("i2c bus send msg %d byte %d (0x%X) ACK missed!",
                                i + 1, count, msgs[i].buf[count - 1]);
                        goto err;
                    }
                }
        }
        /* read or write data */
    }
    I2C_Stop(bus->i2c_ops);
    return I2C_OK;
err:
    I2C_Stop(bus->i2c_ops);
    return I2C_ERROR;
}

/* i2c1 register needed */
#ifdef BSP_USING_I2C1

#include "delay.h"

static void i2c1_scl_out(i2c_polarity_t polar)
{
	/* PB3  --> I2C_SCL */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, (polar == I2C_HIGH) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
	
static void i2c_sda_out(i2c_polarity_t polar)
{
	/* PA15 --> I2C_SDA */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, (polar == I2C_HIGH) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}	

static i2c_polarity_t i2c_sda_in(void)
{
	/* PA15 --> I2C_SDA */
	return (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_SET) ? I2C_HIGH : I2C_LOW;
}

static void i2c_sda_mode(i2c_mode_t mode)
{
	/* PA15 --> I2C_SDA */
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/*Configure GPIO pin : PA15 */
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	if(mode == I2C_IN)
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	else
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
}

static void i2c_delay_us(uint32_t value)
{
	hw_us_delay(value);
}

const i2c_bus_ops_t i2c1_bus_ops =
{
	i2c1_scl_out,
	i2c_sda_out,
	i2c_sda_in,
	i2c_sda_mode,
	i2c_delay_us
};

static void i2c1_bus_init(void)
{
	i2c1_bus_device.timeout = 0xFFF;
	i2c1_bus_device.retries = 0x03;
	i2c1_bus_device.i2c_ops = &i2c1_bus_ops;
	i2c1_bus_device.i2c_ops->i2c_scl_out(I2C_HIGH);
	i2c1_bus_device.i2c_ops->i2c_sda_mode(I2C_OUT);
	i2c1_bus_device.i2c_ops->i2c_sda_out(I2C_HIGH);	
}

#endif
/* i2c1 register needed */


/* i2c2 register needed */
#ifdef BSP_USING_I2C2
/*add your code as I2C1 */
#endif
/* i2c2 register needed */


i2c_err_t i2c_bus_init(void)
{
#ifdef BSP_USING_I2C1
	i2c1_bus_init();	
#endif

#ifdef BSP_USING_I2C2
/*add your code as I2C1 shown*/
#endif
    return I2C_OK;
}
