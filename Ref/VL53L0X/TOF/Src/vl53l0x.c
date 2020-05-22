/*
 * Copyright (c) 2006-2020, TJ Team
 *
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-03-17     ���ľ�       the first version
 */

#include "vl53l0x.h"

#define DBG_TAG		"vl53l0x"
#define DBG_LEVEL	DBG_LOG
#include "debug.h"

VL53L0X_Dev_t vl53l0x_dev;
VL53L0X_DeviceInfo_t vl53l0x_dev_info;
uint8_t VhvSettings;
uint8_t PhaseCal;
uint32_t refSpadCount;
uint8_t isApertureSpads;
uint8_t AjustOK = 0;						// adjust status flag

/*
 * VL53L0X mode parameter
 * 0��Default ;1: High Accuracy ;2: Long Distance ;3: High Speed
 */ 
mode_data Mode_data[]=
{
    {(FixPoint1616_t)(0.25*65536), 
	 (FixPoint1616_t)(18*65536),
	 33000,
	 14,
	 10},//0��Default
		
	{(FixPoint1616_t)(0.25*65536) ,
	 (FixPoint1616_t)(18*65536),
	 200000, 
	 14,
	 10},//1: High Accuracy
		
    {(FixPoint1616_t)(0.1*65536) ,
	 (FixPoint1616_t)(60*65536),
	 33000,
	 18,
	 14},//2: Long Distance
	
    {(FixPoint1616_t)(0.25*65536) ,
	 (FixPoint1616_t)(32*65536),
	 20000,
	 14,
	 10},//3: High Speed
};

/* LOG OUT API ERROR Info */
void print_pal_error(VL53L0X_Error Status)
{	
	char buf[VL53L0X_MAX_STRING_LENGTH];
	VL53L0X_GetPalErrorString(Status,buf);
    LOG_E("API Status: %i : %s",Status, buf);
}

/* get mode string */
void mode_string(uint8_t mode,char *buf)
{
	switch(mode)
	{
		case Default_Mode: strcpy(buf,"Default");        break;
		case HIGH_ACCURACY: strcpy(buf,"High Accuracy"); break;
		case LONG_RANGE: strcpy(buf,"Long Range");       break;
		case HIGH_SPEED: strcpy(buf,"High Speed");       break;
	}

}

//����VL53L0X�豸I2C��ַ
//dev:�豸I2C�����ṹ��
//newaddr:�豸��I2C��ַ
VL53L0X_Error vl53l0x_Addr_set(VL53L0X_Dev_t *dev,uint8_t newaddr)
{
	uint16_t Id;
	uint8_t FinalAddress;
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	uint8_t sta=0x00;
	
	FinalAddress = newaddr;
	
	if(FinalAddress==dev->I2cDevAddr)//���豸I2C��ַ��ɵ�ַһ��,ֱ���˳�
		return VL53L0X_ERROR_NONE;
	//�ڽ��е�һ���Ĵ�������֮ǰ����I2C��׼ģʽ(400Khz)
	Status = VL53L0X_WrByte(dev,0x88,0x00);
	if(Status!=VL53L0X_ERROR_NONE) 
	{
		sta=0x01;//����I2C��׼ģʽ����
		goto set_error;
	}
	//����ʹ��Ĭ�ϵ�0x52��ַ��ȡһ���Ĵ���
	Status = VL53L0X_RdWord(dev, VL53L0X_REG_IDENTIFICATION_MODEL_ID, &Id);
	if(Status!=VL53L0X_ERROR_NONE) 
	{
		sta=0x02;//��ȡ�Ĵ�������
		goto set_error;
	}
	if(Id == 0xEEAA)
	{
		//�����豸�µ�I2C��ַ
		Status = VL53L0X_SetDeviceAddress(dev,FinalAddress);
		if(Status!=VL53L0X_ERROR_NONE) 
		{
			sta=0x03;//����I2C��ַ����
			goto set_error;
		}
		//�޸Ĳ����ṹ���I2C��ַ
		dev->I2cDevAddr = FinalAddress;
		//����µ�I2C��ַ��д�Ƿ�����
		Status = VL53L0X_RdWord(dev, VL53L0X_REG_IDENTIFICATION_MODEL_ID, &Id);
		if(Status!=VL53L0X_ERROR_NONE) 
		{
			sta=0x04;//��I2C��ַ��д����
			goto set_error;
		}	
	}
	set_error:
	if(Status!=VL53L0X_ERROR_NONE)
	{
		print_pal_error(Status);//��ӡ������Ϣ
	}
	if(sta!=0)
	  printf("sta:0x%x\r\n",sta);
	return Status;
}

