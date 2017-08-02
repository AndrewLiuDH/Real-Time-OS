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
static TaskHandle_t xHandleTaskB = NULL;
static TaskHandle_t xHandleTaskC = NULL;

static void vTimerCallback1(xTimerHandle pxTimer);
static void vTimerCallback2(xTimerHandle pxTimer);

static void vTaskB(void *pvParameters);


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
xTimerHandle xTimer1 = NULL;
xTimerHandle xTimer2 = NULL;


static void AppTaskCreate (void)
{
	xTimer1 = xTimerCreate("Timer1",
								1000,
								pdTRUE,
								(void *)0,
								vTimerCallback1);
								
	xTimer2 = xTimerCreate("Timer2",
							500,
							pdFALSE,
							(void *)1,
							vTimerCallback1);
	
	xTaskCreate(vTaskB, 					/* 任务函数 */
							"vTaskB", 				/* 任务名 */
							512, 							/* 任务栈大小，单位 word，也就是 4 字节 */
							NULL, 						/* 任务参数 */
							4, 								/* 任务优先级*/
							&xHandleTaskB ); 	/* 任务句柄 */	
}

static void vTaskB(void *pvParameters)
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
					xTimerStart(xTimer1, 200);
					break;
				case 2:
					printf("KEY 2 PRESSED  \r\n");
					xTimerStart(xTimer2, 2000);
					break;
				case 3:
					printf("KEY 3 PRESSED  \r\n");
					xTimerStop(xTimer1, 0);
					xTimerStop(xTimer2, 0);
					printf("ShutDown Timer1 And Timer2  \r\n");
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

static void vTimerCallback1(xTimerHandle pxTimer)
{
	uint32_t ulTimerID;
	static uint8_t Timer1Runtimes=0,Timer2Runtimes=0;
	
	configASSERT(pxTimer);
	/* 获取那个定时器时间到 */
	ulTimerID = (uint32_t)pvTimerGetTimerID(pxTimer);
	/* 处理定时器 0 任务 */
	if(ulTimerID == 0)
	{
		LED1=!LED1;
		Timer1Runtimes++;
		printf("Timer 1 RUNNING : %d\r\n",Timer1Runtimes);
	}
	/* 处理定时器 1 任务 */
	if(ulTimerID == 1)
	{
		Timer2Runtimes++;
		LED2=!LED2;
		printf("Timer 2 RUNNING : %d\r\n",Timer2Runtimes);
	}
}