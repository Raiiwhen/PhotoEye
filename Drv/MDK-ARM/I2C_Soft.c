#include "I2C_Soft.h"
#include <stdio.h>
/*Soft IIC1 Time Simualtor*/

#define IIC1_SCL VL_SCL
#define IIC1_WR VL_SDA_WR
#define IIC1_RD VL_SDA_RD
#define SDA1_IN() {GPIOB->CRL &= 0xff0fffff; GPIOB->CRL |= 0x00400000; }
#define SDA1_OUT(){GPIOB->CRL &= 0xff0fffff; GPIOB->CRL |= 0x00100000; }

static void delay_us(uint32_t us){
	while(us--){
		int cnt = 72;
		while(cnt--){};
	}
}

void IIC1_Start(void){
	SDA1_OUT();
	
	IIC1_WR=1;	  	  
	IIC1_SCL=1;
	delay_us(4);
 	IIC1_WR=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC1_SCL=0;
}	  
void IIC1_Stop(void){
	SDA1_OUT();
	
	IIC1_SCL=0;
	IIC1_WR=0;
 	delay_us(4);
	IIC1_SCL=1; 
	IIC1_WR=1;
	delay_us(4);			
}
uint8_t IIC1_Wait_Ack(void){
	uint8_t ucErrTime=0;
	SDA1_IN();
	
	IIC1_WR=1;delay_us(1);	   
	IIC1_SCL=1;delay_us(1);	 
	while(IIC1_RD)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC1_Stop();
			printf("ACK missed!\r\n");
			return 1;
		}
	}
	//printf("Copy.\r\n");
	IIC1_SCL=0;
	return 0;  
}
void IIC1_Ack(void){
	SDA1_OUT();
	
	IIC1_SCL=0;
	IIC1_WR=0;
	delay_us(2);
	IIC1_SCL=1;
	delay_us(2);
	IIC1_SCL=0;
}
void IIC1_NAck(void){
	IIC1_SCL=0;
	SDA1_OUT();
	IIC1_WR=1;
	delay_us(2);
	IIC1_SCL=1;
	delay_us(2);
	IIC1_SCL=0;
}
void IIC1_Send_Byte(uint8_t txd){                        
    uint8_t cnt;   
		SDA1_OUT(); 	 
	
    IIC1_SCL=0;
    for(cnt=0;cnt<8;cnt++){              
			IIC1_WR=(txd&0x80)>>7;
			txd<<=1; 	  
			delay_us(2);
			IIC1_SCL=1;
			delay_us(2); 
			IIC1_SCL=0;	
			delay_us(2);
    }	 
}
uint8_t IIC1_Read_Byte(unsigned char ack){
	unsigned char cnt,temp=0;
	SDA1_IN();//SDA设置为输入
	for(cnt=0;cnt<8;cnt++ )
	{
		IIC1_SCL=0; 
		delay_us(2);
		IIC1_SCL=1;
		temp<<=1;
		if(IIC1_RD)temp++;   
		delay_us(1); 
	}					 
	if (!ack)
		IIC1_NAck();//发送nACK
	else
		IIC1_Ack(); //发送ACK   
	return temp;
}

uint8_t IIC1_Read_One_Byte(uint8_t addr, uint8_t reg){
	uint8_t  data;
	IIC1_Start();
	
	IIC1_Send_Byte(0x40);
	IIC1_Wait_Ack();
	IIC1_Send_Byte(reg);
	IIC1_Wait_Ack();
	IIC1_Start();
	IIC1_Send_Byte(0xc0);
	IIC1_Wait_Ack();
	data = IIC1_Read_Byte(0);
	IIC1_NAck();

	IIC1_Stop();
	return data;
}

/*IIC1 Test Sequence*/
void IIC1_Test(void){
/*IIC IO Bus test sequence*/
//	uint8_t cnt,reg;
/*Output test*/
//	SDA1_OUT();
//	IIC1_WR = 1;
//	IIC1_SCL = 1;
//	while(1){};
/*Input Test*/
//	SDA1_IN();
//	while(1){
//		IIC1_RD ? (LED_R = 1) : (LED_R = 0);
//	}
/*SCL SDA out put test*/
//	while(1){
//		SDA1_OUT();
//		IIC1_SCL = 1;
//		delay_ms(5);
//		IIC1_WR = 0;
//		delay_ms(5);
//		IIC1_SCL = 0;
//		delay_ms(5);
//		IIC1_WR = 1;
//		delay_ms(5);
//		LED_G = !LED_G;
//	}
/*Byte Send Tesst*/
//	while(1)	IIC1_Send_Byte(0xd6);//1101 0110
/*List all address that has ACK*/
	IIC1_Slave_List();
	while(1){};
}

void IIC1_Slave_List(void){
	uint8_t i=0,j=0,res = 0;
	uint8_t ucErrTime = 0;
	uint8_t CNT = 0;
	printf("Console>IIC1: List all devices.\r\n");

	for(i=0;i<255;i++)
	{
		ucErrTime = 0;
		IIC1_Start();
    IIC1_Send_Byte((i<<1)|0);
		
		SDA1_IN();      //wait for ACK
		IIC1_WR=1;delay_us(1);	   
		IIC1_SCL=1;delay_us(1);	 
		while(IIC1_RD)
		{
			ucErrTime++;
			if(ucErrTime>250){
				IIC1_Stop();
				break;
			}
		}
		if(ucErrTime<250){
			printf("Console>IIC1:  |DeviceCNT.%3d, IIC_ADDR = %#x\r\n",CNT, i);
			for(;j<255;j++){
				res = IIC1_Read_One_Byte(CNT,j);
				printf("Console>IIC1:  |reg.%2X, %2X\r\n",j, res);
			}
			CNT++;
		}
		IIC1_SCL=0;//时钟输出0 	   
		IIC1_Stop();
	}
	printf("Console>IIC1: Scan over.\r\n");
}