//vl53l0x��λ����
//dev:�豸I2C�����ṹ��
void vl53l0x_reset(VL53L0X_Dev_t *dev)
{
	uint8_t addr;
	addr = dev->I2cDevAddr;//�����豸ԭI2C��ַ
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);//ʧ��VL53L0X
	hw_ms_delay(30);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);//ʹ��VL53L0X,�ô��������ڹ���(I2C��ַ��ָ�Ĭ��0X52)
	hw_ms_delay(30);	
	dev->I2cDevAddr=0x52;
	vl53l0x_Addr_set(dev,addr);//����VL53L0X������ԭ���ϵ�ǰԭI2C��ַ
	VL53L0X_DataInit(dev);	
}

//��ʼ��vl53l0x
//dev:�豸I2C�����ṹ��
VL53L0X_Error vl53l0x_init(VL53L0X_Dev_t *dev)
{
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	VL53L0X_Dev_t *pMyDevice = dev;
	GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_GPIOB_CLK_ENABLE();           //����GPIOAʱ��
	GPIO_Initure.Pin=GPIO_PIN_0;            //PB0
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//����
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	
	pMyDevice->I2cDevAddr = VL53L0X_Addr;//I2C��ַ(�ϵ�Ĭ��0x52)
	pMyDevice->comms_type = 1;           //I2Cͨ��ģʽ
	pMyDevice->comms_speed_khz = 400;    //I2Cͨ������
	
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);//ʧ��VL53L0X
	hw_ms_delay(30);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);//ʹ��VL53L0X,�ô��������ڹ���
	hw_ms_delay(30);
	
    vl53l0x_Addr_set(pMyDevice,0x54);//����VL53L0X������I2C��ַ
    if(Status!=VL53L0X_ERROR_NONE) goto error;
	Status = VL53L0X_DataInit(pMyDevice);//�豸��ʼ��
	if(Status!=VL53L0X_ERROR_NONE) goto error;
	hw_ms_delay(2);
	Status = VL53L0X_GetDeviceInfo(pMyDevice,&vl53l0x_dev_info);//��ȡ�豸ID��Ϣ
    if(Status!=VL53L0X_ERROR_NONE) goto error;
	
	error:
	if(Status!=VL53L0X_ERROR_NONE)
	{
		print_pal_error(Status);//��ӡ������Ϣ
		return Status;
	}
  	
	return Status;
}


/* get vl53l0x ID info */
void vl53l0x_info(void)
{
	LOG_I("-------vl53l0x�������豸��Ϣ-------");
	LOG_I("  Name:%s",vl53l0x_dev_info.Name);
	LOG_I("  Addr:0x%x",vl53l0x_dev.I2cDevAddr);
	LOG_I("  ProductId:%s",vl53l0x_dev_info.ProductId);
	LOG_I("  RevisionMajor:0x%x",vl53l0x_dev_info.ProductRevisionMajor);
	LOG_I("  RevisionMinor:0x%x",vl53l0x_dev_info.ProductRevisionMinor);
	LOG_I("-----------------------------------");
}


