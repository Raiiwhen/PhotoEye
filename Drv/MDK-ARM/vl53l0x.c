/*
 * Copyright (c) 2006-2020, TJ Team
 *
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-03-17     唐文军       the first version
 */

#include "vl53l0x.h"
#include <stdio.h>

#define DBG_TAG		"vl53l0x"
#define DBG_LEVEL	DBG_LOG

VL53L0X_Dev_t vl53l0x_dev;
VL53L0X_DeviceInfo_t vl53l0x_dev_info;
uint8_t VhvSettings;
uint8_t PhaseCal;
uint32_t refSpadCount;
uint8_t isApertureSpads;
uint8_t AjustOK = 1;						// adjust status flag

/*
 * VL53L0X mode parameter
 * 0：Default ;1: High Accuracy ;2: Long Distance ;3: High Speed
 */ 
mode_data Mode_data[]=
{
    {(FixPoint1616_t)(0.25*65536), 
	 (FixPoint1616_t)(18*65536),
	 33000,
	 14,
	 10},//0：Default
		
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

static void VL_delay_ms(uint32_t ms){
	while(ms--){
		int cnt = 7200;
		while(cnt--){};
	}
}

/* LOG OUT API ERROR Info */
void print_pal_error(VL53L0X_Error Status)
{	
	char buf[VL53L0X_MAX_STRING_LENGTH];
	VL53L0X_GetPalErrorString(Status,buf);
    printf("API Status: %i : %s",Status, buf);
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

//配置VL53L0X设备I2C地址
//dev:设备I2C参数结构体
//newaddr:设备新I2C地址
VL53L0X_Error vl53l0x_Addr_set(VL53L0X_Dev_t *dev,uint8_t newaddr)
{
	uint16_t Id;
	uint8_t FinalAddress;
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	uint8_t sta=0x00;
	
	FinalAddress = newaddr;
	
	if(FinalAddress==dev->I2cDevAddr)//新设备I2C地址与旧地址一致,直接退出
		return VL53L0X_ERROR_NONE;
	//在进行第一个寄存器访问之前设置I2C标准模式(400Khz)
	Status = VL53L0X_WrByte(dev,0x88,0x00);
	if(Status!=VL53L0X_ERROR_NONE) 
	{
		sta=0x01;//设置I2C标准模式出错
		goto set_error;
	}
	//尝试使用默认的0x52地址读取一个寄存器
	Status = VL53L0X_RdWord(dev, VL53L0X_REG_IDENTIFICATION_MODEL_ID, &Id);
	if(Status!=VL53L0X_ERROR_NONE) 
	{
		sta=0x02;//读取寄存器出错
		goto set_error;
	}
	if(Id == 0xEEAA)
	{
		//设置设备新的I2C地址
		Status = VL53L0X_SetDeviceAddress(dev,FinalAddress);
		if(Status!=VL53L0X_ERROR_NONE) 
		{
			sta=0x03;//设置I2C地址出错
			goto set_error;
		}
		//修改参数结构体的I2C地址
		dev->I2cDevAddr = FinalAddress;
		//检查新的I2C地址读写是否正常
		Status = VL53L0X_RdWord(dev, VL53L0X_REG_IDENTIFICATION_MODEL_ID, &Id);
		if(Status!=VL53L0X_ERROR_NONE) 
		{
			sta=0x04;//新I2C地址读写出错
			goto set_error;
		}	
	}
	set_error:
	if(Status!=VL53L0X_ERROR_NONE)
	{
		print_pal_error(Status);//打印错误信息
	}
	if(sta!=0)
	  printf("sta:0x%x\r\n",sta);
	return Status;
}

//vl53l0x复位函数
//dev:设备I2C参数结构体
void vl53l0x_reset(VL53L0X_Dev_t *dev)
{
	uint8_t addr;
	addr = dev->I2cDevAddr;//保存设备原I2C地址
  VL_XSHUT = 0;
	VL_delay_ms(30);
	VL_XSHUT = 1;
	VL_delay_ms(30);	
	dev->I2cDevAddr=0x52;
	vl53l0x_Addr_set(dev,addr);//设置VL53L0X传感器原来上电前原I2C地址
	VL53L0X_DataInit(dev);	
}

//初始化vl53l0x
//dev:设备I2C参数结构体
VL53L0X_Error vl53l0x_init(VL53L0X_Dev_t *dev)
{
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	VL53L0X_Dev_t *pMyDevice = dev;
	
	pMyDevice->I2cDevAddr = VL53L0X_Addr;
	pMyDevice->comms_type = 1;
	pMyDevice->comms_speed_khz = 400;
	
	
	VL_XSHUT = 0;
	VL_delay_ms(30);
	VL_XSHUT = 1;
	VL_delay_ms(30);
	
  vl53l0x_Addr_set(pMyDevice,0x54);//设置VL53L0X传感器I2C地址
    if(Status!=VL53L0X_ERROR_NONE) goto error;
	Status = VL53L0X_DataInit(pMyDevice);//设备初始化
		if(Status!=VL53L0X_ERROR_NONE) goto error;
	VL_delay_ms(2);
	Status = VL53L0X_GetDeviceInfo(pMyDevice,&vl53l0x_dev_info);//获取设备ID信息
    if(Status!=VL53L0X_ERROR_NONE) goto error;
	
	error:
	if(Status!=VL53L0X_ERROR_NONE)
	{
		print_pal_error(Status);//打印错误信息
		return Status;
	}
  	
	return Status;
}


/* get vl53l0x ID info */
void vl53l0x_info(void)
{
	printf("-------vl53l0x传感器设备信息-------\r\n");
	printf("  Name:%s\r\n",vl53l0x_dev_info.Name);
	printf("  Addr:0x%x\r\n",vl53l0x_dev.I2cDevAddr);
	printf("  ProductId:%s\r\n",vl53l0x_dev_info.ProductId);
	printf("  RevisionMajor:0x%x\r\n",vl53l0x_dev_info.ProductRevisionMajor);
	printf("  RevisionMinor:0x%x\r\n",vl53l0x_dev_info.ProductRevisionMinor);
	printf("-----------------------------------\r\n");
}


//VL53L0X 测量模式配置
//dev:设备I2C参数结构体
//mode: 0:默认;1:高精度;2:长距离
VL53L0X_Error vl53l0x_set_mode(VL53L0X_Dev_t *dev,uint8_t mode)
{
	
	 VL53L0X_Error status = VL53L0X_ERROR_NONE;

	
	 vl53l0x_reset(dev);//复位vl53l0x(频繁切换工作模式容易导致采集距离数据不准，需加上这一代码)
	 status = VL53L0X_StaticInit(dev);

     if(AjustOK != 0)//已校准好了,写入校准值
     {
	    status= VL53L0X_SetReferenceSpads(dev, VhvSettings, PhaseCal);//设定Spads校准值
        if(status!=VL53L0X_ERROR_NONE) goto error;	
        VL_delay_ms(2);		 
	    status= VL53L0X_SetRefCalibration(dev,refSpadCount, isApertureSpads);//设定Ref校准值
		if(status!=VL53L0X_ERROR_NONE) goto error;
		 VL_delay_ms(2);
//	    status=VL53L0X_SetOffsetCalibrationDataMicroMeter(dev,Vl53l0x_data.OffsetMicroMeter);//设定偏移校准值
//		if(status!=VL53L0X_ERROR_NONE) goto error; 
//		 VL_delay_ms(2);
//		status=VL53L0X_SetXTalkCompensationRateMegaCps(dev,Vl53l0x_data.XTalkCompensationRateMegaCps);//设定串扰校准值
//		if(status!=VL53L0X_ERROR_NONE) goto error;
//         VL_delay_ms(2);		 
		 
     }
	 else
	 {
		status = VL53L0X_PerformRefCalibration(dev, &VhvSettings, &PhaseCal);//Ref参考校准
		if(status!=VL53L0X_ERROR_NONE) goto error;
		VL_delay_ms(2);
		status = VL53L0X_PerformRefSpadManagement(dev, &refSpadCount, &isApertureSpads);//执行参考SPAD管理
		if(status!=VL53L0X_ERROR_NONE) goto error;
        VL_delay_ms(2);		
		AjustOK = 1;
	 }
	 status = VL53L0X_SetDeviceMode(dev,VL53L0X_DEVICEMODE_SINGLE_RANGING);//使能单次测量模式
	 if(status!=VL53L0X_ERROR_NONE) goto error;
	 VL_delay_ms(2);
	 status = VL53L0X_SetLimitCheckEnable(dev,VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE,1);//使能SIGMA范围检查
	 if(status!=VL53L0X_ERROR_NONE) goto error;
	 VL_delay_ms(2);
	 status = VL53L0X_SetLimitCheckEnable(dev,VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE,1);//使能信号速率范围检查
	 if(status!=VL53L0X_ERROR_NONE) goto error;
	 VL_delay_ms(2);
	 status = VL53L0X_SetLimitCheckValue(dev,VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE,Mode_data[mode].sigmaLimit);//设定SIGMA范围
	 if(status!=VL53L0X_ERROR_NONE) goto error;
	 VL_delay_ms(2);
	 status = VL53L0X_SetLimitCheckValue(dev,VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE,Mode_data[mode].signalLimit);//设定信号速率范围范围
	 if(status!=VL53L0X_ERROR_NONE) goto error;
	 VL_delay_ms(2);
	 status = VL53L0X_SetMeasurementTimingBudgetMicroSeconds(dev,Mode_data[mode].timingBudget);//设定完整测距最长时间
	 if(status!=VL53L0X_ERROR_NONE) goto error;
	 VL_delay_ms(2);
	 status = VL53L0X_SetVcselPulsePeriod(dev, VL53L0X_VCSEL_PERIOD_PRE_RANGE, Mode_data[mode].preRangeVcselPeriod);//设定VCSEL脉冲周期
	 if(status!=VL53L0X_ERROR_NONE) goto error;
	 VL_delay_ms(2);
	 status = VL53L0X_SetVcselPulsePeriod(dev, VL53L0X_VCSEL_PERIOD_FINAL_RANGE, Mode_data[mode].finalRangeVcselPeriod);//设定VCSEL脉冲周期范围
	 
	 error://错误信息
	 if(status!=VL53L0X_ERROR_NONE)
	 {
		print_pal_error(status);
		return status;
	 }
	 return status;
	
}

VL53L0X_RangingMeasurementData_t vl53l0x_data;//测距测量结构体
uint16_t Distance_data=0;//保存测距数据

//获取一次测量距离数据
//mode模式配置 0:默认;1:高精度;2:长距离;3:高速
void One_measurement(VL53L0X_Dev_t *dev)
{
	VL53L0X_Error status = VL53L0X_ERROR_NONE;
	status = VL53L0X_PerformSingleRangingMeasurement(dev,&vl53l0x_data);
	printf("STA: %02X | ",status);
	printf("DAT: %d mm\r\n",vl53l0x_data.RangeMilliMeter);		
}

