#ifndef __USART_H__
#define __USART_H__

#include "stm32f4xx.h"

void USART1_Init(void);
void NVIC_Configuation(void);
void kprintf(char *fmt,...); //���ڸ�ʽ�����

#endif

