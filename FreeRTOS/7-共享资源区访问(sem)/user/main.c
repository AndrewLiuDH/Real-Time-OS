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
static TaskHandle_t xTASKBHandle = NULL;
static TaskHandle_t xTASKCHandle = NULL;

static void TASKB_task(void *pvParameters);
static void TASKC_task(void *pvParameters);

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
QueueHandle_t xSemaphore = NULL;

static void AppTaskCreate (void)
{
	xSemaphore = xSemaphoreCreateCounting(1, 0);/* 创建计数信号量 */
	
	xTaskCreate(TASKB_task, 					/* 任务函数 */
						"TASKB_task", 				/* 任务名 */
						512, 							/* 任务栈大小，单位 word，也就是 4 字节 */
						NULL, 						/* 任务参数 */
						3, 								/* 任务优先级*/
						&xTASKBHandle ); 	/* 任务句柄 */	

	
	xTaskCreate(TASKC_task, 					/* 任务函数 */
							"TASKC_task", 				/* 任务名 */
							512, 							/* 任务栈大小，单位 word，也就是 4 字节 */
							NULL, 						/* 任务参数 */
							4, 								/* 任务优先级*/
							&xTASKCHandle ); 	/* 任务句柄 */	
}

u8 share_resource[30]; //共享资源区
//led0任务函数
void TASKB_task(void *p_arg)
{
	u8 task1_str[]="First task Running!";
	
	while(1)
	{
		xSemaphoreTake(xSemaphore,portMAX_DELAY);
		printf("\r\n 任务 1:\r\n");
		memcpy(share_resource,task1_str,sizeof(task1_str)); //向共享资源区拷贝数据 (1)
//		vTaskDelay(200);//会导致任务切换
		printf("%s\r\n",share_resource); //串口输出共享资源区数据
		LED1 = ~LED1;
		xSemaphoreGive(xSemaphore);
		vTaskDelay(800); //延时 1s
	}
}

//led1任务函数
void TASKC_task(void *p_arg)
{
	u8 i=0;
	u8 task2_str[]="Second task Running!";
	
	while(1)
	{
		xSemaphoreTake(xSemaphore,portMAX_DELAY);
		printf("\r\n 任务 2:\r\n");
		memcpy(share_resource,task2_str,sizeof(task2_str));//向共享资源区拷贝数据
//		vTaskDelay(200);//会导致任务切换
		printf("%s\r\n",share_resource); //串口输出共享资源区数据
		LED2 = ~LED2;
		xSemaphoreGive(xSemaphore);
		vTaskDelay(800); //延时 1s
	}
}
