#include "vl53l0x_i2c.h"
#include "main.h"
void delay_us(int us){
	for (int i = 0; i < 72;i++){}
}

//VL53L0X I2C初始化
void VL53L0X_i2c_init(void)
{
	VL_IIC_SDA=1;	  	  
	VL_IIC_SCL=1;
}

//产生IIC起始信号
void VL_IIC_Start(void)
{
	VL_SDA_OUT();//sda线输出
	VL_IIC_SDA=1;	  	  
	VL_IIC_SCL=1;
	delay_us(4);
 	VL_IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	VL_IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}

//产生IIC停止信号
void VL_IIC_Stop(void)
{
	VL_SDA_OUT();//sda线输出
	VL_IIC_SCL=0;
	VL_IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	VL_IIC_SCL=1; 
	VL_IIC_SDA=1;//发送I2C总线结束信号
	delay_us(4);							   	
}

//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t VL_IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	VL_SDA_IN();  //SDA设置为输入  
	VL_IIC_SDA=1;delay_us(1);	   
	VL_IIC_SCL=1;delay_us(1);	 
	while(VL_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			VL_IIC_Stop();
			return 1;
		}
	}
	VL_IIC_SCL=0;//时钟输出0 	   
	return 0;  
}

//产生ACK应答
void VL_IIC_Ack(void)
{
	VL_IIC_SCL=0;
	VL_SDA_OUT();
	VL_IIC_SDA=0;
	delay_us(2);
	VL_IIC_SCL=1;
	delay_us(2);
	VL_IIC_SCL=0;
}

//不产生ACK应答		    
void VL_IIC_NAck(void)
{
	VL_IIC_SCL=0;
	VL_SDA_OUT();
	VL_IIC_SDA=1;
	delay_us(2);
	VL_IIC_SCL=1;
	delay_us(2);
	VL_IIC_SCL=0;
}

//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void VL_IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	VL_SDA_OUT(); 	    
    VL_IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
		if((txd&0x80)>>7)
			VL_IIC_SDA=1;
		else
			VL_IIC_SDA=0;
		txd<<=1; 	  
		delay_us(2);  
		VL_IIC_SCL=1;
		delay_us(2); 
		VL_IIC_SCL=0;	
		delay_us(2);
    }	 
} 

//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t VL_IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	VL_SDA_IN();//SDA设置为输入
	for(i=0;i<8;i++ )
	{
		VL_IIC_SCL=0; 
		delay_us(4);
	  VL_IIC_SCL=1;
		receive<<=1;
		if(VL_READ_SDA)receive++;   
	  delay_us(4); //1
	}					 
	if (!ack)
		VL_IIC_NAck();//发送nACK
	else
		VL_IIC_Ack(); //发送ACK   
	return receive;
}

//IIC写一个字节数据
uint8_t VL_IIC_Write_1Byte(uint8_t SlaveAddress, uint8_t REG_Address,uint8_t REG_data)
{
	VL_IIC_Start();
	VL_IIC_Send_Byte(SlaveAddress);
	if(VL_IIC_Wait_Ack())
	{
		VL_IIC_Stop();//释放总线
		return 1;//没应答则退出

	}
	VL_IIC_Send_Byte(REG_Address);
	VL_IIC_Wait_Ack();	
	VL_IIC_Send_Byte(REG_data);
	VL_IIC_Wait_Ack();	
	VL_IIC_Stop();

	return 0;
}

//IIC读一个字节数据
uint8_t VL_IIC_Read_1Byte(uint8_t SlaveAddress, uint8_t REG_Address,uint8_t *REG_data)
{
	VL_IIC_Start();
	VL_IIC_Send_Byte(SlaveAddress);//发写命令
	if(VL_IIC_Wait_Ack())
	{
		 VL_IIC_Stop();//释放总线
		 return 1;//没应答则退出
	}		
	VL_IIC_Send_Byte(REG_Address);
	VL_IIC_Wait_Ack();
	VL_IIC_Start(); 
	VL_IIC_Send_Byte(SlaveAddress|0x01);//发读命令
	VL_IIC_Wait_Ack();
	*REG_data = VL_IIC_Read_Byte(0);
	VL_IIC_Stop();

	return 0;
}

//IIC写n字节数据
uint8_t VL_IIC_Write_nByte(uint8_t SlaveAddress, uint8_t REG_Address,uint16_t len, uint8_t *buf)
{
	VL_IIC_Start();
	VL_IIC_Send_Byte(SlaveAddress);//发写命令
	if(VL_IIC_Wait_Ack()) 
	{
		VL_IIC_Stop();//释放总线
		return 1;//没应答则退出
	}
	VL_IIC_Send_Byte(REG_Address);
	VL_IIC_Wait_Ack();
	while(len--)
	{
		VL_IIC_Send_Byte(*buf++);//发送buff的数据
		VL_IIC_Wait_Ack();	
	}
	VL_IIC_Stop();//释放总线

	return 0;
	
}

