#include "main.h"
/*
Import following hard resources:
IIC_SCL
IIC_SDA(read)
IIC_SDA(write)
*/

/*IIC1*/
void IIC1_Start(void);
void IIC1_Stop(void);
void IIC1_Send_Byte(uint8_t txd);
uint8_t IIC1_Read_Byte(unsigned char ack);
uint8_t IIC1_Wait_Ack(void);
void IIC1_Ack(void);
void IIC1_NAck(void);
void IIC1_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t IIC1_Read_One_Byte(uint8_t daddr,uint8_t addr);	  

void IIC1_Test(void);
void IIC1_Slave_List(void);
