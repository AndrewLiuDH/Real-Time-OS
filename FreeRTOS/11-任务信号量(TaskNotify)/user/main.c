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
static TaskHandle_t xKeyHandle = NULL;
static TaskHandle_t xTASKCHandle = NULL;

static void Key_task(void *pvParameters);
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
static void AppTaskCreate (void)
{	
	xTaskCreate(Key_task, 					/* ������ */
						"Key_task", 				/* ������ */
						512, 							/* ����ջ��С����λ word��Ҳ���� 4 �ֽ� */
						NULL, 						/* ������� */
						3, 								/* �������ȼ�*/
						&xKeyHandle ); 	/* ������ */	

	
	xTaskCreate(TASKC_task, 					/* ������ */
							"TASKC_task", 				/* ������ */
							512, 							/* ����ջ��С����λ word��Ҳ���� 4 �ֽ� */
							NULL, 						/* ������� */
							4, 								/* �������ȼ�*/
							&xTASKCHandle ); 	/* ������ */	
}

UBaseType_t uxSemNum;
//led0������
void Key_task(void *p_arg)
{
	u8 key=0,TaskBnum=0;
	
	while(1)
	{
		key = Key_Scan(0);
		if(key!=0)
		{
			switch(key)
			{
				case 1:
					printf("KEY 1 PRESSED \r\n");
					xTaskNotifyGive(xTASKCHandle);
					break;
				case 2:
					printf("KEY 2 PRESSED  \r\n");
					
					break;
				case 3:
					printf("KEY 3 PRESSED  \r\n");
					break;
				default:
					printf("KEY 4 PRESSED  \r\n");
			}
		}
		TaskBnum++;
		if(TaskBnum==50)
		{
			LED0=!LED0;
			TaskBnum=0;
		}
		vTaskDelay(10);
	}
}

//led1������
void TASKC_task(void *p_arg)
{
	uint32_t ulNotifiedValue=NULL;
	while(1)
	{
//		ulNotifiedValue =  ulTaskNotifyTake(pdFALSE,500);/* �����ź��� */
		ulNotifiedValue =  ulTaskNotifyTake(pdTRUE,500);/* ��ֵ�ź��� */

		if( ulNotifiedValue > 0 )
		{
			printf("Get the Request : %d \r\n",ulNotifiedValue); //�������������Դ������
		}
		LED2 = ~LED2;
		vTaskDelay(800); //��ʱ 1s
	}
}
