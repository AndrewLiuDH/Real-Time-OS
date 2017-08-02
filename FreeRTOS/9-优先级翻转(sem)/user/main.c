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
QueueHandle_t xSemaphore = NULL;

static void AppTaskCreate (void)
{
	xSemaphore = xSemaphoreCreateCounting(10, 1);/* ���������ź��� */
//	xSemaphore = xSemaphoreCreateBinary();/* ������ֵ�ź��� ����ʼֵΪ0����Ҫ��give*/

	
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
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	
	while(1)
	{
		xSemaphoreTake(xSemaphore,portMAX_DELAY);
		LED0=!LED0;
		vTaskDelayUntil(&xLastWakeTime,5000);
		xSemaphoreGive(xSemaphore);
		printf("Low_task Now RUNNING  �� \r\n"); 
		vTaskDelay(1000); //��ʱ 1s
	}
}

//led1������
void TASKB_task(void *p_arg)//�����ȼ�
{	
	while(1)
	{
		printf("Middle_task Now RUNNING \r\n"); 
		LED1 = ~LED1;
		vTaskDelay(1000); //��ʱ 1s
	}
}


//led1������
void TASKC_task(void *p_arg)//�����ȼ�
{
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

	while(1)
	{
		uxSemNum = uxSemaphoreGetCount(xSemaphore);
		xSemaphoreTake(xSemaphore,portMAX_DELAY);
		printf("High_task Now RUNNING  �� \r\n");
		LED2 = ~LED2;
		xSemaphoreGive(xSemaphore);
		vTaskDelayUntil(&xLastWakeTime,500);
	}
}
