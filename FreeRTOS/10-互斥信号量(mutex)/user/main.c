#include "sys.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "string.h"
#include "usart.h"
#include "semphr.h"

void vApplicationIdleHook(void)
{
//  printf("StackOverflow \r\n");
}
void vApplicationTickHook(void)
{
#if ( mainCREATE_SIMPLE_BLINKY_DEMO_ONLY == 0 )
    {
        // In this case the tick hook is used as part of the queue set test. 
    }
#endif // mainCREATE_SIMPLE_BLINKY_DEMO_ONLY 
}
void vApplicationMallocFailedHook(void)
{
	printf("MallocFailed \r\n");
	taskDISABLE_INTERRUPTS();
	for(;;);
}

void vApplicationStackOverflowHook(void)
{
	printf("StackOverflow \r\n");
}


static void AppTaskCreate (void);

/* ������ */
static TaskHandle_t xTASKAHandle = NULL;
static TaskHandle_t xTASKBHandle = NULL;
static TaskHandle_t xTASKCHandle = NULL;

static void TASKA_task(void *pvParameters);
static void TASKB_task(void *pvParameters);
static void TASKC_task(void *pvParameters);


/*ժ�Թٷ��ֲ�
Mutexes include a priority inheritance mechanism, binary semaphores do not. This makes
binary semaphores the better choice for implementing synchronization (between tasks or
between tasks and an interrupt), and mutexes the better choice for implementing simple
mutual exclusion
*/

int main(void)
{	
	delay_init(168);
	LED_Init();
	KEY_init();
	USART1_init();

	/* �������� */
	AppTaskCreate();
	
	/* �������ȣ���ʼִ������ */
	vTaskStartScheduler();
}

/*
*********************************************************************************************************
* �� �� ��: AppTaskCreate
* ����˵��: ����Ӧ������
* �� ��: ��
* �� �� ֵ: ��
*********************************************************************************************************
*/
SemaphoreHandle_t xMutex = NULL;

static void AppTaskCreate (void)
{
	xMutex = xSemaphoreCreateMutex();/* ���������ź��� */
	
	xTaskCreate(TASKA_task, 					/* ������ */
							"TASKA_task", 				/* ������ */
							512, 							/* ����ջ��С����λ word��Ҳ���� 4 �ֽ� */
							NULL, 						/* ������� */
							3, 								/* �������ȼ�*/
							&xTASKAHandle ); 	/* ������ */	

	xTaskCreate(TASKB_task, 					/* ������ */
							"TASKB_task", 				/* ������ */
							512, 							/* ����ջ��С����λ word��Ҳ���� 4 �ֽ� */
							NULL, 						/* ������� */
							4, 								/* �������ȼ�*/
							&xTASKBHandle ); 	/* ������ */	
	
	xTaskCreate(TASKC_task, 					/* ������ */
							"TASKC_task", 				/* ������ */
							512, 							/* ����ջ��С����λ word��Ҳ���� 4 �ֽ� */
							NULL, 						/* ������� */
							5, 								/* �������ȼ�*/
							&xTASKCHandle ); 	/* ������ */	
}

UBaseType_t uxSemNum;
//led0������
void TASKA_task(void *p_arg)//�����ȼ�
{
	static u32 times;

	while(1)
	{
		xSemaphoreTake(xMutex,portMAX_DELAY);
		LED0=!LED0;
		printf("Low_task Now RUNNING  �� \r\n"); 
		vTaskDelay(5000);
		xSemaphoreGive(xMutex);
		vTaskDelay(1000);
	}
}

//led1������
void TASKB_task(void *p_arg)//�����ȼ�
{	
	while(1)
	{
		printf("Middle_task Now RUNNING \r\n"); 
		LED1 = ~LED1;
		vTaskDelay(1000);
	}
}


//led1������
void TASKC_task(void *p_arg)//�����ȼ�
{
//	xSemaphoreGive(xSemaphore);

	while(1)
	{
		uxSemNum = uxSemaphoreGetCount(xMutex);
		xSemaphoreTake(xMutex,portMAX_DELAY);
		printf("High_task Now RUNNING  �� \r\n");
		LED2 = ~LED2;
		xSemaphoreGive(xMutex);
		vTaskDelay(500);

	}
}
