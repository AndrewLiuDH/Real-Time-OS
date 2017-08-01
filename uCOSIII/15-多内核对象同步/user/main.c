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
OS_TCB MainTaskTCB;							//������ƿ�
CPU_STK Main_TASK_STK[Main_STK_SIZE];//�����ջ
void Main_task(void *p_arg);		//������

#define Multi_TASK_PRIO 5				//�������ȼ�
#define Multi_STK_SIZE 128				//�����ջ��С
OS_TCB 	Multi_TaskTCB;							//������ƿ�
CPU_STK Multi_TASK_STK[Multi_STK_SIZE];//�����ջ
void Multi_task(void *p_arg);		//������

////////////////////////��Ϣ����//////////////////////////////
#define KEYMSG_Q_NUM 1 //������Ϣ���е�����
OS_Q Multi_Q; //����һ����Ϣ���У����ڰ�����Ϣ���ݣ�ģ����Ϣ����

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

	//����LED0����
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
							
	//����key��������
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
								 
				 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//����ʼ����			 
	OS_CRITICAL_EXIT();	//�����ٽ���
}

//led0������
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
		key = KEY_Scan(0); //ɨ�谴��
		switch(key)
		{
			case 1://�����ź���1
			OSSemPost((OS_SEM *)&Multi_Sem1,
								(OS_OPT )OS_OPT_POST_1,
								(OS_ERR*) &err);
			printf("�����ź���1 %d\r\n",Multi_Sem1.Ctr);
			break;
			case 2://�����ź���2
			OSSemPost((OS_SEM *)&Multi_Sem2,
								(OS_OPT )OS_OPT_POST_1,
								(OS_ERR*) &err);
			printf("�����ź���2 %d\r\n",Multi_Sem2.Ctr);
			break;
			case 3://������Ϣ
			msg_num++;
			sprintf((char*)pbuf,"ALIENTEK %d",msg_num);
			
			OSQPost ((OS_Q *)&Multi_Q,
               (void *)pbuf,
               (OS_MSG_SIZE)sizeof(pbuf),
               (OS_OPT  )OS_OPT_POST_FIFO,
               (OS_ERR *)&err);
			printf("������Ϣ %s\r\n",pbuf);
			break;
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

//�������������������
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
		//������Ϣ KEY_Msg
		index=OSPendMulti((OS_PEND_DATA* )pend_multi_tbl,
										(OS_OBJ_QTY )3,
										(OS_TICK )0,
										(OS_OPT )OS_OPT_PEND_BLOCKING,
										(OS_ERR* )&err);
		
		printf("Now Have %d Core Object \r\n",index);
		LED1 = ~LED1;
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_PERIODIC,&err); //��ʱ 1s
	}
}
