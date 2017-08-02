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
static void vTaskTaskUserIF(void *pvParameters);
static void vTaskLED(void *pvParameters);
static void vTaskLED(void *pvParameters);
static void StackOverflowTest(void);

int main(void)
{	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	delay_init(168);
	LED_Init();
	KEY_init();
	USART1_init();

	/* 创建任务 */
	AppTaskCreate();
	
	/* 启动调度，开始执行任务 */
	vTaskStartScheduler();
}

/* 任务句柄 */
static TaskHandle_t xHandleTaskUserIF = NULL;
static TaskHandle_t xHandleTaskLED = NULL;
static TaskHandle_t xHandleTaskMsgPro = NULL;
static TaskHandle_t xHandleTaskStart = NULL;
static void vTaskMsgPro(void *pvParameters);
static void vTaskStart(void *pvParameters);


/*
*********************************************************************************************************
* 函 数 名: AppTaskCreate
* 功能说明: 创建应用任务
* 形 参：无
* 返 回 值: 无
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
	xTaskCreate( vTaskTaskUserIF, /* 任务函数 */
							"vTaskUserIF", /* 任务名 */
							512, /* 任务栈大小，单位 word，也就是 4 字节 */
							NULL, /* 任务参数 */
							1, /* 任务优先级*/
							&xHandleTaskUserIF ); /* 任务句柄 */
	
	xTaskCreate( vTaskLED, /* 任务函数 */
							"vTaskLED", /* 任务名 */
							512, /* 任务栈大小，单位 word，也就是 4 字节 */
							NULL, /* 任务参数 */
							2, /* 任务优先级*/
							&xHandleTaskLED ); /* 任务句柄 */
	
	xTaskCreate( vTaskMsgPro, /* 任务函数 */
							"vTaskMsgPro", /* 任务名 */
							512, /* 任务栈大小，单位 word，也就是 4 字节 */
							NULL, /* 任务参数 */
							3, /* 任务优先级*/
							&xHandleTaskMsgPro ); /* 任务句柄 */
	
	xTaskCreate( vTaskStart, /* 任务函数 */
							"vTaskStart", /* 任务名 */
							512, /* 任务栈大小，单位 word，也就是 4 字节 */
							NULL, /* 任务参数 */
							4, /* 任务优先级*/
							&xHandleTaskStart ); /* 任务句柄 */
}

/*
*********************************************************************************************************
* 函 数 名: vTaskTaskUserIF
* 功能说明: 接口消息处理。
* 形 参: pvParameters 是在创建该任务时传递的形参
* 返 回 值: 无
* 优 先 级: 1 (数值越小优先级越低，这个跟 uCOS 相反)
*********************************************************************************************************
*/
static void vTaskTaskUserIF(void *pvParameters)
{
	uint8_t ucKeyCode;

	while(1)
	{
		ucKeyCode = KEY_Scan(0);
		if (ucKeyCode != 0)
		{
			switch (ucKeyCode)
			{
				/* K1 键按下 打印任务执行情况 */
				case 1:
							printf("=================================================\r\n");
							printf("任务名 任务状态 优先级 剩余栈 任务序号\r\n");
							
							printf("\r\n 任务名 运行计数 使用率\r\n");
				break;
				/* K2 键按下 删除任务 vTaskLED */
				case 2:
							printf("K2 键按下，模拟任务栈溢出检测\r\n");
							StackOverflowTest();
				break;
				/* 其他的键值不处理 */
				default:
				break;
			}
		}
		vTaskDelay(20);
	}
}
/*
*********************************************************************************************************
* 函 数 名: vTaskLED
* 功能说明: LED 闪烁
* 形 参: pvParameters 是在创建该任务时传递的形参
* 返 回 值: 无
* 优 先 级: 2
*********************************************************************************************************
*/
static void vTaskLED(void *pvParameters)
{
	while(1)
	{
		LED2=!LED2;
		vTaskDelay(200);
	}
}
/*
*********************************************************************************************************
* 函 数 名: vTaskMsgPro
* 功能说明: 消息处理，这里是用作 LED 闪烁
* 形 参: pvParameters 是在创建该任务时传递的形参
* 返 回 值: 无
* 优 先 级: 3
*********************************************************************************************************
*/
static void vTaskMsgPro(void *pvParameters)
{
	while(1)
	{
		LED3=!LED3;
		vTaskDelay(300);
	}
}
/*
*********************************************************************************************************
* 函 数 名: vTaskStart
* 功能说明: 启动任务，也就是最高优先级任务，这里用作按键扫描。
* 形 参: pvParameters 是在创建该任务时传递的形参
* 返 回 值: 无
* 优 先 级: 4
*********************************************************************************************************
*/
static void vTaskStart(void *pvParameters)
{
	while(1)
	{
		/* 按键扫描 */
//		bsp_KeyScan();
		vTaskDelay(100);
	}
}

static void StackOverflowTest(void)
{
	int16_t i;
	uint8_t buf[2048];
	(void)buf; /* 防止警告 */
	/*
		1. 为了能够模拟任务栈溢出，并触发任务栈溢出函数，这里强烈建议使用数组的时候逆着赋值。
		因为对于 M3 和 M4 内核的 MCU，堆栈生长方向是向下生长的满栈。即高地址是 buf[2047], 低地址
		是 buf[0]。如果任务栈溢出了，也是从高地址 buf[2047]到 buf[0]的某个地址开始溢出。
		因此，如果用户直接修改的是 buf[0]开始的数据且这些溢出部分的数据比较重要，会直接导致
		进入到硬件异常。
		2. 栈溢出检测是在任务切换的时候执行的，我们这里加个延迟函数，防止修改了重要的数据导致直接
		进入硬件异常。
		3. 任务 vTaskTaskUserIF 的栈空间大小是 2048 字节，在此任务的入口已经申请了栈空间大小
		------uint8_t ucKeyCode;
		------uint8_t pcWriteBuffer[500];
		这里再申请如下这么大的栈空间
		-------int16_t i;
		-------uint8_t buf[2048];
		必定溢出。
	*/
	for(i = 2047; i >= 0; i--)
	{
		buf[i] = 0x55;
		vTaskDelay(1);
	}
}


