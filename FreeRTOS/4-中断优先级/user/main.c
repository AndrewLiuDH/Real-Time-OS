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
static void vTaskTaskUserIF(void *pvParameters);
static void vTaskLED(void *pvParameters);
static void vTaskLED(void *pvParameters);
static void StackOverflowTest(void);

int main(void)
{	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	delay_init(168);
	LED_Init();
	KEY_init();
	USART1_init();

	/* �������� */
	AppTaskCreate();
	
	/* �������ȣ���ʼִ������ */
	vTaskStartScheduler();
}

/* ������ */
static TaskHandle_t xHandleTaskUserIF = NULL;
static TaskHandle_t xHandleTaskLED = NULL;
static TaskHandle_t xHandleTaskMsgPro = NULL;
static TaskHandle_t xHandleTaskStart = NULL;
static void vTaskMsgPro(void *pvParameters);
static void vTaskStart(void *pvParameters);


/*
*********************************************************************************************************
* �� �� ��: AppTaskCreate
* ����˵��: ����Ӧ������
* �� �Σ���
* �� �� ֵ: ��
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
	xTaskCreate( vTaskTaskUserIF, /* ������ */
							"vTaskUserIF", /* ������ */
							512, /* ����ջ��С����λ word��Ҳ���� 4 �ֽ� */
							NULL, /* ������� */
							1, /* �������ȼ�*/
							&xHandleTaskUserIF ); /* ������ */
	
	xTaskCreate( vTaskLED, /* ������ */
							"vTaskLED", /* ������ */
							512, /* ����ջ��С����λ word��Ҳ���� 4 �ֽ� */
							NULL, /* ������� */
							2, /* �������ȼ�*/
							&xHandleTaskLED ); /* ������ */
	
	xTaskCreate( vTaskMsgPro, /* ������ */
							"vTaskMsgPro", /* ������ */
							512, /* ����ջ��С����λ word��Ҳ���� 4 �ֽ� */
							NULL, /* ������� */
							3, /* �������ȼ�*/
							&xHandleTaskMsgPro ); /* ������ */
	
	xTaskCreate( vTaskStart, /* ������ */
							"vTaskStart", /* ������ */
							512, /* ����ջ��С����λ word��Ҳ���� 4 �ֽ� */
							NULL, /* ������� */
							4, /* �������ȼ�*/
							&xHandleTaskStart ); /* ������ */
}

/*
*********************************************************************************************************
* �� �� ��: vTaskTaskUserIF
* ����˵��: �ӿ���Ϣ����
* �� ��: pvParameters ���ڴ���������ʱ���ݵ��β�
* �� �� ֵ: ��
* �� �� ��: 1 (��ֵԽС���ȼ�Խ�ͣ������ uCOS �෴)
*********************************************************************************************************
*/
static void vTaskTaskUserIF(void *pvParameters)
{
	uint8_t ucKeyCode;

	while(1)
	{
		ucKeyCode = KEY_Scan(0);
		if (ucKeyCode != 0)
		{
			switch (ucKeyCode)
			{
				/* K1 ������ ��ӡ����ִ����� */
				case 1:
							printf("=================================================\r\n");
							printf("������ ����״̬ ���ȼ� ʣ��ջ �������\r\n");
							
							printf("\r\n ������ ���м��� ʹ����\r\n");
				break;
				/* K2 ������ ɾ������ vTaskLED */
				case 2:
							printf("K2 �����£�ģ������ջ������\r\n");
							StackOverflowTest();
				break;
				/* �����ļ�ֵ������ */
				default:
				break;
			}
		}
		vTaskDelay(20);
	}
}
/*
*********************************************************************************************************
* �� �� ��: vTaskLED
* ����˵��: LED ��˸
* �� ��: pvParameters ���ڴ���������ʱ���ݵ��β�
* �� �� ֵ: ��
* �� �� ��: 2
*********************************************************************************************************
*/
static void vTaskLED(void *pvParameters)
{
	while(1)
	{
		LED2=!LED2;
		vTaskDelay(200);
	}
}
/*
*********************************************************************************************************
* �� �� ��: vTaskMsgPro
* ����˵��: ��Ϣ�������������� LED ��˸
* �� ��: pvParameters ���ڴ���������ʱ���ݵ��β�
* �� �� ֵ: ��
* �� �� ��: 3
*********************************************************************************************************
*/
static void vTaskMsgPro(void *pvParameters)
{
	while(1)
	{
		LED3=!LED3;
		vTaskDelay(300);
	}
}
/*
*********************************************************************************************************
* �� �� ��: vTaskStart
* ����˵��: ��������Ҳ����������ȼ�����������������ɨ�衣
* �� ��: pvParameters ���ڴ���������ʱ���ݵ��β�
* �� �� ֵ: ��
* �� �� ��: 4
*********************************************************************************************************
*/
static void vTaskStart(void *pvParameters)
{
	while(1)
	{
		/* ����ɨ�� */
//		bsp_KeyScan();
		vTaskDelay(100);
	}
}

static void StackOverflowTest(void)
{
	int16_t i;
	uint8_t buf[2048];
	(void)buf; /* ��ֹ���� */
	/*
		1. Ϊ���ܹ�ģ������ջ���������������ջ�������������ǿ�ҽ���ʹ�������ʱ�����Ÿ�ֵ��
		��Ϊ���� M3 �� M4 �ں˵� MCU����ջ����������������������ջ�����ߵ�ַ�� buf[2047], �͵�ַ
		�� buf[0]���������ջ����ˣ�Ҳ�ǴӸߵ�ַ buf[2047]�� buf[0]��ĳ����ַ��ʼ�����
		��ˣ�����û�ֱ���޸ĵ��� buf[0]��ʼ����������Щ������ֵ����ݱȽ���Ҫ����ֱ�ӵ���
		���뵽Ӳ���쳣��
		2. ջ���������������л���ʱ��ִ�еģ���������Ӹ��ӳٺ�������ֹ�޸�����Ҫ�����ݵ���ֱ��
		����Ӳ���쳣��
		3. ���� vTaskTaskUserIF ��ջ�ռ��С�� 2048 �ֽڣ��ڴ����������Ѿ�������ջ�ռ��С
		------uint8_t ucKeyCode;
		------uint8_t pcWriteBuffer[500];
		����������������ô���ջ�ռ�
		-------int16_t i;
		-------uint8_t buf[2048];
		�ض������
	*/
	for(i = 2047; i >= 0; i--)
	{
		buf[i] = 0x55;
		vTaskDelay(1);
	}
}


