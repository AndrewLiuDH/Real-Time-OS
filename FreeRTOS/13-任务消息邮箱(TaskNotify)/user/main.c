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

/* 任务句柄 */
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
static void AppTaskCreate (void)
{
	
	xTaskCreate(Key_task, 					/* 任务函数 */
						"Key_task", 				/* 任务名 */
						512, 							/* 任务栈大小，单位 word，也就是 4 字节 */
						NULL, 						/* 任务参数 */
						3, 								/* 任务优先级*/
						&xKeyHandle ); 	/* 任务句柄 */	

	
	xTaskCreate(TASKC_task, 					/* 任务函数 */
							"TASKC_task", 				/* 任务名 */
							512, 							/* 任务栈大小，单位 word，也就是 4 字节 */
							NULL, 						/* 任务参数 */
							4, 								/* 任务优先级*/
							&xTASKCHandle ); 	/* 任务句柄 */	
}

//led0任务函数
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
					xTaskNotify(xTASKCHandle, /* 目标任务 */
											ucCount++, /* 发送数据 */
											eSetValueWithOverwrite);/* 如果目标任务上次的数据还没有处理，会被覆盖*/
				break;
				case 2:
					printf("KEY 2 PRESSED  \r\n");
					/* 非覆盖模式的数据发送 */
					if(xTaskNotify(xTASKCHandle, ucCount++, eSetValueWithoutOverwrite) == pdPASS)
					{
						/* 目标任务的消息邮箱数据被更新 */
						printf("任务 vTaskMsgPro 的消息邮箱被更新\r\n");
					}
					else
					{
						/* 目标任务的消息邮箱数据未更新，因为上次的数据还没有处理，不能进行覆盖 */
						printf("任务 vTaskMsgPro 的消息邮箱数据未被更新\r\n");
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

//led1任务函数
void TASKC_task(void *p_arg)
{
	BaseType_t xResult;
	uint32_t ulValue;
	
	while(1)
	{
		xResult = xTaskNotifyWait(0x00000000,
															0xFFFFFFFF,
															&ulValue, /* 保存 ulNotifiedValue 到变量 ulValue 中 */
															500); /* 最大允许延迟时间 */
		if( xResult == pdPASS )
		{
			printf("接收到消息邮箱数据 ulValue = %d\r\n", ulValue);
		}
		LED2 = ~LED2;
		vTaskDelay(500);
	}
}
