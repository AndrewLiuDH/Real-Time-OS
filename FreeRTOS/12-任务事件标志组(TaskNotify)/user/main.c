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
	
	while(1)
	{
		key = Key_Scan(0);
		if(key!=0)
		{
			switch(key)
			{
				case 1:
					printf("KEY 1 PRESSED \r\n");
					printf("K1 键按下，任务 vTaskMsgPro 事件标志位 bit0 被置位 \r\n");
					xTaskNotify(xTASKCHandle, /* 目标任务 */
											0x01, /* 设置目标任务事件标志位 bit0 */
											eSetBits); /* 将目标任务的事件标志位与 BIT_0 进行或操作，将结果赋值给事件标志位。 */
					break;
				case 2:
					printf("KEY 2 PRESSED  \r\n");
					printf("K2 键按下，任务 vTaskMsgPro 事件标志位 bit1 被置位 \r\n");
					xTaskNotify(xTASKCHandle, /* 目标任务 */
											0x02, /* 设置目标任务事件标志位 bit0 */
											eSetBits); /* 将目标任务的事件标志位与 BIT_0 进行或操作，将结果赋值给事件标志位。 */
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
															300); /* 最大允许延迟时间 */
		if( xResult == pdPASS )
		{
			/* 接收到消息，检测那个位被按下 */
			if((ulValue & 0x01) != 0)
			{
				printf("接收到 K1 按键按下消息, ulNotifiedValue = 0x%08x\r\n", ulValue);
			}
			if((ulValue & 0x02) != 0)
			{
				printf("接收到 K2 按键按下消息, ulNotifiedValue = 0x%08x\r\n", ulValue);
			}
		}
	}
}
