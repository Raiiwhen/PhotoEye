/*
 * Copyright (c) 2006-2020, TJ Team
 *
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-03-17     ���ľ�       the first version
 */
#include "task_includes.h"

#include "vl53l0x.h"

#define DBG_TAG		"task_launch"
#define DBG_LEVEL	DBG_LOG
#include "debug.h"


__IO OS_TCB LaunchTaskTCB;
//�����ջ
__align(8) __IO CPU_STK LAUNCH_TASK_STK[LAUNCH_STK_SIZE];
//������
void launch_task(void *p_arg);

OS_MUTEX task_lock;
uint8_t master_launch_flag;

//��ʼ����
void launch_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	uint16_t time  = 0;
	while(1){
		OS_CRITICAL_ENTER();	//�����ٽ���
		LOG_I("timestamp : %d ms",time);
		One_measurement();
		time += 30;
//		OSMutexPend(&task_lock,0,OS_OPT_PEND_BLOCKING,NULL,&err);
//		if(!master_launch_flag)
//			continue;
//		LOG_I("launch_task : test!");
//		OSMutexPost(&task_lock,OS_OPT_POST_NONE,&err);
//		master_launch_flag = 0;

		OS_CRITICAL_EXIT();		//�˳��ٽ���
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); 	//��ʱ1ms
	}
}


