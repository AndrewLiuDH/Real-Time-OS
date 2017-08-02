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

/* 任务句柄 */
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

	/* 创建任务 */
	AppTaskCreate();
	
	/* 启动调度，开始执行任务 */
	vTaskStartScheduler();
}

/*
*********************************************************************************************************
* 函 数 名: AppTaskCreate
* 功能说明: 创建应用任务
* 形 参: 无
* 返 回 值: 无
*********************************************************************************************************
*/
EventGroupHandle_t xCreatedEventGroup = NULL;
static void AppTaskCreate (void)
{
	xCreatedEventGroup = xEventGroupCreate();
	
	xTaskCreate(KeyScan_Task, 					/* 任务函数 */
						"KeyScan_Task", 				/* 任务名 */
						512, 							/* 任务栈大小，单位 word，也就是 4 字节 */
						NULL, 						/* 任务参数 */
						4, 								/* 任务优先级*/
						&xKeyScanHandle ); 	/* 任务句柄 */	

	
	xTaskCreate(FlagsProcess_Task, 					/* 任务函数 */
							"FlagsProcess_Task", 				/* 任务名 */
							512, 							/* 任务栈大小，单位 word，也就是 4 字节 */
							NULL, 						/* 任务参数 */
							4, 								/* 任务优先级*/
							&xFlagsProcessHandle ); 	/* 任务句柄 */	
}

static void KeyScan_Task(void *pvParameters)
{
	static uint8_t key=0,TaskBnum=0;

	while(1)
	{
		key=KEY_Scan(0);
		if(key!=0)
		{
//			printf("任务B : %d \r\n",TaskBnum);			
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
		vTaskDelay(10);//退出调度500个ticks
	}
}


static void FlagsProcess_Task(void *pvParameters)
{
	uint8_t num;
	EventBits_t uxBits;
	
	while(1)
	{
		uxBits = xEventGroupWaitBits(xCreatedEventGroup, /* 事件标志组句柄 */
												KEY0_FLAG+KEY1_FLAG, 									/* 等待 bit0 和 bit1 被设置 */
												pdTRUE, 										/* 退出前 bit0 和 bit1 被清除，这里是 bit0 和 bit1都被设置才表示“退出” */
												pdTRUE, 										/* 设置为 pdTRUE 表示等待 bit1 和 bit0 都被设置*/
												portMAX_DELAY); 						/* 等待延迟时间 */
	
		num++;
		LED1 = ~LED1;
		printf("事件标志组 EventFlags 的值:%d\r\n",uxBits);	
	}
}

