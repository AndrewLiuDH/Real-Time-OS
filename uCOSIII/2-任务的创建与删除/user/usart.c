#include "usart.h"
#include "stdarg.h"
#include <stdio.h>

/********************************************************
*������    ��USART_Configuration 		 ������  �� ��
*��������  ����ʼ��RCC 			 ����ֵ    �� ��
*�������  ����
*********************************************************/

//PA9/USART1_TX
//PA10/USART1_RX
void USART1_Init(void)
{
/* ����USART��ʼ���ṹ�� USART_InitStructure */
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);						//ʹ�� USART1,GPIOC ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);		   	//ʹ�� USART1,GPIOC ʱ��
	
	/*	������Ϊ115200bps
	*	8λ���ݳ���
	*	1��ֹͣλ����У��
	*	����Ӳ��������
	*	��ֹUSARTʱ��
	*	ʱ�Ӽ��Ե�
	*	�ڵ�1�����ز�������
	*	���һλ���ݵ�ʱ�����岻�� SCLK ���
	*/
	USART_DeInit(USART1);
	USART_InitStructure.USART_BaudRate = 115200;	//USART ����Ĳ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	 //���͵�ֹͣλ��Ŀ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;  //��żģʽ����żʧ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	  // Ӳ��������ģʽʧ��
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	  // ����ʹ�ܣ�����ʹ��
	
	
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
	USART_Cmd(USART1, ENABLE);	  // ʹ��USART1
	
	kprintf("USART OK	\r\n");
}


/*
*********************************************************************************************************
*	�� �� ��: fputc
*	����˵��: �ض���putc��������������ʹ��kprintf�����Ӵ���2��ӡ���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int fputc(int ch, FILE *f)
{
	/* дһ���ֽڵ�USART1 */
	USART_SendData(USART1, (uint16_t) ch);

	/* �ȴ����ͽ��� */
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

void kprintf(char *fmt,...) //���ڸ�ʽ�����
{
	va_list ap;
	
	va_start(ap,fmt);
	vsprintf((char *)USART_BUF,fmt,ap);
	va_end(ap);
	
	USART_SendString(USART_BUF);
}


/*
*********************************************************************************************************
*	�� �� ��: fgetc
*	����˵��: �ض���getc��������������ʹ��scanff�����Ӵ���1��������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int fgetc(FILE *f)
{
	/* �ȴ�����1�������� */
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
	return (int)USART_ReceiveData(USART1);
}