//IIC读n字节数据
uint8_t VL_IIC_Read_nByte(uint8_t SlaveAddress, uint8_t REG_Address,uint16_t len,uint8_t *buf)
{
	VL_IIC_Start();
	VL_IIC_Send_Byte(SlaveAddress);//发写命令
	if(VL_IIC_Wait_Ack()) 
	{
		VL_IIC_Stop();//释放总线
		return 1;//没应答则退出
	}
	VL_IIC_Send_Byte(REG_Address);
	VL_IIC_Wait_Ack();

	VL_IIC_Start();
	VL_IIC_Send_Byte(SlaveAddress|0x01);//发读命令
	VL_IIC_Wait_Ack();
	while(len)
	{
		if(len==1)
		{
			*buf = VL_IIC_Read_Byte(0);
		}
		else
		{
			*buf = VL_IIC_Read_Byte(1);
		}
		buf++;
		len--;
	}
	VL_IIC_Stop();//释放总线

	return 0;
	
}

//VL53L0X 写多个数据
//address:地址
//index:偏移地址
//pdata:数据指针
//count:长度 最大65535
uint8_t VL53L0X_write_multi(uint8_t address, uint8_t index,uint8_t *pdata,uint16_t count)
{
	uint8_t status = STATUS_OK;

	if(VL_IIC_Write_nByte(address,index,count,pdata))
	{
	   status  = STATUS_FAIL;

	}

	return status;
}


//VL53L0X 读多个数据
//address:地址
//index:偏移地址
//pdata:数据指针
//count:长度 最大65535
uint8_t VL53L0X_read_multi(uint8_t address,uint8_t index,uint8_t *pdata,uint16_t count)
{
	uint8_t status = STATUS_OK;

	if(VL_IIC_Read_nByte(address,index,count,pdata))
	{
	  status  = STATUS_FAIL;
	}

	return status;
}

//VL53L0X 写1个数据(单字节)
//address:地址
//index:偏移地址
//data:数据(8位)
uint8_t VL53L0X_write_byte(uint8_t address,uint8_t index,uint8_t data)
{
	uint8_t status = STATUS_OK;

	status = VL53L0X_write_multi(address,index,&data,1);

	return status;
}

//VL53L0X 写1个数据(双字节)
//address:地址
//index:偏移地址
//data:数据(16位)
uint8_t VL53L0X_write_word(uint8_t address,uint8_t index,uint16_t data)
{
	uint8_t status = STATUS_OK;
	
	uint8_t buffer[2];
	
	//将16位数据拆分成8位
	buffer[0] = (uint8_t)(data>>8);//高八位
	buffer[1] = (uint8_t)(data&0xff);//低八位
	
	if(index%2==1)
	{  
		//串行通信不能处理对非2字节对齐寄存器的字节
		status = VL53L0X_write_multi(address,index,&buffer[0],1);
		status = VL53L0X_write_multi(address,index,&buffer[0],1);
	}else
	{
		status = VL53L0X_write_multi(address,index,buffer,2);
	}
	
	return status;
}

//VL53L0X 写1个数据(四字节)
//address:地址
//index:偏移地址
//data:数据(32位)
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


//VL53L0X 读1个数据(单字节)
//address:地址
//index:偏移地址
//data:数据(8位)
uint8_t VL53L0X_read_byte(uint8_t address,uint8_t index,uint8_t *pdata)
{
	uint8_t status = STATUS_OK;
	 
	status = VL53L0X_read_multi(address,index,pdata,1);
	
	return status;
	 
}

//VL53L0X 读个数据(双字节)
//address:地址
//index:偏移地址
//data:数据(16位)
uint8_t VL53L0X_read_word(uint8_t address,uint8_t index,uint16_t *pdata)
{
	uint8_t status = STATUS_OK;
	
	uint8_t buffer[2];
	
	status = VL53L0X_read_multi(address,index,buffer,2);
	
	*pdata = ((uint16_t)buffer[0]<<8)+(uint16_t)buffer[1];
	
	return status;
	
}

//VL53L0X 读1个数据(四字节)
//address:地址
//index:偏移地址
//data:数据(32位)
uint8_t VL53L0X_read_dword(uint8_t address,uint8_t index,uint32_t *pdata)
{
	uint8_t status = STATUS_OK;
	
	uint8_t buffer[4];
	
	status = VL53L0X_read_multi(address,index,buffer,4);
	
	*pdata = ((uint32_t)buffer[0]<<24)+((uint32_t)buffer[1]<<16)+((uint32_t)buffer[2]<<8)+((uint32_t)buffer[3]);
	
	return status;
	
}
