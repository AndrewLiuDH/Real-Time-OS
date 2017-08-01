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

#define MMU_TASK_PRIO 5				//�������ȼ�
#define MMU_STK_SIZE 128				//�����ջ��С
OS_TCB 	MMU_TaskTCB;							//������ƿ�
CPU_STK MMU_TASK_STK[MMU_STK_SIZE];//�����ջ
void MMU_task(void *p_arg);		//������

OS_MEM INTERNAL_MEM; //����һ���洢��
#define INTERNAL_MEM_NUM 5 //�洢���д洢������
#define INTERNAL_MEMBLOCK_SIZE 100 //ÿ���洢���С����һ��ָ�����ռ�� 4
//�ֽ����Կ�Ĵ�Сһ��ҪΪ 4 �ı�������
//�������һ��ָ�����(4 �ֽ�)ռ�õĿռ�,
//����Ļ��洢�鴴�����ɹ�
//�洢�����ڴ�أ�ʹ���ڲ� RAM
__align(4) CPU_INT08U Internal_RamMemp[INTERNAL_MEM_NUM] [INTERNAL_MEMBLOCK_SIZE];

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
	
	//����һ���洢����
	OSMemCreate((OS_MEM* )&INTERNAL_MEM, //�洢�����ƿ� (1)
							(CPU_CHAR* )"Internal Mem", //�洢������
							(void* )&Internal_RamMemp[0][0], //�洢�ռ����ַ
							(OS_MEM_QTY )INTERNAL_MEM_NUM, //�洢������
							(OS_MEM_SIZE )INTERNAL_MEMBLOCK_SIZE,//�洢���С
							(OS_ERR* )&err); //������

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
								 
				 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//����ʼ����			 
	OS_CRITICAL_EXIT();	//�����ٽ���
}

//led0������
void Main_task(void *p_arg)
{
	u8 key,num;
	static u8 internal_memget_num;
	CPU_INT08U *internal_buf;
	OS_ERR err;
	
	while(1)
	{
		key = KEY_Scan(0); //ɨ�谴��
		switch(key)
		{
			case 1://�����ڴ�
				internal_buf=OSMemGet((OS_MEM* )&INTERNAL_MEM,
															(OS_ERR* )&err);
				printf("internal_buf �ڴ�����֮��ĵ�ַΪ:%#x\r\n",(u32)(internal_buf));
				if(err == OS_ERR_NONE) //�ڴ�����ɹ� (3)
				{
					printf("Memory Get success! ");
					internal_memget_num++;
					sprintf((char*)internal_buf,"INTERNAL_MEM Use %d times",internal_memget_num);
					printf("%s \r\n",internal_buf);
				}
				if(err == OS_ERR_MEM_NO_FREE_BLKS) //�ڴ�鲻��
				{
					printf("INTERNAL_MEM Empty! \r\n");
				}
			break;
			case 2://�ͷ��ڴ�
				if(internal_buf != NULL) //internal_buf ��Ϊ�վ��ͷ��ڴ�
				{
					OSMemPut((OS_MEM* )&INTERNAL_MEM, //�ͷ��ڴ� (4)
									(void* )internal_buf,
									(OS_ERR* )&err);
					printf("internal_buf �ڴ��ͷ�֮��ĵ�ַΪ:%#x\r\n",(u32)(internal_buf));
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
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err); //��ʱ 10ms
	}
}

//�������������������
void MMU_task(void *p_arg)
{
	OS_ERR err;

	while(1)
	{
		printf("Total: Remain:%d :%d \r\n",INTERNAL_MEM.NbrMax,INTERNAL_MEM.NbrFree);

		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_PERIODIC,&err);//��ʱ 100ms
	}
}
