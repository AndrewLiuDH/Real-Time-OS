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
	delay_init(168);
	LED_Init();
	USART1_init();

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

	while(1)
	{
		TaskBnum++;
		printf("����B : %d \r\n",TaskBnum);
		LED0=!LED0;
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
		LED1=!LED1;
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


