/**********************************************************
* @ File name -> lcd_drive.c
* @ Version   -> V1.0.2
* @ Date      -> 03-09-2014
* @ Brief     -> TFT彩屏底层驱动函数

 V1.0.1
* @ Revise    -> 编写完成GPIO控制LCD驱动的初始化函数

 V1.0.2
* @ Revise    -> 优化移植时的方便性，尽量做到只是修改少量的宏定义即可
**********************************************************/

#include "lcd_drive.h"

/**********************************************************
                 |                        |
                 |  下面进入移植时修改区  |
                \|/                      \|/
**********************************************************/

//=========================================================
//否则使用普通GPIO控制LCD
//=========================================================

/**********************************************************
* 函数功能 ---> 通讯、控制端口初始化
* 入口参数 ---> none
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
void LCD_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE,ENABLE);

	//LCD控制引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOD,GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
	
	// DB15--0
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	GPIO_SetBits(GPIOE,GPIO_Pin_All);

}
/**********************************************************
* 函数功能 ---> 写数据到总线
* 入口参数 ---> val：要写入的数据
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
void LCD_Write_Bus(u16 val)
{
/* 使用16bits数据总线宽度时的逻辑控制时序 */

//	LCD_BUS_OUT16();	//设置总线为输出方向
	LCD_RS_SET;
	LCD_CS_CLR;
	DATAOUT(val);
	LCD_WR_CLR;
	LCD_WR_SET;
	LCD_CS_SET;
}
/**********************************************************
* 函数功能 ---> 读取LCD数据
* 入口参数 ---> none
* 返回数值 ---> 读取到的数据
* 功能说明 ---> none
**********************************************************/
u16 LCD_Read_Data(void)
{
	u16 temp=0;	//接收数据缓存

/* 使用16bits数据总线宽度时的逻辑控制时序 */
	LCD_BUS_IN16();

	LCD_RS_SET;
	LCD_CS_CLR;
	//读取数据(读寄存器时,并不需要读2次)
	LCD_RD_CLR;
	temp=DATAIN;  
	LCD_RD_SET;
	LCD_CS_SET; 

	LCD_BUS_OUT16();
	
	return temp;	//返回读取到的数据     
}

/**********************************************************
                /|\                   /|\
                 |   结束移植时修改区   |
				 |                     |
**********************************************************/

/**********************************************************
* 函数功能 ---> 背光、复位控制端口初始化
* 入口参数 ---> none
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
void LCD_RST_BL_Init(void)
{
	/******背光管脚******/
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE,ENABLE);

	//LCD控制引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOD,GPIO_Pin_11);
	GPIO_ResetBits(GPIOD,GPIO_Pin_9);
}

/**********************************************************
        下面是相关的应用调用API函数，非必要可不修改
**********************************************************/


//=========================================================
//否则使用普通GPIO控制LCD                  ********************************************** BULA
//=========================================================

/**********************************************************
* 函数功能 ---> 写数据到LCD寄存器（主要写命令或寄存器地址）
* 入口参数 ---> reg_val：要写入的数据
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
void LCD_Write_Register(u16 reg_val)
{
	LCD_RS_CLR;		//写地址  
 	LCD_CS_CLR; 
	DATAOUT(reg_val);
	LCD_WR_CLR;
	LCD_WR_SET;
 	LCD_CS_SET;	
}
/**********************************************************
* 函数功能 ---> 写数据到LCD RAM
* 入口参数 ---> dat：要写入的数据
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
void LCD_Write_Data(u16 dat)
{
	LCD_WR_Data(dat);
}
/**********************************************************
* 函数功能 ---> 向LCD某个寄存器写入数据
* 入口参数 ---> reg：要写入数据的寄存器地址
*               dat：要写入的数据
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
void LCD_WriteRegisterData(u16 reg, u16 dat)
{
	LCD_Write_Register(reg);
	LCD_WR_Data(dat);
}
/**********************************************************
* 函数功能 ---> 读取LCD某个寄存器数值
* 入口参数 ---> reg：要读取数据的寄存器地址
* 返回数值 ---> 寄存器数值
* 功能说明 ---> none
**********************************************************/
u16 LCD_ReadRegisterData(u16 reg)
{
	LCD_Write_Register(reg);
	return LCD_Read_Data();
}

//=========================================================
//=========================================================

