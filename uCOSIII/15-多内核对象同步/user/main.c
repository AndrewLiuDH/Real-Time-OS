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
OS_TCB MainTaskTCB;							//任务控制块
CPU_STK Main_TASK_STK[Main_STK_SIZE];//任务堆栈
void Main_task(void *p_arg);		//任务函数

#define Multi_TASK_PRIO 5				//任务优先级
#define Multi_STK_SIZE 128				//任务堆栈大小
OS_TCB 	Multi_TaskTCB;							//任务控制块
CPU_STK Multi_TASK_STK[Multi_STK_SIZE];//任务堆栈
void Multi_task(void *p_arg);		//任务函数

////////////////////////消息队列//////////////////////////////
#define KEYMSG_Q_NUM 1 //按键消息队列的数量
OS_Q Multi_Q; //定义一个消息队列，用于按键消息传递，模拟消息邮箱

//////////////////////Sem/////////////////////////////////////
OS_SEM Multi_Sem1,Multi_Sem2;

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

	OSSemCreate ((OS_SEM   *)&Multi_Sem1,
							 (CPU_CHAR *)"Multi_Sem1",
							 (OS_SEM_CTR)1,
							 (OS_ERR  *)&err);

	OSSemCreate ((OS_SEM   *)&Multi_Sem2,
							 (CPU_CHAR *)"Multi_Sem2",
							 (OS_SEM_CTR)1,
							 (OS_ERR  *)&err);
							
	OSQCreate((OS_Q *)&Multi_Q,
						(CPU_CHAR *)"Multi_Q",
						(OS_MSG_QTY)KEYMSG_Q_NUM,
						(OS_ERR *)&err);

	//创建LED0任务
	OSTaskCreate((OS_TCB 	* )&MainTaskTCB,		
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
							
	//创建key处理任务
	OSTaskCreate((OS_TCB 	* )&Multi_TaskTCB,		
							(CPU_CHAR	* )"Multi_task", 		
							(OS_TASK_PTR )Multi_task, 			
							(void		* )0,					
							(OS_PRIO	  )Multi_TASK_PRIO,     
							(CPU_STK   * )&Multi_TASK_STK[0],	
							(CPU_STK_SIZE)Multi_STK_SIZE/10,	
							(CPU_STK_SIZE)Multi_STK_SIZE,		
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
	u8 key;
	OS_ERR err;
	u8 num;
	u8 *pbuf;
	static u8 msg_num;
	pbuf=malloc(10);
	
	while(1)
	{
		key = KEY_Scan(0); //扫描按键
		switch(key)
		{
			case 1://发送信号量1
			OSSemPost((OS_SEM *)&Multi_Sem1,
								(OS_OPT )OS_OPT_POST_1,
								(OS_ERR*) &err);
			printf("发送信号量1 %d\r\n",Multi_Sem1.Ctr);
			break;
			case 2://发送信号量2
			OSSemPost((OS_SEM *)&Multi_Sem2,
								(OS_OPT )OS_OPT_POST_1,
								(OS_ERR*) &err);
			printf("发送信号量2 %d\r\n",Multi_Sem2.Ctr);
			break;
			case 3://发送消息
			msg_num++;
			sprintf((char*)pbuf,"ALIENTEK %d",msg_num);
			
			OSQPost ((OS_Q *)&Multi_Q,
               (void *)pbuf,
               (OS_MSG_SIZE)sizeof(pbuf),
               (OS_OPT  )OS_OPT_POST_FIFO,
               (OS_ERR *)&err);
			printf("发送消息 %s\r\n",pbuf);
			break;
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

//按键处理任务的任务函数
void Multi_task(void *p_arg)
{
	u8 num;
	u8 *key;
	OS_MSG_SIZE size;
	OS_ERR err;
	
	OS_OBJ_QTY index;
	OS_PEND_DATA pend_multi_tbl[3];
	
	pend_multi_tbl[0].PendObjPtr=(OS_PEND_OBJ*)&Multi_Sem1;
	pend_multi_tbl[1].PendObjPtr=(OS_PEND_OBJ*)&Multi_Sem2;
	pend_multi_tbl[2].PendObjPtr=(OS_PEND_OBJ*)&Multi_Q;

	while(1)
	{
		//请求消息 KEY_Msg
		index=OSPendMulti((OS_PEND_DATA* )pend_multi_tbl,
										(OS_OBJ_QTY )3,
										(OS_TICK )0,
										(OS_OPT )OS_OPT_PEND_BLOCKING,
										(OS_ERR* )&err);
		
		printf("Now Have %d Core Object \r\n",index);
		LED1 = ~LED1;
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_PERIODIC,&err); //延时 1s
	}
}