//VL53L0X ����ģʽ����
//dev:�豸I2C�����ṹ��
//mode: 0:Ĭ��;1:�߾���;2:������
VL53L0X_Error vl53l0x_set_mode(VL53L0X_Dev_t *dev,uint8_t mode)
{
	
	 VL53L0X_Error status = VL53L0X_ERROR_NONE;

	
	 vl53l0x_reset(dev);//��λvl53l0x(Ƶ���л�����ģʽ���׵��²ɼ��������ݲ�׼���������һ����)
	 status = VL53L0X_StaticInit(dev);

     if(AjustOK != 0)//��У׼����,д��У׼ֵ
     {
	    status= VL53L0X_SetReferenceSpads(dev, VhvSettings, PhaseCal);//�趨SpadsУ׼ֵ
        if(status!=VL53L0X_ERROR_NONE) goto error;	
        hw_ms_delay(2);		 
	    status= VL53L0X_SetRefCalibration(dev,refSpadCount, isApertureSpads);//�趨RefУ׼ֵ
		if(status!=VL53L0X_ERROR_NONE) goto error;
		 hw_ms_delay(2);
//	    status=VL53L0X_SetOffsetCalibrationDataMicroMeter(dev,Vl53l0x_data.OffsetMicroMeter);//�趨ƫ��У׼ֵ
//		if(status!=VL53L0X_ERROR_NONE) goto error; 
//		 hw_ms_delay(2);
//		status=VL53L0X_SetXTalkCompensationRateMegaCps(dev,Vl53l0x_data.XTalkCompensationRateMegaCps);//�趨����У׼ֵ
//		if(status!=VL53L0X_ERROR_NONE) goto error;
//         hw_ms_delay(2);		 
		 
     }
	 else
	 {
		status = VL53L0X_PerformRefCalibration(dev, &VhvSettings, &PhaseCal);//Ref�ο�У׼
		if(status!=VL53L0X_ERROR_NONE) goto error;
		hw_ms_delay(2);
		status = VL53L0X_PerformRefSpadManagement(dev, &refSpadCount, &isApertureSpads);//ִ�вο�SPAD����
		if(status!=VL53L0X_ERROR_NONE) goto error;
        hw_ms_delay(2);		
		AjustOK = 1;
	 }
	 status = VL53L0X_SetDeviceMode(dev,VL53L0X_DEVICEMODE_SINGLE_RANGING);//ʹ�ܵ��β���ģʽ
	 if(status!=VL53L0X_ERROR_NONE) goto error;
	 hw_ms_delay(2);
	 status = VL53L0X_SetLimitCheckEnable(dev,VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE,1);//ʹ��SIGMA��Χ���
	 if(status!=VL53L0X_ERROR_NONE) goto error;
	 hw_ms_delay(2);
	 status = VL53L0X_SetLimitCheckEnable(dev,VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE,1);//ʹ���ź����ʷ�Χ���
	 if(status!=VL53L0X_ERROR_NONE) goto error;
	 hw_ms_delay(2);
	 status = VL53L0X_SetLimitCheckValue(dev,VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE,Mode_data[mode].sigmaLimit);//�趨SIGMA��Χ
	 if(status!=VL53L0X_ERROR_NONE) goto error;
	 hw_ms_delay(2);
	 status = VL53L0X_SetLimitCheckValue(dev,VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE,Mode_data[mode].signalLimit);//�趨�ź����ʷ�Χ��Χ
	 if(status!=VL53L0X_ERROR_NONE) goto error;
	 hw_ms_delay(2);
	 status = VL53L0X_SetMeasurementTimingBudgetMicroSeconds(dev,Mode_data[mode].timingBudget);//�趨��������ʱ��
	 if(status!=VL53L0X_ERROR_NONE) goto error;
	 hw_ms_delay(2);
	 status = VL53L0X_SetVcselPulsePeriod(dev, VL53L0X_VCSEL_PERIOD_PRE_RANGE, Mode_data[mode].preRangeVcselPeriod);//�趨VCSEL��������
	 if(status!=VL53L0X_ERROR_NONE) goto error;
	 hw_ms_delay(2);
	 status = VL53L0X_SetVcselPulsePeriod(dev, VL53L0X_VCSEL_PERIOD_FINAL_RANGE, Mode_data[mode].finalRangeVcselPeriod);//�趨VCSEL�������ڷ�Χ
	 
	 error://������Ϣ
	 if(status!=VL53L0X_ERROR_NONE)
	 {
		print_pal_error(status);
		return status;
	 }
	 return status;
	
}

VL53L0X_RangingMeasurementData_t vl53l0x_data;//�������ṹ��
uint16_t Distance_data=0;//����������

//��ȡһ�β�����������
//modeģʽ���� 0:Ĭ��;1:�߾���;2:������;3:����
void One_measurement(void)
{
	VL53L0X_Error status = VL53L0X_ERROR_NONE;
	status = VL53L0X_PerformSingleRangingMeasurement(&vl53l0x_dev,&vl53l0x_data);
	LOG_I("VL53L0X_PerformSingleRangingMeasurement status:%d",status);
	LOG_I("distance : %4d mm.",vl53l0x_data.RangeMilliMeter);		
}

