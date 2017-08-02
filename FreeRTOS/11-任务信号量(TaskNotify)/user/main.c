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

UBaseType_t uxSemNum;
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
					xTaskNotifyGive(xTASKCHandle);
					break;
				case 2:
					printf("KEY 2 PRESSED  \r\n");
					
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
	uint32_t ulNotifiedValue=NULL;
	while(1)
	{
//		ulNotifiedValue =  ulTaskNotifyTake(pdFALSE,500);/* 计数信号量 */
		ulNotifiedValue =  ulTaskNotifyTake(pdTRUE,500);/* 二值信号量 */

		if( ulNotifiedValue > 0 )
		{
			printf("Get the Request : %d \r\n",ulNotifiedValue); //串口输出共享资源区数据
		}
		LED2 = ~LED2;
		vTaskDelay(800); //延时 1s
	}
}
