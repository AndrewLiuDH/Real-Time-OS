#ifndef _IWDG_H
#define _IWDG_H
#include "sys.h"

//V1.1 20140504
//�����˴��ڿ��Ź���غ�����
////////////////////////////////////////////////////////////////////////////////// 	

void IWDG_Init(u8 prer,u16 rlr);//IWDG��ʼ��
void IWDG_Feed(void);  //ι������
#endif


