#ifndef _IWDG_H
#define _IWDG_H
#include "sys.h"

//V1.1 20140504
//增加了窗口看门狗相关函数。
////////////////////////////////////////////////////////////////////////////////// 	

void IWDG_Init(u8 prer,u16 rlr);//IWDG初始化
void IWDG_Feed(void);  //喂狗函数
#endif


