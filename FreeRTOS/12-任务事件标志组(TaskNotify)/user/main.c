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
	
	while(1)
	{
		key = Key_Scan(0);
		if(key!=0)
		{
			switch(key)
			{
				case 1:
					printf("KEY 1 PRESSED \r\n");
					printf("K1 �����£����� vTaskMsgPro �¼���־λ bit0 ����λ \r\n");
					xTaskNotify(xTASKCHandle, /* Ŀ������ */
											0x01, /* ����Ŀ�������¼���־λ bit0 */
											eSetBits); /* ��Ŀ��������¼���־λ�� BIT_0 ���л�������������ֵ���¼���־λ�� */
					break;
				case 2:
					printf("KEY 2 PRESSED  \r\n");
					printf("K2 �����£����� vTaskMsgPro �¼���־λ bit1 ����λ \r\n");
					xTaskNotify(xTASKCHandle, /* Ŀ������ */
											0x02, /* ����Ŀ�������¼���־λ bit0 */
											eSetBits); /* ��Ŀ��������¼���־λ�� BIT_0 ���л�������������ֵ���¼���־λ�� */
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
															300); /* ��������ӳ�ʱ�� */
		if( xResult == pdPASS )
		{
			/* ���յ���Ϣ������Ǹ�λ������ */
			if((ulValue & 0x01) != 0)
			{
				printf("���յ� K1 ����������Ϣ, ulNotifiedValue = 0x%08x\r\n", ulValue);
			}
			if((ulValue & 0x02) != 0)
			{
				printf("���յ� K2 ����������Ϣ, ulNotifiedValue = 0x%08x\r\n", ulValue);
			}
		}
	}
}
