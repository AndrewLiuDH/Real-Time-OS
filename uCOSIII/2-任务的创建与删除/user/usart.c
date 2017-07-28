#include "usart.h"
#include "stdarg.h"
#include <stdio.h>

/********************************************************
*函数名    ：USART_Configuration 		 输出结果  ： 无
*函数描述  ：初始化RCC 			 返回值    ： 无
*输入参数  ：无
*********************************************************/

//PA9/USART1_TX
//PA10/USART1_RX
void USART1_Init(void)
{
/* 定义USART初始化结构体 USART_InitStructure */
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);						//使能 USART1,GPIOC 时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);		   	//使能 USART1,GPIOC 时钟
	
	/*	波特率为115200bps
	*	8位数据长度
	*	1个停止位，无校验
	*	禁用硬件流控制
	*	禁止USART时钟
	*	时钟极性低
	*	在第1个边沿捕获数据
	*	最后一位数据的时钟脉冲不从 SCLK 输出
	*/
	USART_DeInit(USART1);
	USART_InitStructure.USART_BaudRate = 115200;	//USART 传输的波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	 //发送的停止位数目
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;  //奇偶模式，奇偶失能
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	  // 硬件流控制模式失能
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	  // 发送使能，接收使能
	
	
	/* Connect pin to Periph */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	
	/* Configure USART Tx as alternate function  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure USART Rx as alternate function  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_Init(USART1, &USART_InitStructure);
	USART_Cmd(USART1, ENABLE);	  // 使能USART1
	
	kprintf("USART OK	\r\n");
}


/*
*********************************************************************************************************
*	函 数 名: fputc
*	功能说明: 重定义putc函数，这样可以使用kprintf函数从串口2打印输出
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int fputc(int ch, FILE *f)
{
	/* 写一个字节到USART1 */
	USART_SendData(USART1, (uint16_t) ch);

	/* 等待发送结束 */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	return ch;
}

u8 USART_BUF[100];

void USART_SendString(u8 *s)
{
	while(*s)
	{
		USART_SendData(USART1,*s++);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	}
}

void kprintf(char *fmt,...) //串口格式化输出
{
	va_list ap;
	
	va_start(ap,fmt);
	vsprintf((char *)USART_BUF,fmt,ap);
	va_end(ap);
	
	USART_SendString(USART_BUF);
}


/*
*********************************************************************************************************
*	函 数 名: fgetc
*	功能说明: 重定义getc函数，这样可以使用scanff函数从串口1输入数据
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int fgetc(FILE *f)
{
	/* 等待串口1输入数据 */
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
	return (int)USART_ReceiveData(USART1);
}



