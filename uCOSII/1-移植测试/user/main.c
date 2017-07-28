#include "sys.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "includes.h"

/////////////////////////UCOSII任务设置///////////////////////////////////
//START 任务
//设置任务优先级
#define START_TASK_PRIO      			10 //开始任务的优先级设置为最低
#define START_STK_SIZE  				64//设置任务堆栈大小
OS_STK START_TASK_STK[START_STK_SIZE];//任务堆栈	
void start_task(void *pdata);	//任务函数

 			   
//LED0任务
#define LED0_TASK_PRIO       			7 //设置任务优先级
#define LED0_STK_SIZE  		    		64//设置任务堆栈大小
OS_STK LED0_TASK_STK[LED0_STK_SIZE];//任务堆栈	
void led0_task(void *pdata);//任务函数

//LED1任务
#define LED1_TASK_PRIO       			6 //设置任务优先级
#define LED1_STK_SIZE  					64//设置任务堆栈大小
OS_STK LED1_TASK_STK[LED1_STK_SIZE];//任务堆栈
void led1_task(void *pdata);//任务函数

#define FLOAT_TASK_PRIO 8				//任务优先级
#define FLOAT_STK_SIZE 128			//任务堆栈大小
OS_TCB FloatTaskTCB;						//任务控制块
OS_STK FLOAT_TASK_STK[FLOAT_STK_SIZE];//任务堆栈
void float_task(void *p_arg);		//任务函数

int main(void)
{
	delay_init(168);
	USART1_Init();
	LED_Init();
	
	OSInit();   
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//创建起始任务
	OSStart();	

	while(1);
}

 //开始任务
void start_task(void *pdata)
{
  OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
  OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)    
 	OSTaskCreate(led0_task,(void *)0,(OS_STK*)&LED0_TASK_STK[LED0_STK_SIZE-1],LED0_TASK_PRIO);						   
 	OSTaskCreate(led1_task,(void *)0,(OS_STK*)&LED1_TASK_STK[LED1_STK_SIZE-1],LED1_TASK_PRIO);
	OSTaskCreate(float_task,(void *)0,(OS_STK*)&FLOAT_TASK_STK[FLOAT_STK_SIZE-1],FLOAT_TASK_PRIO);	
	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务.
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
} 

//LED0任务
void led0_task(void *pdata)
{	 	
	while(1)
	{
		LED0=0;
		delay_ms(80);
		LED0=1;
		delay_ms(920);
	};
}

//LED1任务
void led1_task(void *pdata)
{	  
	while(1)
	{
		LED1=0;
		delay_ms(300);
		LED1=1;
		delay_ms(300);
	};
}

//浮点测试任务
void float_task(void *pdata)
{
	static float float_num=0.01;
	while(1)
	{
		float_num+=0.01f;
		kprintf("float_num的值为: %.4f\r\n",float_num);
		delay_ms(500);			//延时500ms
	}
}




