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

static QueueHandle_t xQueue1 = NULL;
static QueueHandle_t xQueue2 = NULL;

typedef struct Msg
{
	uint8_t ucMessageID;
	uint16_t usData[2];
	uint32_t ulData[2];
}MSG_T;
MSG_T g_tMsg; /* ����һ���ṹ��������Ϣ���� */

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
	/* ���� 10 �� uint8_t ����Ϣ���� */
	xQueue1 = xQueueCreate(10, sizeof(uint8_t));
	if( xQueue1 == 0 )
	{
		/* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
		printf("Creat Queue Error");
	}
	/* ���� 10 ���洢ָ���������Ϣ���У����� CM3/CM4 �ں��� 32 λ����һ��ָ�����ռ�� 4 ���ֽ� */
	xQueue2 = xQueueCreate(10, sizeof(struct Msg *));
	if( xQueue2 == 0 )
	{
		/* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
		printf("Creat Queue Error");
	}
	
	xTaskCreate(KeyScan_Task, 					/* ������ */
							"KeyScan_Task", 				/* ������ */
							512, 							/* ����ջ��С����λ word��Ҳ���� 4 �ֽ� */
							NULL, 						/* ������� */
							3, 								/* �������ȼ�*/
							&xKeyScanHandle); 	/* ������ */	

	xTaskCreate(FlagsProcess_Task, 					/* ������ */
							"FlagsProcess_Task", 				/* ������ */
							512, 							/* ����ջ��С����λ word��Ҳ���� 4 �ֽ� */
							NULL, 						/* ������� */
							4, 								/* �������ȼ�*/
							&xFlagsProcessHandle ); 	/* ������ */	
}

static void KeyScan_Task(void *pvParameters)
{
	static uint8_t key,TaskBnum=0;
	MSG_T *ptMsg;
	uint8_t ucCount = 0;
	uint8_t ucKeyCode;
	/* ��ʼ���ṹ��ָ�� */
	ptMsg = &g_tMsg;
	/* ��ʼ������ */
	ptMsg->ucMessageID = 0;
	ptMsg->ulData[0] = 0;
	ptMsg->usData[0] = 0;

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
					ucCount++;
					/* ����Ϣ���з����ݣ������Ϣ�������ˣ��ȴ� 10 ��ʱ�ӽ��� */
					xQueueSend(xQueue1,(void *) &ucCount,(TickType_t)10);
					
				printf("Queue1 Free Space: %ld \r\n",uxQueueSpacesAvailable(xQueue1));
					break;
				case 2:
					printf("KEY 2 PRESSED  \r\n");
					ptMsg->ucMessageID++;
					ptMsg->ulData[0]++;;
					ptMsg->usData[0]++;

				/* ����Ϣ���з����ݣ������Ϣ�������ˣ��ȴ� 10 ��ʱ�ӽ��� */
					xQueueSend(xQueue2, 				/* ��Ϣ���о�� */
										(void *) &ptMsg, 	/* ���ͽṹ��ָ����� ptMsg �ĵ�ַ */
										(TickType_t)10);
					printf("Queue2 Free Space: %ld \r\n",uxQueueSpacesAvailable(xQueue2));
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
	uint8_t num,ucQueueMsgValue;
	BaseType_t xResult;
//	MSG_T *ptMsg;
	
	while(1)
	{
		xResult = xQueueReceive(xQueue1, /* ��Ϣ���о�� */
													(void *)&ucQueueMsgValue, /* �����ȡ���ǽṹ��ĵ�ַ */
													(TickType_t)200);/* ��������ʱ�� */
		if(xResult == pdPASS)
		{
			printf("���յ���Ϣ�������� ptMsg->ucMessageID = %d\r\n", ucQueueMsgValue);
			
//			/* �ɹ����գ���ͨ�����ڽ����ݴ�ӡ���� */
//			printf("���յ���Ϣ�������� ptMsg->ucMessageID = %d\r\n", ptMsg->ucMessageID);
//			printf("���յ���Ϣ�������� ptMsg->ulData[0] = %d\r\n", ptMsg->ulData[0]);
//			printf("���յ���Ϣ�������� ptMsg->usData[0] = %d\r\n", ptMsg->usData[0]);
		}else
		{
			/* ��ʱ */
			printf("û�н��յ���Ϣ�������� \r\n");
		}
		vTaskDelay(500);
	}
}

