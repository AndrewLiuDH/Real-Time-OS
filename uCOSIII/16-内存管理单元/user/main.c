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

#define MMU_TASK_PRIO 5				//任务优先级
#define MMU_STK_SIZE 128				//任务堆栈大小
OS_TCB 	MMU_TaskTCB;							//任务控制块
CPU_STK MMU_TASK_STK[MMU_STK_SIZE];//任务堆栈
void MMU_task(void *p_arg);		//任务函数

OS_MEM INTERNAL_MEM; //定义一个存储区
#define INTERNAL_MEM_NUM 5 //存储区中存储块数量
#define INTERNAL_MEMBLOCK_SIZE 100 //每个存储块大小由于一个指针变量占用 4
//字节所以块的大小一定要为 4 的倍数而且
//必须大于一个指针变量(4 字节)占用的空间,
//否则的话存储块创建不成功
//存储区的内存池，使用内部 RAM
__align(4) CPU_INT08U Internal_RamMemp[INTERNAL_MEM_NUM] [INTERNAL_MEMBLOCK_SIZE];

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
	
	//创建一个存储分区
	OSMemCreate((OS_MEM* )&INTERNAL_MEM, //存储区控制块 (1)
							(CPU_CHAR* )"Internal Mem", //存储区名字
							(void* )&Internal_RamMemp[0][0], //存储空间基地址
							(OS_MEM_QTY )INTERNAL_MEM_NUM, //存储块数量
							(OS_MEM_SIZE )INTERNAL_MEMBLOCK_SIZE,//存储块大小
							(OS_ERR* )&err); //错误码

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
	OSTaskCreate((OS_TCB 	* )&MMU_TaskTCB,		
							(CPU_CHAR	* )"MMU_task", 		
							(OS_TASK_PTR )MMU_task, 			
							(void		* )0,					
							(OS_PRIO	  )MMU_TASK_PRIO,     
							(CPU_STK   * )&MMU_TASK_STK[0],	
							(CPU_STK_SIZE)MMU_STK_SIZE/10,	
							(CPU_STK_SIZE)MMU_STK_SIZE,		
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
	static u8 internal_memget_num;
	CPU_INT08U *internal_buf;
	OS_ERR err;
	
	while(1)
	{
		key = KEY_Scan(0); //扫描按键
		switch(key)
		{
			case 1://申请内存
				internal_buf=OSMemGet((OS_MEM* )&INTERNAL_MEM,
															(OS_ERR* )&err);
				printf("internal_buf 内存申请之后的地址为:%#x\r\n",(u32)(internal_buf));
				if(err == OS_ERR_NONE) //内存申请成功 (3)
				{
					printf("Memory Get success! ");
					internal_memget_num++;
					sprintf((char*)internal_buf,"INTERNAL_MEM Use %d times",internal_memget_num);
					printf("%s \r\n",internal_buf);
				}
				if(err == OS_ERR_MEM_NO_FREE_BLKS) //内存块不足
				{
					printf("INTERNAL_MEM Empty! \r\n");
				}
			break;
			case 2://释放内存
				if(internal_buf != NULL) //internal_buf 不为空就释放内存
				{
					OSMemPut((OS_MEM* )&INTERNAL_MEM, //释放内存 (4)
									(void* )internal_buf,
									(OS_ERR* )&err);
					printf("internal_buf 内存释放之后的地址为:%#x\r\n",(u32)(internal_buf));
					printf("Memory Put success! \r\n");
				}
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
void MMU_task(void *p_arg)
{
	OS_ERR err;

	while(1)
	{
		printf("Total: Remain:%d :%d \r\n",INTERNAL_MEM.NbrMax,INTERNAL_MEM.NbrFree);

		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_PERIODIC,&err);//延时 100ms
	}
}
