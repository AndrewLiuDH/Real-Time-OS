#include "sys.h"
#include "led.h"
#include "key.h"
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

/* ������ */
static TaskHandle_t xFlagsProcessHandle = NULL;
static TaskHandle_t xKeyScanHandle = NULL;

static void FlagsProcess_Task(void *pvParameters);
static void KeyScan_Task(void *pvParameters);

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
EventGroupHandle_t xCreatedEventGroup = NULL;
static void AppTaskCreate (void)
{
	xCreatedEventGroup = xEventGroupCreate();
	
	xTaskCreate(KeyScan_Task, 					/* ������ */
						"KeyScan_Task", 				/* ������ */
						512, 							/* ����ջ��С����λ word��Ҳ���� 4 �ֽ� */
						NULL, 						/* ������� */
						4, 								/* �������ȼ�*/
						&xKeyScanHandle ); 	/* ������ */	

	
	xTaskCreate(FlagsProcess_Task, 					/* ������ */
							"FlagsProcess_Task", 				/* ������ */
							512, 							/* ����ջ��С����λ word��Ҳ���� 4 �ֽ� */
							NULL, 						/* ������� */
							4, 								/* �������ȼ�*/
							&xFlagsProcessHandle ); 	/* ������ */	
}

static void KeyScan_Task(void *pvParameters)
{
	static uint8_t key=0,TaskBnum=0;

	while(1)
	{
		key=KEY_Scan(0);
		if(key!=0)
		{
//			printf("����B : %d \r\n",TaskBnum);			
			switch(key)
			{
				case 1:
					printf("KEY 1 PRESSED \r\n");
					xEventGroupSetBits(xCreatedEventGroup, KEY0_FLAG);
					break;
				case 2:
					printf("KEY 2 PRESSED  \r\n");
					xEventGroupSetBits(xCreatedEventGroup, KEY1_FLAG);
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
		vTaskDelay(10);//�˳�����500��ticks
	}
}


static void FlagsProcess_Task(void *pvParameters)
{
	uint8_t num;
	EventBits_t uxBits;
	
	while(1)
	{
		uxBits = xEventGroupWaitBits(xCreatedEventGroup, /* �¼���־���� */
												KEY0_FLAG+KEY1_FLAG, 									/* �ȴ� bit0 �� bit1 ������ */
												pdTRUE, 										/* �˳�ǰ bit0 �� bit1 ������������� bit0 �� bit1�������òű�ʾ���˳��� */
												pdTRUE, 										/* ����Ϊ pdTRUE ��ʾ�ȴ� bit1 �� bit0 ��������*/
												portMAX_DELAY); 						/* �ȴ��ӳ�ʱ�� */
	
		num++;
		LED1 = ~LED1;
		printf("�¼���־�� EventFlags ��ֵ:%d\r\n",uxBits);	
	}
}

