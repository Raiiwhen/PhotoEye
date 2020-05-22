/*
 * Copyright (c) 2006-2020, TJ Team
 *
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-03-17     ���ľ�       the first version
 */
#ifndef TASK_INCLUDES_H_
#define TASK_INCLUDES_H_

#include "main.h"

//�������ȼ�
#define LAUNCH_TASK_PRIO  4
//�����ջ��С
#define LAUNCH_STK_SIZE   256
extern __IO OS_TCB LaunchTaskTCB;
//�����ջ
extern __align(8) __IO CPU_STK LAUNCH_TASK_STK[LAUNCH_STK_SIZE];
//������
void launch_task(void *p_arg);

extern OS_MUTEX task_lock;
extern uint8_t master_launch_flag;

#endif /* TASK_INCLUDES_H_ */
