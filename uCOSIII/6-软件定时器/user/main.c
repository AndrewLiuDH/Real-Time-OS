#include "sys.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "includes.h"

#define START_TASK_PRIO 3				//任务优先级
#define START_STK_SIZE 512			//任务堆栈大小
OS_TCB StartTaskTCB;						//任务控制块
CPU_STK START_TASK_STK[START_STK_SIZE];//任务堆栈
void start_task(void *p_arg);		//任务函数

#define TASKB_TASK_PRIO 4				//任务优先级
#define TASKB_STK_SIZE 128				//任务堆栈大小
OS_TCB TASKBTaskTCB;							//任务控制块
CPU_STK TASKB_TASK_STK[TASKB_STK_SIZE];//任务堆栈
void TASKB_task(void *p_arg);		//任务函数

void tmr1_callback(void *p_tmr, void *p_arg);
void tmr2_callback(void *p_tmr, void *p_arg);

int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();

	delay_init(168);
	USART1_Init();
	LED_Init();
	KEY_init();

	OSInit(&err);		//初始化UCOSIII
	OS_CRITICAL_ENTER();//进入临界区
	//创建开始任务
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,					//任务控制块
				 (CPU_CHAR	* )"start task", 							//任务名字
                 (OS_TASK_PTR )start_task, 				//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,									//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,										//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,										//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);								//存放该函数错误时的返回值
	OS_CRITICAL_EXIT();	//退出临界区	 
	OSStart(&err);  //开启UCOSIII
	
	while(1);
}

OS_TMR tmr1; //定时器 1
OS_TMR tmr2; //定时器 2

//开始任务函数
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	
	OSTmrCreate((OS_TMR* )&tmr1, //定时器 1 (1)
							(CPU_CHAR* )"tmr1", //定时器名字
							(OS_TICK )20, //20*10=200ms
							(OS_TICK )100, //100*10=1000ms
							(OS_OPT )OS_OPT_TMR_PERIODIC, //周期模式
							(OS_TMR_CALLBACK_PTR)tmr1_callback, //定时器 1 回调函数
							(void* )0, //参数为 0
							(OS_ERR* )&err); //返回的错误码
	//创建定时器 2
	OSTmrCreate((OS_TMR* )&tmr2, //(2)
							(CPU_CHAR* )"tmr2",
							(OS_TICK )200, //200*10=2000ms
							(OS_TICK )0,
							(OS_OPT )OS_OPT_TMR_ONE_SHOT, //单次定时器
							(OS_TMR_CALLBACK_PTR)tmr2_callback,//定时器 2 回调函数
							(void* )0,
							(OS_ERR* )&err);
	
	OS_CRITICAL_ENTER();	//进入临界区
	//创建LED0任务
	OSTaskCreate((OS_TCB 	* )&TASKBTaskTCB,		
							(CPU_CHAR	* )"TASKB task", 		
							(OS_TASK_PTR )TASKB_task, 			
							(void		* )0,					
							(OS_PRIO	  )TASKB_TASK_PRIO,     
							(CPU_STK   * )&TASKB_TASK_STK[0],	
							(CPU_STK_SIZE)TASKB_STK_SIZE/10,	
							(CPU_STK_SIZE)TASKB_STK_SIZE,		
							(OS_MSG_QTY  )0,					
							(OS_TICK	  )0,					
							(void   	* )0,					
							(OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
							(OS_ERR 	* )&err);				

	OSTaskDel((OS_TCB*)&StartTaskTCB,&err);		//删除开始任务			 
	OS_CRITICAL_EXIT();	//进入临界区
}

//led0任务函数
void TASKB_task(void *p_arg)
{
	OS_ERR err;
	u8 key,num;

	static uint8_t TaskBRunTimes=0; 
	
	while(1)
	{
		key = KEY_Scan(0);
		if(key!=0)
		{
			switch(key)
			{
			case 1: //当 key_up 按下的话打开定时器 1
						OSTmrStart(&tmr1,&err); //开启定时器 1
						printf("开启定时器 1\r\n");
						break;
			case 2: //当 key0 按下的话打开定时器 2
						OSTmrStart(&tmr2,&err); //开启定时器 1
						printf("开启定时器 2\r\n");
						break;
			case 3: //当 key0 按下的话打开定时器 2
						if(tmr1.State!=OS_TMR_STATE_STOPPED)
						{
							OSTmrStop(&tmr1,OS_OPT_TMR_NONE,0,&err); //关闭定时器 1
							printf("关闭定时器 1\r\n");
						}
						
						if(tmr2.State==OS_TMR_STATE_COMPLETED)
						{
							OSTmrStop(&tmr2,OS_OPT_TMR_NONE,0,&err); //关闭定时器 1
							printf("关闭定时器 2\r\n");
						}break;
			default:
						printf("现在按下了无用按键：%d\r\n",key);
			}
		}
		
		num++;
		if(num==50) //每 500msLED0 闪烁一次
		{
			num = 0;
			LED0 = ~LED0;
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err); //延时 10ms		
	}
}

//tmr1回调函数
void tmr1_callback(void *p_tmr, void *p_arg)
{
	OS_ERR err;
	static uint8_t Tmr1RunTimes=0; 
	
//	while(1)//不能while(1)
//	{
		LED2=!LED2;
		Tmr1RunTimes++;
		printf("Tmr1 has Running for %d Times!\r\n",Tmr1RunTimes);
//	}
}

//tmr2回调函数
void tmr2_callback(void *p_tmr, void *p_arg)
{
	OS_ERR err;
	static uint8_t Tmr2RunTimes=0; 
	
	LED3=!LED3;
	Tmr2RunTimes++;
	printf("Tmr2 has Running for %d Times!\r\n",Tmr2RunTimes);
}


