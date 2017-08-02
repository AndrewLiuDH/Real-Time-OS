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
QueueHandle_t xSemaphore = NULL;

static void AppTaskCreate (void)
{
	xSemaphore = xSemaphoreCreateCounting(10, 1);/* 创建计数信号量 */
//	xSemaphore = xSemaphoreCreateBinary();/* 创建二值信号量 ，初始值为0，需要先give*/

	
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
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	
	while(1)
	{
		xSemaphoreTake(xSemaphore,portMAX_DELAY);
		LED0=!LED0;
		vTaskDelayUntil(&xLastWakeTime,5000);
		xSemaphoreGive(xSemaphore);
		printf("Low_task Now RUNNING  ★ \r\n"); 
		vTaskDelay(1000); //延时 1s
	}
}

//led1任务函数
void TASKB_task(void *p_arg)//中优先级
{	
	while(1)
	{
		printf("Middle_task Now RUNNING \r\n"); 
		LED1 = ~LED1;
		vTaskDelay(1000); //延时 1s
	}
}


//led1任务函数
void TASKC_task(void *p_arg)//高优先级
{
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

	while(1)
	{
		uxSemNum = uxSemaphoreGetCount(xSemaphore);
		xSemaphoreTake(xSemaphore,portMAX_DELAY);
		printf("High_task Now RUNNING  ★ \r\n");
		LED2 = ~LED2;
		xSemaphoreGive(xSemaphore);
		vTaskDelayUntil(&xLastWakeTime,500);
	}
}
