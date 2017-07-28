#include "sys.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "includes.h"

/////////////////////////UCOSII��������///////////////////////////////////
//START ����
//�����������ȼ�
#define START_TASK_PRIO      			10 //��ʼ��������ȼ�����Ϊ���
#define START_STK_SIZE  				64//���������ջ��С
OS_STK START_TASK_STK[START_STK_SIZE];//�����ջ	
void start_task(void *pdata);	//������

 			   
//LED0����
#define LED0_TASK_PRIO       			7 //�����������ȼ�
#define LED0_STK_SIZE  		    		64//���������ջ��С
OS_STK LED0_TASK_STK[LED0_STK_SIZE];//�����ջ	
void led0_task(void *pdata);//������

//LED1����
#define LED1_TASK_PRIO       			6 //�����������ȼ�
#define LED1_STK_SIZE  					64//���������ջ��С
OS_STK LED1_TASK_STK[LED1_STK_SIZE];//�����ջ
void led1_task(void *pdata);//������

#define FLOAT_TASK_PRIO 8				//�������ȼ�
#define FLOAT_STK_SIZE 128			//�����ջ��С
OS_TCB FloatTaskTCB;						//������ƿ�
OS_STK FLOAT_TASK_STK[FLOAT_STK_SIZE];//�����ջ
void float_task(void *p_arg);		//������

int main(void)
{
	delay_init(168);
	USART1_Init();
	LED_Init();
	
	OSInit();   
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//������ʼ����
	OSStart();	

	while(1);
}

 //��ʼ����
void start_task(void *pdata)
{
  OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
  OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)    
 	OSTaskCreate(led0_task,(void *)0,(OS_STK*)&LED0_TASK_STK[LED0_STK_SIZE-1],LED0_TASK_PRIO);						   
 	OSTaskCreate(led1_task,(void *)0,(OS_STK*)&LED1_TASK_STK[LED1_STK_SIZE-1],LED1_TASK_PRIO);
	OSTaskCreate(float_task,(void *)0,(OS_STK*)&FLOAT_TASK_STK[FLOAT_STK_SIZE-1],FLOAT_TASK_PRIO);	
	OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.
	OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
} 

//LED0����
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

//LED1����
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

//�����������
void float_task(void *pdata)
{
	static float float_num=0.01;
	while(1)
	{
		float_num+=0.01f;
		kprintf("float_num��ֵΪ: %.4f\r\n",float_num);
		delay_ms(500);			//��ʱ500ms
	}
}




