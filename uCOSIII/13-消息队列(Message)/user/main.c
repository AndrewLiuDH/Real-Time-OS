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

#define KeyProcess_TASK_PRIO 5				//�������ȼ�
#define KeyProcess_STK_SIZE 128				//�����ջ��С
OS_TCB 	Keyprocess_TaskTCB;							//������ƿ�
CPU_STK KeyProcess_TASK_STK[KeyProcess_STK_SIZE];//�����ջ
void KeyProcess_task(void *p_arg);		//������

#define MSGDIS_TASK_PRIO 6			//�������ȼ�
#define MSGDIS_STK_SIZE 128				//�����ջ��С
OS_TCB 	Msgdis_TaskTCB;							//������ƿ�
CPU_STK MSGDIS_TASK_STK[MSGDIS_STK_SIZE];//�����ջ
void Msgdis_task(void *p_arg);		//������



////////////////////////��Ϣ����//////////////////////////////
#define KEYMSG_Q_NUM 1 //������Ϣ���е�����
#define DATAMSG_Q_NUM 4 //�������ݵ���Ϣ���е�����
OS_Q KEY_Msg; //����һ����Ϣ���У����ڰ�����Ϣ���ݣ�ģ����Ϣ����
OS_Q DATA_Msg; //����һ����Ϣ���У����ڷ�������
////////////////////////��ʱ��////////////////////////////////
u8 tmr1sta=0; //��Ƕ�ʱ���Ĺ���״̬
OS_TMR tmr1; //����һ����ʱ��
void tmr1_callback(void *p_tmr,void *p_arg);

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

	OSTmrCreate((OS_TMR*)&tmr1,
							(CPU_CHAR*)"tmr1",
							(OS_TICK)0,//delay 0ms
							(OS_TICK)50,//50*10=500ms
							(OS_OPT)OS_OPT_TMR_PERIODIC,//����ģʽ
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
								 
	//����MSGDIS����
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
				 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//����ʼ����			 
	OS_CRITICAL_EXIT();	//�����ٽ���
}

//��ѯ DATA_Msg ��Ϣ�����е��ܶ���������ʣ���������
void check_msg_queue(u8 *p)
{
	u8 msgq_remain_size; //��Ϣ����ʣ���С
	msgq_remain_size = DATA_Msg.MsgQ.NbrEntriesSize-DATA_Msg.MsgQ.NbrEntries;
	p = malloc(20); //�����ڴ�
	//��ʾ DATA_Msg ��Ϣ�����ܵĴ�С
	sprintf((char*)p,"Total Size:%d",DATA_Msg.MsgQ.NbrEntriesSize);
	printf("%s ",p);
	sprintf((char*)p,"Remain Size:%d",msgq_remain_size); //��ʾ DATA_Msg ʣ���С
	printf("%s \r\n",p);
	free(p); //�ͷ��ڴ�
}

//led0������
void Main_task(void *p_arg)
{
	u8 key,num;
	OS_ERR err;
	u8 *p;
	while(1)
	{
		key = KEY_Scan(0); //ɨ�谴��
		if(key)
		{
			//������Ϣ
			OSQPost((OS_Q* )&KEY_Msg,
							(void* )&key,
							(OS_MSG_SIZE )1,
							(OS_OPT )OS_OPT_POST_FIFO,
							(OS_ERR*) &err);
		}
		num++;
		if(num%50==0) check_msg_queue(p);//��� DATA_Msg ��Ϣ���е�����
		if(num==50)
		{
			num=0;
			LED0 = ~LED0;
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err); //��ʱ 10ms
	}
}

//��ʱ�� 1 �Ļص�����
void tmr1_callback(void *p_tmr,void *p_arg)
{
	u8 *pbuf;
	static u8 msg_num;
	OS_ERR err;
	pbuf = malloc(10); //���� 10 ���ֽ�
	if(pbuf) //�����ڴ�ɹ�
	{
		msg_num++;
		sprintf((char*)pbuf,"ALIENTEK %d",msg_num);
		//������Ϣ
		OSQPost((OS_Q* )&DATA_Msg,
						(void* )pbuf,
						(OS_MSG_SIZE )10,
						(OS_OPT )OS_OPT_POST_FIFO,
						(OS_ERR* )&err);
		if(err != OS_ERR_NONE)
		{
			free(pbuf); //�ͷ��ڴ�
			OSTmrStop(&tmr1,OS_OPT_TMR_NONE,0,&err); //ֹͣ��ʱ�� 1
			tmr1sta = !tmr1sta;
			printf("TMR1 STOP! \r\n");
		}
	}
}

//�������������������
void KeyProcess_task(void *p_arg)
{
	u8 num;
	u8 *key;
	OS_MSG_SIZE size;
	OS_ERR err;
	while(1)
	{
		//������Ϣ KEY_Msg
		key=OSQPend((OS_Q* )&KEY_Msg,
								(OS_TICK )0,
								(OS_OPT )OS_OPT_PEND_BLOCKING,
								(OS_MSG_SIZE* )&size,
								(CPU_TS* )0,
								(OS_ERR* )&err);
		
		switch(*key)
		{
			case 1: //KEY_UP ���� LED1
			LED1 = ~LED1;
			break;
			case 2: //KEY2 ���Ʒ�����
			LED2 = ~LED2;
			break;
			case 3: //KEY0 ˢ�� LCD ����
			tmr1sta = !tmr1sta;
			if(tmr1sta)
			{
				OSTmrStart(&tmr1,&err);
				printf("TMR1 START! \r\n");
			}else
			{
				OSTmrStop(&tmr1,OS_OPT_TMR_NONE,0,&err); //ֹͣ��ʱ�� 1
				printf("TMR1 STOP! \r\n");
			}
			break;
		}
	}
}
//��ʾ��Ϣ�����е���Ϣ
void Msgdis_task(void *p_arg)
{
	u8 *p;
	OS_MSG_SIZE size;
	OS_ERR err;
	while(1)
	{
		//������Ϣ
		p=OSQPend((OS_Q* )&DATA_Msg,
							(OS_TICK )0,
							(OS_OPT )OS_OPT_PEND_BLOCKING,
							(OS_MSG_SIZE* )&size,
							(CPU_TS* )0,
							(OS_ERR* )&err);
		printf("%s \r\n",p);
		free(p); //�ͷ��ڴ�
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_PERIODIC,&err); //��ʱ 1s
	}
}
