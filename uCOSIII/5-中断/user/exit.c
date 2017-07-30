#include "exit.h"
#include "led.h"
#include "includes.h"	

static void KEY_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//ʹ��GPIOA,GPIOEʱ��
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10; //KEY0 KEY1 KEY2��Ӧ����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��GPIOE2,3,4
}

void EXTI_init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	KEY_init(); //������Ӧ��IO�ڳ�ʼ��

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//ʹ��SYSCFGʱ��
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource7);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource8);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource9);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource10);

	/* ����EXTI_Line7,8,9,10 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line7 | EXTI_Line8 | EXTI_Line9 | EXTI_Line10;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½��ش���
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//�ж���ʹ��
  EXTI_Init(&EXTI_InitStructure);//����
 
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;//�ⲿ�ж�0
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//�����ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);//����

}

//�ⲿ�ж�0�������
void EXTI9_5_IRQHandler(void)
{	
	OSIntEnter();
	delay_ms(10);//��ʱ������������������
	if(EXTI_GetFlagStatus(EXTI_Line7)!=RESET)
	{
		LED0=!LED0;
		printf("LED0 Status Reversed\r\n");
		
		EXTI_ClearFlag(EXTI_Line7); //���LINE0�ϵ��жϱ�־λ 
	}else if(EXTI_GetITStatus(EXTI_Line8)!=RESET)
	{
		LED1=!LED1;
		printf("LED1 Status Reversed\r\n");
		
		EXTI_ClearITPendingBit(EXTI_Line8); //���LINE0�ϵ��жϱ�־λ 
	}else if(EXTI_GetITStatus(EXTI_Line9)!=RESET)
	{
		LED2=!LED2;
		printf("LED2 Status Reversed\r\n");
		EXTI_ClearITPendingBit(EXTI_Line9); //���LINE0�ϵ��жϱ�־λ 
	}
	
	OSIntExit();    	//�˳��ж�
}



