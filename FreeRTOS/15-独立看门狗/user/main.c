#include "sys.h"
#include "led.h"
#include "delay.h"
#include "iwdg.h"
#include "usart.h"
#include "event_groups.h"

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
static TaskHandle_t xHandleTaskA = NULL;
static TaskHandle_t xHandleTaskB = NULL;
static TaskHandle_t xHandleTaskC = NULL;
static TaskHandle_t xHandleTaskD = NULL;

EventGroupHandle_t xCreatedEventGroup;


static void vTaskA(void *pvParameters)
{
	EventBits_t uxBits;
	const TickType_t xTicksToWait = 100 / portTICK_PERIOD_MS; /* ����ӳ� 100ms */
	/*
	��ʼִ����������������ǰʹ�ܶ������Ź���
	���� LSI �� 128 ��Ƶ�����溯��������Χ 0-0xFFF���ֱ������Сֵ 3.2ms �����ֵ 13107.2ms
	�������õ��� 10s����� 10s ��û��ι����ϵͳ��λ��
	*/
	IWDG_Init(4,5000);
	/* ��ӡϵͳ����״̬������鿴ϵͳ�Ƿ�λ */
	printf("=====================================================\r\n");
	printf("=ϵͳ����ִ��\r\n");
	printf("=====================================================\r\n");
	while(1)
	{
		/* �ȴ������������¼���־ */
		uxBits = xEventGroupWaitBits(xCreatedEventGroup, /* �¼���־���� */
																0x07, /* �ȴ� TASK_BIT_ALL ������ */
																pdTRUE, /* �˳�ǰ TASK_BIT_ALL ������������� TASK_BIT_ALL
																�������òű�ʾ���˳��� */
																pdTRUE, /* ����Ϊ pdTRUE ��ʾ�ȴ� TASK_BIT_ALL ��������*/
																xTicksToWait); /* �ȴ��ӳ�ʱ�� */
		if((uxBits & 0x07) == 0x07)
		{
			IWDG_Feed();
			printf("����û�������������\r\n");
		}
		else
		{
			/* ������ÿ xTicksToWait ����һ�� */
			/* ͨ������ uxBits �򵥵Ŀ����ڴ˴�����Ǹ�������û�з������б�־ */
		}
	}
}

static void vTaskB(void *pvParameters)
{
	static uint8_t TaskBnum=0;

	while(1)
	{
		TaskBnum++;
		printf("����B : %d \r\n",TaskBnum);
		LED2=!LED2;
		/* �����¼���־����ʾ������������ */
		xEventGroupSetBits(xCreatedEventGroup, 0x01);
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
		LED3=!LED3;
		/* �����¼���־����ʾ������������ */
		xEventGroupSetBits(xCreatedEventGroup, 0x02);
		vTaskDelay(500);//�˳�����500��ticks
	}
}

static void vTaskD(void *pvParameters)
{
	static uint8_t TaskDnum=0;
	
	while(1)
	{
		TaskDnum++;
		printf("����D : %d \r\n",TaskDnum);
		LED1=!LED1;
		/* �����¼���־����ʾ������������ */
		xEventGroupSetBits(xCreatedEventGroup, 0x04);
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
	/* �����¼���־�� */
	xCreatedEventGroup = xEventGroupCreate();
	
	xTaskCreate(vTaskA, 					/* ������ */
							"vTaskA", 				/* ������ */
							512, 							/* ����ջ��С����λ word��Ҳ���� 4 �ֽ� */
							NULL, 						/* ������� */
							1, 								/* �������ȼ�*/
							&xHandleTaskA ); 	/* ������ */
	
	xTaskCreate(vTaskB, 					/* ������ */
							"vTaskB", 				/* ������ */
							512, 							/* ����ջ��С����λ word��Ҳ���� 4 �ֽ� */
							NULL, 						/* ������� */
							2, 								/* �������ȼ�*/
							&xHandleTaskB ); 	/* ������ */
	
	xTaskCreate(vTaskC, 					/* ������ */
							"vTaskC", 				/* ������ */
							512, 							/* ����ջ��С����λ word��Ҳ���� 4 �ֽ� */
							NULL, 						/* ������� */
							3, 								/* �������ȼ�*/
							&xHandleTaskC ); 	/* ������ */
	
	xTaskCreate(vTaskD, 					/* ������ */
							"vTaskD", 				/* ������ */
							512, 							/* ����ջ��С����λ word��Ҳ���� 4 �ֽ� */
							NULL, 						/* ������� */
							4, 								/* �������ȼ�*/
							&xHandleTaskD ); 	/* ������ */
	
}


