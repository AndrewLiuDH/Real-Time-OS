#include "sys.h"
#include "led.h"
#include "exit.h"
#include "delay.h"
#include "usart.h"
#include "includes.h"

#define START_TASK_PRIO 3				//�������ȼ�
#define START_STK_SIZE 512			//�����ջ��С
OS_TCB StartTaskTCB;						//������ƿ�
CPU_STK START_TASK_STK[START_STK_SIZE];//�����ջ
void start_task(void *p_arg);		//������

#define TASKB_TASK_PRIO 5				//�������ȼ�
#define TASKB_STK_SIZE 128				//�����ջ��С
OS_TCB TASKBTaskTCB;							//������ƿ�
CPU_STK TASKB_TASK_STK[TASKB_STK_SIZE];//�����ջ
void TASKB_task(void *p_arg);		//������

#define TASKC_TASK_PRIO 5				//�������ȼ�
#define TASKC_STK_SIZE 128				//�����ջ��С
OS_TCB TASKCTaskTCB;							//������ƿ�
CPU_STK TASKC_TASK_STK[TASKC_STK_SIZE];//�����ջ
void TASKC_task(void *p_arg);		//������


int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();

	delay_init(168);
	USART1_Init();
	LED_Init();
	EXTI_init();

	OSInit(&err);		//��ʼ��UCOSIII
	
	OSStart(&err);  //����UCOSIII
	
	while(1);
}