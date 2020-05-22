#ifndef OS_CFG_APP_H
#define OS_CFG_APP_H

#define OS_CFG_MSG_POOL_SIZE 					100u 			//��Ϣ����
#define OS_CFG_ISR_STK_SIZE 					128u 			//�ж������ջ��С
//�����ջ��ȣ����綨��Ϊ10ʱ����ʾ�������ջʣ��10%ʱ�򣬾�˵����ջΪ��
#define OS_CFG_TASK_STK_LIMIT_PCT_EMPTY 		10u

//*********************��������*************************
#define OS_CFG_IDLE_TASK_STK_SIZE 				128u 			//���������ջ��С

//********************�жϷ����������*****************
#define OS_CFG_INT_Q_SIZE 						10u 			//�ж϶��д�С
#define OS_CFG_INT_Q_TASK_STK_SIZE 				128u 			//�жϷ�����������ջ��С

//********************ͳ������**************************
//ͳ����������ȼ�Ϊ�����ڶ��������һ�����ȼ��ǿ�������ġ�
#define OS_CFG_STAT_TASK_PRIO (OS_CFG_PRIO_MAX-2u)
// OS_CFG_STAT_TASK_RATE_HZ ����ͳ���������CPU��ʹ���ʣ�ͳ������ͨ��ͳ����
//(1/ OS_CFG_STAT_TASK_RATE_HZ)���ʱ����OSStatTaskCtr�ܹ��ﵽ�����ֵ���õ�
//CPU��ʹ���ʣ�OS_CFG_STAT_TASK_RATE_HZ ֵӦ���� 1~10Hz
#define OS_CFG_STAT_TASK_RATE_HZ 				10u
#define OS_CFG_STAT_TASK_STK_SIZE 				128u 			//ͳ�������ջ��С

//******************ʱ�ӽ�������***********************
#define OS_CFG_TICK_RATE_HZ 					1000u 			//ϵͳʱ�ӽ���Ƶ��
//ʱ�ӽ�������һ������һ����Խϸߵ����ȼ�
#define OS_CFG_TICK_TASK_PRIO 					1u
#define OS_CFG_TICK_TASK_STK_SIZE 				128u 			//ʱ�ӽ��������ջ��С
#define OS_CFG_TICK_WHEEL_SIZE 					17u 			//ʱ�ӽ����б��С

//************************��ʱ����************************
#define OS_CFG_TMR_TASK_PRIO 					2u 				//��ʱ���������ȼ�
#define OS_CFG_TMR_TASK_RATE_HZ 				100u 			//��ʱ��Ƶ�ʣ�һ��Ϊ100Hz
#define OS_CFG_TMR_TASK_STK_SIZE 				128u 			//��ʱ�������ջ��С
#define OS_CFG_TMR_WHEEL_SIZE 					17u 			//��ʱ���б�����

#endif







