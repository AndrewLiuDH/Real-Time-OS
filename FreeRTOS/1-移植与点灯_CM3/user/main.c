#include "sys.h"
#include "led.h"
#include "delay.h"
#include "usart.h"

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


int main(void)
{	
	USART2_Config();
	LED_Init();

	/* �������� */
	AppTaskCreate();
	
	/* �������ȣ���ʼִ������ */
	vTaskStartScheduler();
}

/* ������ */
static TaskHandle_t xHandleTaskB = NULL;
static TaskHandle_t xHandleTaskC = NULL;

static void vTaskB(void *pvParameters)
{
	static uint8_t TaskBnum=0;
	uint32_t ulNotifiedValue;

	while(1)
	{
		/* Block to wait for prvTask1() to notify this task. */
		ulNotifiedValue = ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
		
		TaskBnum++;
		printf("����B : %d \r\n",TaskBnum);
		LED=!LED;
		vTaskDelay(500);//�˳�����500��ticks
	}
}

static void vTaskC(void *pvParameters)
{
	static uint8_t TaskCnum=0;
	
	while(1)
	{
		TaskCnum++;
		printf("����C : %d \r\n",TaskCnum);
		/* Send a notification to prvTask2(), bringing it out of the Blocked
		state. */
		xTaskNotifyGive( xHandleTaskB );
//		LED=!LED;
		vTaskDelay(500);//�˳�����500��ticks
	}
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
	xTaskCreate(vTaskB, 					/* ������ */
							"vTaskB", 				/* ������ */
							512, 							/* ����ջ��С����λ word��Ҳ���� 4 �ֽ� */
							NULL, 						/* ������� */
							4, 								/* �������ȼ�*/
							&xHandleTaskB ); 	/* ������ */
	
	xTaskCreate(vTaskC, 					/* ������ */
							"vTaskC", 				/* ������ */
							512, 							/* ����ջ��С����λ word��Ҳ���� 4 �ֽ� */
							NULL, 						/* ������� */
							3, 								/* �������ȼ�*/
							&xHandleTaskC ); 	/* ������ */
	
	
}