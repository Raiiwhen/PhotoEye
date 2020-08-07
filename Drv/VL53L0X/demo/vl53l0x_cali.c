#include "vl53l0x_cali.h"

_vl53l0x_adjust Vl53l0x_adjust; //校准数据24c02写缓存区(用于在校准模式校准数据写入24c02)
_vl53l0x_adjust Vl53l0x_data;   //校准数据24c02读缓存区（用于系统初始化时向24C02读取数据）

#define adjust_num 5//校准错误次数

//VL53L0X校准函数
//dev:设备I2C参数结构体
VL53L0X_Error vl53l0x_adjust(VL53L0X_Dev_t *dev)
{	
	return 0;
}

//vl53l0x校准测试
//dev:设备I2C参数结构体
void vl53l0x_calibration_test(VL53L0X_Dev_t *dev)
{  
		
}
