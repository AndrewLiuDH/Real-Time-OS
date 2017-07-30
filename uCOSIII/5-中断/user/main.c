#include "sys.h"
#include "led.h"
#include "exit.h"
#include "delay.h"
#include "usart.h"
#include "includes.h"

#define START_TASK_PRIO 3				//任务优先级
#define START_STK_SIZE 512			//任务堆栈大小
OS_TCB StartTaskTCB;						//任务控制块
CPU_STK START_TASK_STK[START_STK_SIZE];//任务堆栈
void start_task(void *p_arg);		//任务函数

#define TASKB_TASK_PRIO 5				//任务优先级
#define TASKB_STK_SIZE 128				//任务堆栈大小
OS_TCB TASKBTaskTCB;							//任务控制块
CPU_STK TASKB_TASK_STK[TASKB_STK_SIZE];//任务堆栈
void TASKB_task(void *p_arg);		//任务函数

#define TASKC_TASK_PRIO 5				//任务优先级
#define TASKC_STK_SIZE 128				//任务堆栈大小
OS_TCB TASKCTaskTCB;							//任务控制块
CPU_STK TASKC_TASK_STK[TASKC_STK_SIZE];//任务堆栈
void TASKC_task(void *p_arg);		//任务函数


int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();

	delay_init(168);
	USART1_Init();
	LED_Init();
	EXTI_init();

	OSInit(&err);		//初始化UCOSIII
	
	OSStart(&err);  //开启UCOSIII
	
	while(1);
}