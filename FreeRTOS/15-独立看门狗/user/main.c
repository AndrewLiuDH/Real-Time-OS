#include "sys.h"
#include "led.h"
#include "delay.h"
#include "iwdg.h"
#include "usart.h"
#include "event_groups.h"

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
static TaskHandle_t xHandleTaskA = NULL;
static TaskHandle_t xHandleTaskB = NULL;
static TaskHandle_t xHandleTaskC = NULL;
static TaskHandle_t xHandleTaskD = NULL;

EventGroupHandle_t xCreatedEventGroup;


static void vTaskA(void *pvParameters)
{
	EventBits_t uxBits;
	const TickType_t xTicksToWait = 100 / portTICK_PERIOD_MS; /* 最大延迟 100ms */
	/*
	开始执行启动任务主函数前使能独立看门狗。
	设置 LSI 是 128 分频，下面函数参数范围 0-0xFFF，分别代表最小值 3.2ms 和最大值 13107.2ms
	下面设置的是 10s，如果 10s 内没有喂狗，系统复位。
	*/
	IWDG_Init(4,5000);
	/* 打印系统开机状态，方便查看系统是否复位 */
	printf("=====================================================\r\n");
	printf("=系统开机执行\r\n");
	printf("=====================================================\r\n");
	while(1)
	{
		/* 等待所有任务发来事件标志 */
		uxBits = xEventGroupWaitBits(xCreatedEventGroup, /* 事件标志组句柄 */
																0x07, /* 等待 TASK_BIT_ALL 被设置 */
																pdTRUE, /* 退出前 TASK_BIT_ALL 被清除，这里是 TASK_BIT_ALL
																都被设置才表示“退出” */
																pdTRUE, /* 设置为 pdTRUE 表示等待 TASK_BIT_ALL 都被设置*/
																xTicksToWait); /* 等待延迟时间 */
		if((uxBits & 0x07) == 0x07)
		{
			IWDG_Feed();
			printf("五个用户任务都正常运行\r\n");
		}
		else
		{
			/* 基本是每 xTicksToWait 进来一次 */
			/* 通过变量 uxBits 简单的可以在此处检测那个任务长期没有发来运行标志 */
		}
	}
}

static void vTaskB(void *pvParameters)
{
	static uint8_t TaskBnum=0;

	while(1)
	{
		TaskBnum++;
		printf("任务B : %d \r\n",TaskBnum);
		LED2=!LED2;
		/* 发送事件标志，表示任务正常运行 */
		xEventGroupSetBits(xCreatedEventGroup, 0x01);
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
		LED3=!LED3;
		/* 发送事件标志，表示任务正常运行 */
		xEventGroupSetBits(xCreatedEventGroup, 0x02);
		vTaskDelay(500);//退出调度500个ticks
	}
}

static void vTaskD(void *pvParameters)
{
	static uint8_t TaskDnum=0;
	
	while(1)
	{
		TaskDnum++;
		printf("任务D : %d \r\n",TaskDnum);
		LED1=!LED1;
		/* 发送事件标志，表示任务正常运行 */
		xEventGroupSetBits(xCreatedEventGroup, 0x04);
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
	/* 创建事件标志组 */
	xCreatedEventGroup = xEventGroupCreate();
	
	xTaskCreate(vTaskA, 					/* 任务函数 */
							"vTaskA", 				/* 任务名 */
							512, 							/* 任务栈大小，单位 word，也就是 4 字节 */
							NULL, 						/* 任务参数 */
							1, 								/* 任务优先级*/
							&xHandleTaskA ); 	/* 任务句柄 */
	
	xTaskCreate(vTaskB, 					/* 任务函数 */
							"vTaskB", 				/* 任务名 */
							512, 							/* 任务栈大小，单位 word，也就是 4 字节 */
							NULL, 						/* 任务参数 */
							2, 								/* 任务优先级*/
							&xHandleTaskB ); 	/* 任务句柄 */
	
	xTaskCreate(vTaskC, 					/* 任务函数 */
							"vTaskC", 				/* 任务名 */
							512, 							/* 任务栈大小，单位 word，也就是 4 字节 */
							NULL, 						/* 任务参数 */
							3, 								/* 任务优先级*/
							&xHandleTaskC ); 	/* 任务句柄 */
	
	xTaskCreate(vTaskD, 					/* 任务函数 */
							"vTaskD", 				/* 任务名 */
							512, 							/* 任务栈大小，单位 word，也就是 4 字节 */
							NULL, 						/* 任务参数 */
							4, 								/* 任务优先级*/
							&xHandleTaskD ); 	/* 任务句柄 */
	
}


