/*
 * Copyright (c) 2006-2020, TJ Team
 *
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-03-17     唐文军       the first version
 */
#ifndef TASK_INCLUDES_H_
#define TASK_INCLUDES_H_

#include "main.h"

//任务优先级
#define LAUNCH_TASK_PRIO  4
//任务堆栈大小
#define LAUNCH_STK_SIZE   256
extern __IO OS_TCB LaunchTaskTCB;
//任务堆栈
extern __align(8) __IO CPU_STK LAUNCH_TASK_STK[LAUNCH_STK_SIZE];
//任务函数
void launch_task(void *p_arg);

extern OS_MUTEX task_lock;
extern uint8_t master_launch_flag;

#endif /* TASK_INCLUDES_H_ */
