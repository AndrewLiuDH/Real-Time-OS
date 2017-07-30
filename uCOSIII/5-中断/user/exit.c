#include "exit.h"
#include "led.h"
#include "includes.h"	

static void KEY_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOA,GPIOE时钟
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10; //KEY0 KEY1 KEY2对应引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIOE2,3,4
}

void EXTI_init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	KEY_init(); //按键对应的IO口初始化

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource7);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource8);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource9);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource10);

	/* 配置EXTI_Line7,8,9,10 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line7 | EXTI_Line8 | EXTI_Line9 | EXTI_Line10;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿触发
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//中断线使能
  EXTI_Init(&EXTI_InitStructure);//配置
 
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;//外部中断0
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);//配置

}

//外部中断0服务程序
void EXTI9_5_IRQHandler(void)
{	
	OSIntEnter();
	delay_ms(10);//延时函数放在外面会出问题
	if(EXTI_GetFlagStatus(EXTI_Line7)!=RESET)
	{
		LED0=!LED0;
		printf("LED0 Status Reversed\r\n");
		
		EXTI_ClearFlag(EXTI_Line7); //清除LINE0上的中断标志位 
	}else if(EXTI_GetITStatus(EXTI_Line8)!=RESET)
	{
		LED1=!LED1;
		printf("LED1 Status Reversed\r\n");
		
		EXTI_ClearITPendingBit(EXTI_Line8); //清除LINE0上的中断标志位 
	}else if(EXTI_GetITStatus(EXTI_Line9)!=RESET)
	{
		LED2=!LED2;
		printf("LED2 Status Reversed\r\n");
		EXTI_ClearITPendingBit(EXTI_Line9); //清除LINE0上的中断标志位 
	}
	
	OSIntExit();    	//退出中断
}



