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
static TaskHandle_t xTASKBHandle = NULL;
static TaskHandle_t xTASKCHandle = NULL;

static void TASKB_task(void *pvParameters);
static void TASKC_task(void *pvParameters);

#define KEY0_FLAG 0x01
#define KEY1_FLAG 0x02

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
	xSemaphore = xSemaphoreCreateCounting(1, 0);/* ���������ź��� */
	
	xTaskCreate(TASKB_task, 					/* ������ */
						"TASKB_task", 				/* ������ */
						512, 							/* ����ջ��С����λ word��Ҳ���� 4 �ֽ� */
						NULL, 						/* ������� */
						3, 								/* �������ȼ�*/
						&xTASKBHandle ); 	/* ������ */	

	
	xTaskCreate(TASKC_task, 					/* ������ */
							"TASKC_task", 				/* ������ */
							512, 							/* ����ջ��С����λ word��Ҳ���� 4 �ֽ� */
							NULL, 						/* ������� */
							4, 								/* �������ȼ�*/
							&xTASKCHandle ); 	/* ������ */	
}

u8 share_resource[30]; //������Դ��
//led0������
void TASKB_task(void *p_arg)
{
	u8 task1_str[]="First task Running!";
	
	while(1)
	{
		xSemaphoreTake(xSemaphore,portMAX_DELAY);
		printf("\r\n ���� 1:\r\n");
		memcpy(share_resource,task1_str,sizeof(task1_str)); //������Դ���������� (1)
//		vTaskDelay(200);//�ᵼ�������л�
		printf("%s\r\n",share_resource); //�������������Դ������
		LED1 = ~LED1;
		xSemaphoreGive(xSemaphore);
		vTaskDelay(800); //��ʱ 1s
	}
}

//led1������
void TASKC_task(void *p_arg)
{
	u8 i=0;
	u8 task2_str[]="Second task Running!";
	
	while(1)
	{
		xSemaphoreTake(xSemaphore,portMAX_DELAY);
		printf("\r\n ���� 2:\r\n");
		memcpy(share_resource,task2_str,sizeof(task2_str));//������Դ����������
//		vTaskDelay(200);//�ᵼ�������л�
		printf("%s\r\n",share_resource); //�������������Դ������
		LED2 = ~LED2;
		xSemaphoreGive(xSemaphore);
		vTaskDelay(800); //��ʱ 1s
	}
}
