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

//led0������
void Key_task(void *p_arg)
{
	u8 key=0,TaskBnum=0;
	uint8_t ucCount = 0;
	uint8_t pcWriteBuffer[500];
	
	while(1)
	{
		key = Key_Scan(0);
		if(key!=0)
		{
			switch(key)
			{
				case 1:
					printf("KEY 1 PRESSED \r\n");
					xTaskNotify(xTASKCHandle, /* Ŀ������ */
											ucCount++, /* �������� */
											eSetValueWithOverwrite);/* ���Ŀ�������ϴε����ݻ�û�д����ᱻ����*/
				break;
				case 2:
					printf("KEY 2 PRESSED  \r\n");
					/* �Ǹ���ģʽ�����ݷ��� */
					if(xTaskNotify(xTASKCHandle, ucCount++, eSetValueWithoutOverwrite) == pdPASS)
					{
						/* Ŀ���������Ϣ�������ݱ����� */
						printf("���� vTaskMsgPro ����Ϣ���䱻����\r\n");
					}
					else
					{
						/* Ŀ���������Ϣ��������δ���£���Ϊ�ϴε����ݻ�û�д������ܽ��и��� */
						printf("���� vTaskMsgPro ����Ϣ��������δ������\r\n");
					}
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
	BaseType_t xResult;
	uint32_t ulValue;
	
	while(1)
	{
		xResult = xTaskNotifyWait(0x00000000,
															0xFFFFFFFF,
															&ulValue, /* ���� ulNotifiedValue ������ ulValue �� */
															500); /* ��������ӳ�ʱ�� */
		if( xResult == pdPASS )
		{
			printf("���յ���Ϣ�������� ulValue = %d\r\n", ulValue);
		}
		LED2 = ~LED2;
		vTaskDelay(500);
	}
}
