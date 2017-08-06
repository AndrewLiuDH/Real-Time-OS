/**********************************************************
* @ File name -> lcd_drive.h
* @ Version   -> V1.0.2
* @ Date      -> 03-09-2014
* @ Brief     -> TFT彩屏底层驱动函数头文件

 V1.0.1
* @ Revise    -> 编写完成GPIO控制LCD驱动的初始化函数

 V1.0.2
* @ Revise    -> 优化移植时的方便性，尽量做到只是修改少量的宏定义即可
**********************************************************/

#ifndef _lcd_drive_h_
#define _lcd_drive_h_

/**********************************************************
                     外部函数头文件
         应用到不同的外设头文件请在这里修改即可                        
**********************************************************/

#include "sys.h"

/**********************************************************
                    控制LCD相关的宏定义
**********************************************************/


/**********************************************************
                    定义背光接口信息
               不同的控制接口请修改这里                       
**********************************************************/

#define LCD_BLControl               PDout(9)    //背光控制

/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/

//=========================================================
//使用GPIO控制
//=========================================================

	/*******硬件相关的宏定义*******
	LCD_CS ---> PC9   片选
	LCD_RS ---> PC8   数据/命令
	LCD_WR ---> PC7   写数据
	LCD_RD ---> PC6   读数据
	**********/
	#define	LCD_CS_SET  PDout(15)=1    //片选端口  	PC9
	#define	LCD_RS_SET	PDout(14)=1    //数据/命令   PC8	   
	#define	LCD_WR_SET	PDout(13)=1    //写数据			PC7
	#define	LCD_RD_SET	PDout(12)=1    //读数据			PC6
	#define	LCD_RST_SET	PDout(11)=1    //复位				PC5
										
	#define	LCD_CS_CLR  PDout(15)=0     //片选端口  	PC9
	#define	LCD_RS_CLR	PDout(14)=0     //数据/命令  PC8	   
	#define	LCD_WR_CLR	PDout(13)=0     //写数据			PC7
	#define	LCD_RD_CLR	PDout(12)=0     //读数据			PC6
	#define	LCD_RST_CLR	PDout(11)=0     //复位				PC5								    

	//PB0~15,作为数据线
	#define DATAOUT(x) GPIOE->ODR=x; //数据输出
	#define DATAIN     GPIOE->IDR;   //数据输入	
	
	/* 读取数据时设置数据总线方向为输入 */
	#define LCD_BUS_IN16()		{GPIOE->BSRRL = 0x88888888;GPIOE->BSRRH = 0x88888888;GPIOE->ODR=0x0000;}
	
	/* 读取数据时设置数据总线方向为输入 */
	#define LCD_GRAM_IN16()		{GPIOE->BSRRL = 0x88888888;GPIOE->BSRRH = 0x88888888;GPIOE->ODR=0xffff;}

	/* 写入数据时设置数据总线方向为输出 */
	#define LCD_BUS_OUT16()		{GPIOE->BSRRL = 0x33333333;GPIOE->BSRRH = 0x33333333;GPIOE->ODR=0Xffff;}


/**********************************************************
                       外部功能函数                      
**********************************************************/
	
/**********************************************************
* 宏功能 ---> 写数据到LCD RAM
* 入口参数 ---> dat：要写入的数据
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
#define LCD_WR_Data(val)	{\
	LCD_RS_SET;\
	LCD_CS_CLR;\
	DATAOUT(val);\
	LCD_WR_CLR;\
	LCD_WR_SET;\
	LCD_CS_SET;\
}

void LCD_GPIO_Init(void);     //通讯、控制端口初始化

/* 下面通用外调函数 */
void LCD_RST_BL_Init(void);   //背光、复位控制端口初始化     
void LCD_Write_Register(u16 reg_val);     //写数据到LCD寄存器（主要写命令或寄存器地址）
void LCD_Write_Data(u16 dat); //写数据到LCD RAM
u16 LCD_Read_Data(void);      //读取LCD数据
void LCD_WriteRegisterData(u16 reg, u16 dat);   //向LCD某个寄存器写入数据
u16 LCD_ReadRegisterData(u16 reg);  //读取LCD某个寄存器数值


#endif

