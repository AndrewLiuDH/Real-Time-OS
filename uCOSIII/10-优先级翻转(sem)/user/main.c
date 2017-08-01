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

#define TASKB_TASK_PRIO 6				//�������ȼ�
#define TASKB_STK_SIZE 128				//�����ջ��С
OS_TCB TASKBTaskTCB;							//������ƿ�
CPU_STK TASKB_TASK_STK[TASKB_STK_SIZE];//�����ջ
void TASKB_task(void *p_arg);		//������

#define TASKC_TASK_PRIO 5				//�������ȼ�
#define TASKC_STK_SIZE 128				//�����ջ��С
OS_TCB TASKCTaskTCB;							//������ƿ�
CPU_STK TASKC_TASK_STK[TASKC_STK_SIZE];//�����ջ
void TASKC_task(void *p_arg);		//������

#define TASKD_TASK_PRIO 4				//�������ȼ�
#define TASKD_STK_SIZE 128				//�����ջ��С
OS_TCB TASKDTaskTCB;							//������ƿ�
CPU_STK TASKD_TASK_STK[TASKC_STK_SIZE];//�����ջ
void TASKD_task(void *p_arg);		//������


OS_SEM TEST_SEM; //����һ���ź���


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
	
	//////////////////////////////////////////////ʹ���ź���֮ǰҪ�ȴ���
	OS_CRITICAL_ENTER();	//�����ٽ���
	OSSemCreate ((OS_SEM* )&TEST_SEM, //ָ���ź���
							(CPU_CHAR* )"TEST_SEM", //�ź���������
							(OS_SEM_CTR )1, //�ź���ֵΪ 1
							(OS_ERR* )&err);
	
	//����MidPrio����
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
				 
	//����MidPrio����
	OSTaskCreate((OS_TCB 	* )&TASKCTaskTCB,		
							(CPU_CHAR	* )"TASKC task", 		
							(OS_TASK_PTR )TASKC_task, 			
							(void		* )0,					
							(OS_PRIO	  )TASKC_TASK_PRIO,     	
							(CPU_STK   * )&TASKC_TASK_STK[0],	
							(CPU_STK_SIZE)TASKC_STK_SIZE/10,	
							(CPU_STK_SIZE)TASKC_STK_SIZE,		
							(OS_MSG_QTY  )0,					
							(OS_TICK	  )0,					
							(void   	* )0,				
							(OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
							(OS_ERR 	* )&err);
							
	//����HighPrio����
	OSTaskCreate((OS_TCB 	* )&TASKDTaskTCB,		
							(CPU_CHAR	* )"TASKD task", 		
							(OS_TASK_PTR )TASKD_task, 			
							(void		* )0,					
							(OS_PRIO	  )TASKD_TASK_PRIO,     	
							(CPU_STK   * )&TASKD_TASK_STK[0],	
							(CPU_STK_SIZE)TASKD_STK_SIZE/10,	
							(CPU_STK_SIZE)TASKD_STK_SIZE,		
							(OS_MSG_QTY  )0,					
							(OS_TICK	  )0,					
							(void   	* )0,				
							(OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
							(OS_ERR 	* )&err);

	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//����ʼ����			 
	OS_CRITICAL_EXIT();	//�����ٽ���
}

u8 share_resource[30]; //������Դ��

//led0������
void TASKB_task(void *p_arg)
{
	static u32 times;

	OS_ERR err;
	while(1)
	{
		OSSemPend(&TEST_SEM,0,OS_OPT_PEND_BLOCKING,0,&err); //�����ź���(3)
		printf("\r\n ���� 1 :  �����ȼ�����\r\n");
		for(times=0;times<20000000;times++) //(4)
		{
			OSSched(); //�������
		}
		OSSemPost(&TEST_SEM,OS_OPT_POST_1,&err); //�ͷ��ź��� (5)
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_PERIODIC,&err); //��ʱ 1s
	}
}

//led1������
void TASKC_task(void *p_arg)
{
	OS_ERR err;
	
	while(1)
	{
		printf("\r\n ���� 2 :  �����ȼ�����\r\n");
		LED1 = ~LED1;
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_PERIODIC,&err); //��ʱ 1s
	}
}


//led1������
void TASKD_task(void *p_arg)
{
	OS_ERR err;
	
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);//��ʱ500ms
		printf("\r\n ���� 3 :  �����ȼ�����Sem \r\n");
		OSSemPend(&TEST_SEM,0,OS_OPT_PEND_BLOCKING,0,&err); //�����ź���
		printf("\r\n ���� 3 :  �����ȼ�����  �� \r\n");
		LED1 = ~LED1;
		OSSemPost(&TEST_SEM,OS_OPT_POST_1,&err); //�����ź���
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err); //��ʱ 500ms

	}
}


