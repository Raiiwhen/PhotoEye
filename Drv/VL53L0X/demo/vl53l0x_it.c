#include "vl53l0x_it.h"

#define Thresh_Low  60
#define Thresh_High 150

//�ж�ģʽ�����ṹ��
typedef struct 
{
     const int VL53L0X_Mode;//ģʽ
	 uint32_t ThreshLow;    //����ֵ
	 uint32_t ThreshHigh;   //����ֵ
}AlrmMode_t; 

AlrmMode_t AlarmModes ={
	
     VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_OUT,// value < thresh_low OR value > thresh_high
	 Thresh_Low<<16,
	 Thresh_High<<16
};

//�ж����ó�ʼ��
static void exti_init(void)
{

}

//������־λ alarm_flag 
//1:�о���
//0����
uint8_t alarm_flag=0;

extern uint8_t AjustOK;
extern mode_data Mode_data[];

//vl53l0x�жϲ���ģʽ����
//dev:�豸I2C�����ṹ��
//mode: 0:Ĭ��;1:�߾���;2:������;3:����
void vl53l0x_interrupt_start(VL53L0X_Dev_t *dev,uint8_t mode)
{
		
}

//vl53l0x�жϲ���ģʽ����
//dev:�豸I2C�����ṹ��
void vl53l0x_interrupt_test(VL53L0X_Dev_t *dev)
{
	
}
