#include "uart.h"
#include "stdio.h"
#include "main.h"
#include "string.h"

SBUS_Pack RX;
uint8_t RX_CNT;
uint8_t RX_FLAG;
extern UART_HandleTypeDef huart3;

/* basic support function*/
#define CONSOLE_UART USART3
#define RXER_UART USART2

#pragma import(__use_no_semihosting)             
FILE __stdout;       
struct __FILE 							{	int handle; 			}; 
void _sys_exit(int x)			 	{ x = x; 						} 
int fputc(int ch, FILE *f){
	while((CONSOLE_UART->SR&0X40)==0);
	CONSOLE_UART->DR = (uint8_t) ch;      
	return ch;
}
/* end support*/

uint16_t get_sbus(uint8_t ch){
	if(ch>16)return 1000;
	if(ch<1 )return 1000;
	return RX.ch[ch-1];
}

float set_tmp(float tmp){
	float temp = tmp;
	
	
	return temp;
}

uint32_t console_exe(void){
	LED_B = !LED_B;
	
	
	return 1;
}

