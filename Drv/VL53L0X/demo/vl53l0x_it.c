#include "vl53l0x_it.h"

#define Thresh_Low  60
#define Thresh_High 150

//中断模式参数结构体
typedef struct 
{
     const int VL53L0X_Mode;//模式
	 uint32_t ThreshLow;    //下限值
	 uint32_t ThreshHigh;   //上限值
}AlrmMode_t; 

AlrmMode_t AlarmModes ={
	
     VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_OUT,// value < thresh_low OR value > thresh_high
	 Thresh_Low<<16,
	 Thresh_High<<16
};

//中断配置初始化
static void exti_init(void)
{

}

//警报标志位 alarm_flag 
//1:有警报
//0：无
uint8_t alarm_flag=0;

extern uint8_t AjustOK;
extern mode_data Mode_data[];

//vl53l0x中断测量模式测试
//dev:设备I2C参数结构体
//mode: 0:默认;1:高精度;2:长距离;3:高速
void vl53l0x_interrupt_start(VL53L0X_Dev_t *dev,uint8_t mode)
{
		
}

//vl53l0x中断测量模式测试
//dev:设备I2C参数结构体
void vl53l0x_interrupt_test(VL53L0X_Dev_t *dev)
{
	
}
