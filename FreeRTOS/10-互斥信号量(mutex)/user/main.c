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
static TaskHandle_t xTASKAHandle = NULL;
static TaskHandle_t xTASKBHandle = NULL;
static TaskHandle_t xTASKCHandle = NULL;

static void TASKA_task(void *pvParameters);
static void TASKB_task(void *pvParameters);
static void TASKC_task(void *pvParameters);


/*摘自官方手册
Mutexes include a priority inheritance mechanism, binary semaphores do not. This makes
binary semaphores the better choice for implementing synchronization (between tasks or
between tasks and an interrupt), and mutexes the better choice for implementing simple
mutual exclusion
*/

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
SemaphoreHandle_t xMutex = NULL;

static void AppTaskCreate (void)
{
	xMutex = xSemaphoreCreateMutex();/* 创建计数信号量 */
	
	xTaskCreate(TASKA_task, 					/* 任务函数 */
							"TASKA_task", 				/* 任务名 */
							512, 							/* 任务栈大小，单位 word，也就是 4 字节 */
							NULL, 						/* 任务参数 */
							3, 								/* 任务优先级*/
							&xTASKAHandle ); 	/* 任务句柄 */	

	xTaskCreate(TASKB_task, 					/* 任务函数 */
							"TASKB_task", 				/* 任务名 */
							512, 							/* 任务栈大小，单位 word，也就是 4 字节 */
							NULL, 						/* 任务参数 */
							4, 								/* 任务优先级*/
							&xTASKBHandle ); 	/* 任务句柄 */	
	
	xTaskCreate(TASKC_task, 					/* 任务函数 */
							"TASKC_task", 				/* 任务名 */
							512, 							/* 任务栈大小，单位 word，也就是 4 字节 */
							NULL, 						/* 任务参数 */
							5, 								/* 任务优先级*/
							&xTASKCHandle ); 	/* 任务句柄 */	
}

UBaseType_t uxSemNum;
//led0任务函数
void TASKA_task(void *p_arg)//低优先级
{
	static u32 times;

	while(1)
	{
		xSemaphoreTake(xMutex,portMAX_DELAY);
		LED0=!LED0;
		printf("Low_task Now RUNNING  ★ \r\n"); 
		vTaskDelay(5000);
		xSemaphoreGive(xMutex);
		vTaskDelay(1000);
	}
}

//led1任务函数
void TASKB_task(void *p_arg)//中优先级
{	
	while(1)
	{
		printf("Middle_task Now RUNNING \r\n"); 
		LED1 = ~LED1;
		vTaskDelay(1000);
	}
}


//led1任务函数
void TASKC_task(void *p_arg)//高优先级
{
//	xSemaphoreGive(xSemaphore);

	while(1)
	{
		uxSemNum = uxSemaphoreGetCount(xMutex);
		xSemaphoreTake(xMutex,portMAX_DELAY);
		printf("High_task Now RUNNING  ★ \r\n");
		LED2 = ~LED2;
		xSemaphoreGive(xMutex);
		vTaskDelay(500);

	}
}
