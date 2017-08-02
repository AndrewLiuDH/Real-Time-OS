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

static QueueHandle_t xQueue1 = NULL;
static QueueHandle_t xQueue2 = NULL;

typedef struct Msg
{
	uint8_t ucMessageID;
	uint16_t usData[2];
	uint32_t ulData[2];
}MSG_T;
MSG_T g_tMsg; /* 定义一个结构体用于消息队列 */

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
	/* 创建 10 个 uint8_t 型消息队列 */
	xQueue1 = xQueueCreate(10, sizeof(uint8_t));
	if( xQueue1 == 0 )
	{
		/* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
		printf("Creat Queue Error");
	}
	/* 创建 10 个存储指针变量的消息队列，由于 CM3/CM4 内核是 32 位机，一个指针变量占用 4 个字节 */
	xQueue2 = xQueueCreate(10, sizeof(struct Msg *));
	if( xQueue2 == 0 )
	{
		/* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
		printf("Creat Queue Error");
	}
	
	xTaskCreate(KeyScan_Task, 					/* 任务函数 */
							"KeyScan_Task", 				/* 任务名 */
							512, 							/* 任务栈大小，单位 word，也就是 4 字节 */
							NULL, 						/* 任务参数 */
							3, 								/* 任务优先级*/
							&xKeyScanHandle); 	/* 任务句柄 */	

	xTaskCreate(FlagsProcess_Task, 					/* 任务函数 */
							"FlagsProcess_Task", 				/* 任务名 */
							512, 							/* 任务栈大小，单位 word，也就是 4 字节 */
							NULL, 						/* 任务参数 */
							4, 								/* 任务优先级*/
							&xFlagsProcessHandle ); 	/* 任务句柄 */	
}

static void KeyScan_Task(void *pvParameters)
{
	static uint8_t key,TaskBnum=0;
	MSG_T *ptMsg;
	uint8_t ucCount = 0;
	uint8_t ucKeyCode;
	/* 初始化结构体指针 */
	ptMsg = &g_tMsg;
	/* 初始化数组 */
	ptMsg->ucMessageID = 0;
	ptMsg->ulData[0] = 0;
	ptMsg->usData[0] = 0;

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
					ucCount++;
					/* 向消息队列发数据，如果消息队列满了，等待 10 个时钟节拍 */
					xQueueSend(xQueue1,(void *) &ucCount,(TickType_t)10);
					
				printf("Queue1 Free Space: %ld \r\n",uxQueueSpacesAvailable(xQueue1));
					break;
				case 2:
					printf("KEY 2 PRESSED  \r\n");
					ptMsg->ucMessageID++;
					ptMsg->ulData[0]++;;
					ptMsg->usData[0]++;

				/* 向消息队列发数据，如果消息队列满了，等待 10 个时钟节拍 */
					xQueueSend(xQueue2, 				/* 消息队列句柄 */
										(void *) &ptMsg, 	/* 发送结构体指针变量 ptMsg 的地址 */
										(TickType_t)10);
					printf("Queue2 Free Space: %ld \r\n",uxQueueSpacesAvailable(xQueue2));
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
	uint8_t num,ucQueueMsgValue;
	BaseType_t xResult;
//	MSG_T *ptMsg;
	
	while(1)
	{
		xResult = xQueueReceive(xQueue1, /* 消息队列句柄 */
													(void *)&ucQueueMsgValue, /* 这里获取的是结构体的地址 */
													(TickType_t)200);/* 设置阻塞时间 */
		if(xResult == pdPASS)
		{
			printf("接收到消息队列数据 ptMsg->ucMessageID = %d\r\n", ucQueueMsgValue);
			
//			/* 成功接收，并通过串口将数据打印出来 */
//			printf("接收到消息队列数据 ptMsg->ucMessageID = %d\r\n", ptMsg->ucMessageID);
//			printf("接收到消息队列数据 ptMsg->ulData[0] = %d\r\n", ptMsg->ulData[0]);
//			printf("接收到消息队列数据 ptMsg->usData[0] = %d\r\n", ptMsg->usData[0]);
		}else
		{
			/* 超时 */
			printf("没有接收到消息队列数据 \r\n");
		}
		vTaskDelay(500);
	}
}

