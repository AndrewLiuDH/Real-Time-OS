#include "sys.h"
#include "led.h"
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


int main(void)
{	
	delay_init(168);
	LED_Init();
	USART1_init();

	/* 创建任务 */
	AppTaskCreate();
	
	/* 启动调度，开始执行任务 */
	vTaskStartScheduler();
}

/* 任务句柄 */
static TaskHandle_t xHandleTaskB = NULL;
static TaskHandle_t xHandleTaskC = NULL;

static void vTaskB(void *pvParameters)
{
	static uint8_t TaskBnum=0;

	while(1)
	{
		TaskBnum++;
		printf("任务B : %d \r\n",TaskBnum);
		LED0=!LED0;
		vTaskDelay(500);//退出调度500个ticks
	}
}

static void vTaskC(void *pvParameters)
{
	static uint8_t TaskCnum=0;
	
	while(1)
	{
		TaskCnum++;
		printf("任务C : %d \r\n",TaskCnum);
		LED1=!LED1;
		vTaskDelay(500);//退出调度500个ticks
	}
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
	xTaskCreate(vTaskB, 					/* 任务函数 */
							"vTaskB", 				/* 任务名 */
							512, 							/* 任务栈大小，单位 word，也就是 4 字节 */
							NULL, 						/* 任务参数 */
							4, 								/* 任务优先级*/
							&xHandleTaskB ); 	/* 任务句柄 */
	
	xTaskCreate(vTaskC, 					/* 任务函数 */
							"vTaskC", 				/* 任务名 */
							512, 							/* 任务栈大小，单位 word，也就是 4 字节 */
							NULL, 						/* 任务参数 */
							3, 								/* 任务优先级*/
							&xHandleTaskC ); 	/* 任务句柄 */
	
	
}


