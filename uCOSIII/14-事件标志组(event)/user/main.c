#include "sys.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "usart.h"
#include "includes.h"

#define START_TASK_PRIO 3				//�������ȼ�
#define START_STK_SIZE 512			//�����ջ��С
OS_TCB StartTaskTCB;						//������ƿ�
CPU_STK START_TASK_STK[START_STK_SIZE];//�����ջ
void start_task(void *p_arg);		//������

#define Main_TASK_PRIO 4				//�������ȼ�
#define Main_STK_SIZE 128				//�����ջ��С
OS_TCB Main_TaskTCB;							//������ƿ�
CPU_STK Main_TASK_STK[Main_STK_SIZE];//�����ջ
void Main_task(void *p_arg);		//������

#define EventFlags_TASK_PRIO 5				//�������ȼ�
#define EventFlags_STK_SIZE 128				//�����ջ��С
OS_TCB EventFlags_TaskTCB;							//������ƿ�
CPU_STK EventFlags_TASK_STK[EventFlags_STK_SIZE];//�����ջ
void EventFlags_task(void *p_arg);		//������

////////////////////////�¼���־��//////////////////////////////
#define KEY0_FLAG 0x01
#define KEY1_FLAG 0x02
#define KEYFLAGS_VALUE 0X00
OS_FLAG_GRP EventFlags; //����һ���¼���־��

int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();

	delay_init(168);
	USART1_Init();
	LED_Init();
	KEY_init();

		OSInit(&err);		//��ʼ��UCOSIII
	OS_CRITICAL_ENTER();//�����ٽ���
	//������ʼ����
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,					//������ƿ�
				 (CPU_CHAR	* )"start task", 							//��������
                 (OS_TASK_PTR )start_task, 				//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,									//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,										//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,										//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);								//��Ÿú�������ʱ�ķ���ֵ
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
	OSStart(&err);  //����UCOSIII
	
	while(1);
}

//��ʼ������
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	
	OS_CRITICAL_ENTER();	//�����ٽ���
	
	//����һ���¼���־��
	OSFlagCreate((OS_FLAG_GRP* )&EventFlags, //ָ���¼���־��
							(CPU_CHAR* )"Event Flags", //����
							(OS_FLAGS )KEYFLAGS_VALUE, //��ʼֵ
							(OS_ERR* )&err); //������
	//����LED0����
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
				 
	//����LED1����
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

	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//����ʼ����			 
	OS_CRITICAL_EXIT();	//�����ٽ���
}

//led0������
void Main_task(void *p_arg)
{
	u8 key,num;
	OS_FLAGS flags_num;
	OS_ERR err;
	while(1)
	{
		key = KEY_Scan(0); //ɨ�谴��
		if(key == 1)
		{
			//���¼���־�� EventFlags ���ͱ�־
			flags_num=OSFlagPost((OS_FLAG_GRP*)&EventFlags,
													(OS_FLAGS )KEY0_FLAG,
													(OS_OPT )OS_OPT_POST_FLAG_SET,
													(OS_ERR* )&err);
			printf("�¼���־�� EventFlags ��ֵ:%d\r\n",flags_num);
		}else if(key == 2)
		{
			//���¼���־�� EventFlags ���ͱ�־
			flags_num=OSFlagPost((OS_FLAG_GRP*)&EventFlags,
													(OS_FLAGS )KEY1_FLAG,
													(OS_OPT )OS_OPT_POST_FLAG_SET,
													(OS_ERR* )&err);
			printf("�¼���־�� EventFlags ��ֵ:%d\r\n",flags_num);
		}
		num++;
		if(num==50)
		{
			num=0;
			LED0 = ~LED0;
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err); //��ʱ 10ms
	}
}

//led1������
void EventFlags_task(void *p_arg)
{
	u8 num;
	OS_ERR err;
	while(1)
	{
		//�ȴ��¼���־��
		OSFlagPend((OS_FLAG_GRP*)&EventFlags,
							(OS_FLAGS)KEY0_FLAG|KEY1_FLAG,
							(OS_TICK )0,
							(OS_OPT  )OS_OPT_PEND_FLAG_SET_ALL+OS_OPT_PEND_FLAG_CONSUME,
							(CPU_TS* )0,
							(OS_ERR* )&err);
		num++;
		LED1 = ~LED1;
		printf("�¼���־�� EventFlags ��ֵ:%d\r\n",EventFlags.Flags);
	}
}

