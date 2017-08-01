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

#define KeyProcess_TASK_PRIO 5				//任务优先级
#define KeyProcess_STK_SIZE 128				//任务堆栈大小
OS_TCB 	Keyprocess_TaskTCB;							//任务控制块
CPU_STK KeyProcess_TASK_STK[KeyProcess_STK_SIZE];//任务堆栈
void KeyProcess_task(void *p_arg);		//任务函数

#define MSGDIS_TASK_PRIO 6			//任务优先级
#define MSGDIS_STK_SIZE 128				//任务堆栈大小
OS_TCB 	Msgdis_TaskTCB;							//任务控制块
CPU_STK MSGDIS_TASK_STK[MSGDIS_STK_SIZE];//任务堆栈
void Msgdis_task(void *p_arg);		//任务函数



////////////////////////消息队列//////////////////////////////
#define KEYMSG_Q_NUM 1 //按键消息队列的数量
#define DATAMSG_Q_NUM 4 //发送数据的消息队列的数量
OS_Q KEY_Msg; //定义一个消息队列，用于按键消息传递，模拟消息邮箱
OS_Q DATA_Msg; //定义一个消息队列，用于发送数据
////////////////////////定时器////////////////////////////////
u8 tmr1sta=0; //标记定时器的工作状态
OS_TMR tmr1; //定义一个定时器
void tmr1_callback(void *p_tmr,void *p_arg);

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

	OSTmrCreate((OS_TMR*)&tmr1,
							(CPU_CHAR*)"tmr1",
							(OS_TICK)0,//delay 0ms
							(OS_TICK)50,//50*10=500ms
							(OS_OPT)OS_OPT_TMR_PERIODIC,//周期模式
							(OS_TMR_CALLBACK_PTR)tmr1_callback,
							(void *)0,
							(OS_ERR *)&err);
							
	OSQCreate((OS_Q *)&KEY_Msg,
						(CPU_CHAR *)"KEY_Msg",
						(OS_MSG_QTY)KEYMSG_Q_NUM,
						(OS_ERR *)&err);
							
	OSQCreate((OS_Q *)&DATA_Msg,
						(CPU_CHAR *)"DATA_Msg",
						(OS_MSG_QTY)DATAMSG_Q_NUM,
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
	OSTaskCreate((OS_TCB 	* )&Keyprocess_TaskTCB,		
							(CPU_CHAR	* )"Keyprocess task", 		
							(OS_TASK_PTR )KeyProcess_task, 			
							(void		* )0,					
							(OS_PRIO	  )KeyProcess_TASK_PRIO,     
							(CPU_STK   * )&KeyProcess_TASK_STK[0],	
							(CPU_STK_SIZE)KeyProcess_STK_SIZE/10,	
							(CPU_STK_SIZE)KeyProcess_STK_SIZE,		
							(OS_MSG_QTY  )0,					
							(OS_TICK	  )0,  					
							(void   	* )0,					
							(OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
							(OS_ERR 	* )&err);			
								 
	//创建MSGDIS任务
	OSTaskCreate((OS_TCB 	* )&Msgdis_TaskTCB,		
							(CPU_CHAR	* )"Msgdis task", 		
							(OS_TASK_PTR )Msgdis_task,
							(void		* )0,					
							(OS_PRIO	  )MSGDIS_TASK_PRIO,     
							(CPU_STK   * )&MSGDIS_TASK_STK[0],	
							(CPU_STK_SIZE)MSGDIS_STK_SIZE/10,	
							(CPU_STK_SIZE)MSGDIS_STK_SIZE,		
							(OS_MSG_QTY  )0,					
							(OS_TICK	  )0,  					
							(void   	* )0,					
							(OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
							(OS_ERR 	* )&err);	
				 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//挂起开始任务			 
	OS_CRITICAL_EXIT();	//进入临界区
}

//查询 DATA_Msg 消息队列中的总队列数量和剩余队列数量
void check_msg_queue(u8 *p)
{
	u8 msgq_remain_size; //消息队列剩余大小
	msgq_remain_size = DATA_Msg.MsgQ.NbrEntriesSize-DATA_Msg.MsgQ.NbrEntries;
	p = malloc(20); //申请内存
	//显示 DATA_Msg 消息队列总的大小
	sprintf((char*)p,"Total Size:%d",DATA_Msg.MsgQ.NbrEntriesSize);
	printf("%s ",p);
	sprintf((char*)p,"Remain Size:%d",msgq_remain_size); //显示 DATA_Msg 剩余大小
	printf("%s \r\n",p);
	free(p); //释放内存
}

//led0任务函数
void Main_task(void *p_arg)
{
	u8 key,num;
	OS_ERR err;
	u8 *p;
	while(1)
	{
		key = KEY_Scan(0); //扫描按键
		if(key)
		{
			//发送消息
			OSQPost((OS_Q* )&KEY_Msg,
							(void* )&key,
							(OS_MSG_SIZE )1,
							(OS_OPT )OS_OPT_POST_FIFO,
							(OS_ERR*) &err);
		}
		num++;
		if(num%50==0) check_msg_queue(p);//检查 DATA_Msg 消息队列的容量
		if(num==50)
		{
			num=0;
			LED0 = ~LED0;
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err); //延时 10ms
	}
}

//定时器 1 的回调函数
void tmr1_callback(void *p_tmr,void *p_arg)
{
	u8 *pbuf;
	static u8 msg_num;
	OS_ERR err;
	pbuf = malloc(10); //申请 10 个字节
	if(pbuf) //申请内存成功
	{
		msg_num++;
		sprintf((char*)pbuf,"ALIENTEK %d",msg_num);
		//发送消息
		OSQPost((OS_Q* )&DATA_Msg,
						(void* )pbuf,
						(OS_MSG_SIZE )10,
						(OS_OPT )OS_OPT_POST_FIFO,
						(OS_ERR* )&err);
		if(err != OS_ERR_NONE)
		{
			free(pbuf); //释放内存
			OSTmrStop(&tmr1,OS_OPT_TMR_NONE,0,&err); //停止定时器 1
			tmr1sta = !tmr1sta;
			printf("TMR1 STOP! \r\n");
		}
	}
}

//按键处理任务的任务函数
void KeyProcess_task(void *p_arg)
{
	u8 num;
	u8 *key;
	OS_MSG_SIZE size;
	OS_ERR err;
	while(1)
	{
		//请求消息 KEY_Msg
		key=OSQPend((OS_Q* )&KEY_Msg,
								(OS_TICK )0,
								(OS_OPT )OS_OPT_PEND_BLOCKING,
								(OS_MSG_SIZE* )&size,
								(CPU_TS* )0,
								(OS_ERR* )&err);
		
		switch(*key)
		{
			case 1: //KEY_UP 控制 LED1
			LED1 = ~LED1;
			break;
			case 2: //KEY2 控制蜂鸣器
			LED2 = ~LED2;
			break;
			case 3: //KEY0 刷新 LCD 背景
			tmr1sta = !tmr1sta;
			if(tmr1sta)
			{
				OSTmrStart(&tmr1,&err);
				printf("TMR1 START! \r\n");
			}else
			{
				OSTmrStop(&tmr1,OS_OPT_TMR_NONE,0,&err); //停止定时器 1
				printf("TMR1 STOP! \r\n");
			}
			break;
		}
	}
}
//显示消息队列中的消息
void Msgdis_task(void *p_arg)
{
	u8 *p;
	OS_MSG_SIZE size;
	OS_ERR err;
	while(1)
	{
		//请求消息
		p=OSQPend((OS_Q* )&DATA_Msg,
							(OS_TICK )0,
							(OS_OPT )OS_OPT_PEND_BLOCKING,
							(OS_MSG_SIZE* )&size,
							(CPU_TS* )0,
							(OS_ERR* )&err);
		printf("%s \r\n",p);
		free(p); //释放内存
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_PERIODIC,&err); //延时 1s
	}
}
