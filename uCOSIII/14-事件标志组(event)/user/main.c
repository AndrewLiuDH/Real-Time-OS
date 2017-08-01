#include "sys.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "usart.h"
#include "includes.h"

#define START_TASK_PRIO 3				//任务优先级
#define START_STK_SIZE 512			//任务堆栈大小
OS_TCB StartTaskTCB;						//任务控制块
CPU_STK START_TASK_STK[START_STK_SIZE];//任务堆栈
void start_task(void *p_arg);		//任务函数

#define Main_TASK_PRIO 4				//任务优先级
#define Main_STK_SIZE 128				//任务堆栈大小
OS_TCB Main_TaskTCB;							//任务控制块
CPU_STK Main_TASK_STK[Main_STK_SIZE];//任务堆栈
void Main_task(void *p_arg);		//任务函数

#define EventFlags_TASK_PRIO 5				//任务优先级
#define EventFlags_STK_SIZE 128				//任务堆栈大小
OS_TCB EventFlags_TaskTCB;							//任务控制块
CPU_STK EventFlags_TASK_STK[EventFlags_STK_SIZE];//任务堆栈
void EventFlags_task(void *p_arg);		//任务函数

////////////////////////事件标志组//////////////////////////////
#define KEY0_FLAG 0x01
#define KEY1_FLAG 0x02
#define KEYFLAGS_VALUE 0X00
OS_FLAG_GRP EventFlags; //定义一个事件标志组

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
	
	OS_CRITICAL_ENTER();	//进入临界区
	
	//创建一个事件标志组
	OSFlagCreate((OS_FLAG_GRP* )&EventFlags, //指向事件标志组
							(CPU_CHAR* )"Event Flags", //名字
							(OS_FLAGS )KEYFLAGS_VALUE, //初始值
							(OS_ERR* )&err); //错误码
	//创建LED0任务
	OSTaskCreate((OS_TCB 	* )&Main_TaskTCB,		
							(CPU_CHAR	* )"Main task", 		
							(OS_TASK_PTR )Main_task, 			
							(void		* )0,					
							(OS_PRIO	  )Main_TASK_PRIO,     
							(CPU_STK   * )&Main_TASK_STK[0],	
							(CPU_STK_SIZE)Main_STK_SIZE/10,	
							(CPU_STK_SIZE)Main_STK_SIZE,		
							(OS_MSG_QTY  )0,					
							(OS_TICK	  )0,					
							(void   	* )0,					
							(OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
							(OS_ERR 	* )&err);				
				 
	//创建LED1任务
	OSTaskCreate((OS_TCB 	* )&EventFlags_TaskTCB,		
							(CPU_CHAR	* )"EventFlags task", 		
							(OS_TASK_PTR )EventFlags_task, 			
							(void		* )0,
							(OS_PRIO	  )EventFlags_TASK_PRIO,     	
							(CPU_STK   * )&EventFlags_TASK_STK[0],	
							(CPU_STK_SIZE)EventFlags_STK_SIZE/10,	
							(CPU_STK_SIZE)EventFlags_STK_SIZE,		
							(OS_MSG_QTY  )0,					
							(OS_TICK	  )0,					
							(void   	* )0,				
							(OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
							(OS_ERR 	* )&err);

	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//挂起开始任务			 
	OS_CRITICAL_EXIT();	//进入临界区
}

//led0任务函数
void Main_task(void *p_arg)
{
	u8 key,num;
	OS_FLAGS flags_num;
	OS_ERR err;
	while(1)
	{
		key = KEY_Scan(0); //扫描按键
		if(key == 1)
		{
			//向事件标志组 EventFlags 发送标志
			flags_num=OSFlagPost((OS_FLAG_GRP*)&EventFlags,
													(OS_FLAGS )KEY0_FLAG,
													(OS_OPT )OS_OPT_POST_FLAG_SET,
													(OS_ERR* )&err);
			printf("事件标志组 EventFlags 的值:%d\r\n",flags_num);
		}else if(key == 2)
		{
			//向事件标志组 EventFlags 发送标志
			flags_num=OSFlagPost((OS_FLAG_GRP*)&EventFlags,
													(OS_FLAGS )KEY1_FLAG,
													(OS_OPT )OS_OPT_POST_FLAG_SET,
													(OS_ERR* )&err);
			printf("事件标志组 EventFlags 的值:%d\r\n",flags_num);
		}
		num++;
		if(num==50)
		{
			num=0;
			LED0 = ~LED0;
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err); //延时 10ms
	}
}

//led1任务函数
void EventFlags_task(void *p_arg)
{
	u8 num;
	OS_ERR err;
	while(1)
	{
		//等待事件标志组
		OSFlagPend((OS_FLAG_GRP*)&EventFlags,
							(OS_FLAGS)KEY0_FLAG|KEY1_FLAG,
							(OS_TICK )0,
							(OS_OPT  )OS_OPT_PEND_FLAG_SET_ALL+OS_OPT_PEND_FLAG_CONSUME,
							(CPU_TS* )0,
							(OS_ERR* )&err);
		num++;
		LED1 = ~LED1;
		printf("事件标志组 EventFlags 的值:%d\r\n",EventFlags.Flags);
	}
}

