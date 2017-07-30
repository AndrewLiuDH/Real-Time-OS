#include "sys.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "includes.h"

#define START_TASK_PRIO 3				//�������ȼ�
#define START_STK_SIZE 512			//�����ջ��С
OS_TCB StartTaskTCB;						//������ƿ�
CPU_STK START_TASK_STK[START_STK_SIZE];//�����ջ
void start_task(void *p_arg);		//������

#define TASKB_TASK_PRIO 4				//�������ȼ�
#define TASKB_STK_SIZE 128				//�����ջ��С
OS_TCB TASKBTaskTCB;							//������ƿ�
CPU_STK TASKB_TASK_STK[TASKB_STK_SIZE];//�����ջ
void TASKB_task(void *p_arg);		//������

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

OS_TMR tmr1; //��ʱ�� 1
OS_TMR tmr2; //��ʱ�� 2

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
	
	OSTmrCreate((OS_TMR* )&tmr1, //��ʱ�� 1 (1)
							(CPU_CHAR* )"tmr1", //��ʱ������
							(OS_TICK )20, //20*10=200ms
							(OS_TICK )100, //100*10=1000ms
							(OS_OPT )OS_OPT_TMR_PERIODIC, //����ģʽ
							(OS_TMR_CALLBACK_PTR)tmr1_callback, //��ʱ�� 1 �ص�����
							(void* )0, //����Ϊ 0
							(OS_ERR* )&err); //���صĴ�����
	//������ʱ�� 2
	OSTmrCreate((OS_TMR* )&tmr2, //(2)
							(CPU_CHAR* )"tmr2",
							(OS_TICK )200, //200*10=2000ms
							(OS_TICK )0,
							(OS_OPT )OS_OPT_TMR_ONE_SHOT, //���ζ�ʱ��
							(OS_TMR_CALLBACK_PTR)tmr2_callback,//��ʱ�� 2 �ص�����
							(void* )0,
							(OS_ERR* )&err);
	
	OS_CRITICAL_ENTER();	//�����ٽ���
	//����LED0����
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

	OSTaskDel((OS_TCB*)&StartTaskTCB,&err);		//ɾ����ʼ����			 
	OS_CRITICAL_EXIT();	//�����ٽ���
}

//led0������
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
			case 1: //�� key_up ���µĻ��򿪶�ʱ�� 1
						OSTmrStart(&tmr1,&err); //������ʱ�� 1
						printf("������ʱ�� 1\r\n");
						break;
			case 2: //�� key0 ���µĻ��򿪶�ʱ�� 2
						OSTmrStart(&tmr2,&err); //������ʱ�� 1
						printf("������ʱ�� 2\r\n");
						break;
			case 3: //�� key0 ���µĻ��򿪶�ʱ�� 2
						if(tmr1.State!=OS_TMR_STATE_STOPPED)
						{
							OSTmrStop(&tmr1,OS_OPT_TMR_NONE,0,&err); //�رն�ʱ�� 1
							printf("�رն�ʱ�� 1\r\n");
						}
						
						if(tmr2.State==OS_TMR_STATE_COMPLETED)
						{
							OSTmrStop(&tmr2,OS_OPT_TMR_NONE,0,&err); //�رն�ʱ�� 1
							printf("�رն�ʱ�� 2\r\n");
						}break;
			default:
						printf("���ڰ��������ð�����%d\r\n",key);
			}
		}
		
		num++;
		if(num==50) //ÿ 500msLED0 ��˸һ��
		{
			num = 0;
			LED0 = ~LED0;
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err); //��ʱ 10ms		
	}
}

//tmr1�ص�����
void tmr1_callback(void *p_tmr, void *p_arg)
{
	OS_ERR err;
	static uint8_t Tmr1RunTimes=0; 
	
//	while(1)//����while(1)
//	{
		LED2=!LED2;
		Tmr1RunTimes++;
		printf("Tmr1 has Running for %d Times!\r\n",Tmr1RunTimes);
//	}
}

//tmr2�ص�����
void tmr2_callback(void *p_tmr, void *p_arg)
{
	OS_ERR err;
	static uint8_t Tmr2RunTimes=0; 
	
	LED3=!LED3;
	Tmr2RunTimes++;
	printf("Tmr2 has Running for %d Times!\r\n",Tmr2RunTimes);
}


