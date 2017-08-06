/**********************************************************
* @ File name -> lcd_tft.h
* @ Version   -> V1.0.2
* @ Date      -> 02-25-2014
* @ Brief     -> TFT彩屏上层应用函数头文件

 V1.0.1
* @ Revise    -> A、将LCD_Display_String_BK1函数进行修改
*                B、修改为带位数显示选择，默认不叠加显示
*                C、将函数名修改为LCD_Display_String_Num

 V1.0.2
* @ Revise    -> 修正数字串显示函数小bug，原来是从高位开始数的显示N位数字字符
*                修正后从最低位开始数的显示N位数字字符
*                比如显示00336.85这个数，显示8位就全部显示（高位为0也显示），显示5为就是36.85（小数点都算进显示位数）
**********************************************************/

#ifndef _lcd_tft_h_
#define _lcd_tft_h_

/**********************************************************
                     外部函数头文件                        
**********************************************************/

#include "lcd_drive.h"	//LCD驱动底层接口应用函数

#include "fontupd.h"	//汉字字模数据来源函数

/**********************************************************
                定义TFT_LCD操作相关结构
**********************************************************/

//LCD参数结构
typedef struct
{
	u16 width;	//LCD显示宽度
	u16 height;	//LCD显示高度
	u16 ID;	//LCD型号
	u8 dir;	//LCD是横屏显示还是竖屏显示，0：为竖屏，1：为横屏
	u8 wgramcmd;	//写GRAM命令
	u8 setxcmd;	//设置x坐标
	u8 setycmd;	//设置y坐标
}lcd_dev;

extern lcd_dev LCD_Manage;

extern u16 Point_color;	//默认点的颜色为蓝色
extern u16 Back_color;	//默认背景颜色为黑色

/**********************************************************
                     命令和参数宏定义
**********************************************************/

//LCD命令宏定义
#define Display_OFF			0x28	//关闭显示屏
#define Display_ON			0x29	//打开显示屏

#define LCD_EnterSleep		0x10	//LCD进入睡眠模式
#define LCD_ExitSleep		0x11	//LCD退出睡眠模式

#define LCD_SofReset		0x01	//LCD软件复位，打开振荡器

//LCD扫描方向定义
#define L2R_U2D			0	//从左到右，从上到下
#define L2R_D2U			1	//从左到右，从下到上
#define R2L_U2D			2	//从右到左，从上到下
#define R2L_D2U			3	//从右到左，从下到上

#define U2D_L2R			4	//从上到下，从左到右
#define U2D_R2L			5	//从上到下，从右到左
#define D2U_L2R			6	//从下到上，从左到右
#define D2U_R2L			7	//从下到上，从右到左

#define DEF_Scan_DIR	L2R_U2D	//默认扫描方向为从左到右，从上到下	

//定义颜色值
#define RED			0xf800	//红色
#define BLUE		0x01cf	//蓝色	0x001f	
#define WHITE		0xffff	//白色
#define BLACK		0x0000	//黑色
#define GREEN		0x07e0	//绿色
#define YELLOW		0xffe0	//黄色
#define ORANGE		0x05bf	//橙色
#define GREY		0xefbd	//灰色	0xf7de

#define BLUE2		0x051f
#define MAGENTA		0xf81f	//洋红
#define CYAN		0x7fff	//蓝绿色

//定义行列，16 x 16的话就加8即可或者夸行取数值
#define line8x8_0	0
#define line8x8_1	8
#define line8x8_2	16
#define line8x8_3	24
#define line8x8_4	32
#define line8x8_5	40
#define line8x8_6	48
#define line8x8_7	56
#define line8x8_8	64
#define line8x8_9	72
#define line8x8_10	80
#define line8x8_11	88
#define line8x8_12	96
#define line8x8_13	104
#define line8x8_14	112
#define line8x8_15	120
#define line8x8_16	128
#define line8x8_17	136
#define line8x8_18	144
#define line8x8_19	152
#define line8x8_20	160
#define line8x8_21	168
#define line8x8_22	176
#define line8x8_23	184
#define line8x8_24	192
#define line8x8_25	200
#define line8x8_26	208
#define line8x8_27	216
#define line8x8_28	224
#define line8x8_29	232		//最后一列就到这
#define line8x8_30	240
#define line8x8_32	248
#define line8x8_33	256
#define line8x8_34	264
#define line8x8_35	272
#define line8x8_36	280
#define line8x8_37	288
#define line8x8_38	296
#define line8x8_39	304
#define line8x8_40	312

/**********************************************************
                     寄存器地址宏定义
**********************************************************/

#define Drive_Code_Read                         0x00
#define Drive_Output_Control1                  0x01
#define LCD_Drive_Control                       0x02
#define Entry_Mode                              0x03
#define Resize_Control                          0x04
#define Display_Control1                        0x07
#define Display_Control2                        0x08
#define Display_Control3                        0x09
#define Display_Control4                        0x0a
#define RGB_Display_Interface_Control1          0x0c
#define Frame_Maker_Position                    0x0d
#define RGB_Display_Interface_Control2          0x0f

#define Power_Control1                          0x10
#define Power_Control2                          0x11
#define Power_Control3                          0x12
#define Power_Control4                          0x13

#define Horizontal_GRAM_Address_Set             0x20
#define Vertical_GRAM_Address_Set               0x21
#define Write_Data_to_GRAM                      0x22
#define Power_Control7                          0x29
#define Frame_Rate_and_Color_Control            0x2b

#define Gamma_Control1                          0x30
#define Gamma_Control2                          0x31
#define Gamma_Control3                          0x32
#define Gamma_Control4                          0x35
#define Gamma_Control5                          0x36
#define Gamma_Control6                          0x37
#define Gamma_Control7                          0x38
#define Gamma_Control8                          0x39
#define Gamma_Control9                          0x3c
#define Gamma_Control10                          0x3d

#define Horizontal_Address_Start_Position       0x50
#define Horizontal_Address_End_Position         0x51
#define Vertical_Address_Start_Position         0x52
#define Vertical_Address_End_Position           0x53

#define Drive_Output_Control2                   0x60
#define Base_Image_Display_Control              0x61
#define Vertical_Scroll_Control                 0x6a

#define Partical_Image_Display_Position1         0x80
#define Partical_Image_Area_StartLine1         0x81
#define Partical_Image_Area_EndLine1             0x82
#define Partical_Image_Display_Position2         0x83
#define Partical_Image_Area_StartLine2         0x84
#define Partical_Image_Area_EndLine2             0x85

#define Panel_Interface_Control1                0x90
#define Panel_Interface_Control2                0x92
#define Panel_Interface_Control4                0x95

#define OTP_VCM_Programming_Control             0xa1
#define OTP_VCM_Status_and_Enable               0xa2
#define OTP_Programming_ID_Key                  0xa5



/**********************************************************
                       外部功能函数                      
**********************************************************/

void LCDTFT_Init(void);    //LCD初始化

/* 清屏相关API */
void LCD_Clear(u16 color);    //LCD清屏函数
void LCD_Area_Color(u16 x1,u16 y1,u16 x2,u16 y2,u16 color); //LCD指定区域填充颜色函数

/* 画图相关API */
u16 LCD_ReadPointColor(u16 x,u16 y);	//读指定点的颜色值
void LCD_DrawPoint(u16 x,u16 y);	//LCD画点函数
void LCD_DrawPoint_Color(u16 x,u16 y,u16 color);	//LCD画点函数，颜色值随设定写入（快速画点）
void LCD_Draw_Line(u16 x1,u16 y1,u16 x2,u16 y2);      //LCD画线函数
void LCD_Draw_Quad(u16 x1,u16 y1,u16 x2,u16 y2);      //LCD画矩形函数
void LCD_Draw_Circle(u16 x0,u16 y0,u16 r);      //LCD中点法画圆函数

/* 显示字符、汉字相关API */
void LCD_Draw_Char(u16 x,u16 y,u8 ch,u8 size,u8 mode);  //LCD指定位置显示一个字符函数
void LCD_Draw_Char_BK(u16 x,u16 y,u8 ch,u8 size,u16 charcolor,u16 bkcolor);     //LCD指定位置显示一个字符字符，背景颜色随机设定
void LCD_Draw_GB_BK(u16 x,u16 y,u8 *ch,u8 fontsize,u16 charcolor,u16 bkcolor);    //LCD显示一个汉字，背景颜色随机设定
void LCD_Display_String_BK(u16 x,u16 y,u8* ch,u8 size,u16 charcolor,u16 bkcolor);     //LCD显示汉字或字符、背景颜色随机设定

/* 显示数字有关API */
void LCD_Display_Array_No0(u16 x,u16 y,u32 arr,u8 size,u8 sum,u8 mode);   //LCD显示数字，高位为0时不显示
void LCD_Display_Array_Yes0(u16 x,u16 y,u32 arr,u8 size,u8 sum,u8 mode);  //LCD显示数字，高位为0时显示0
void LCD_Display_String_Num(u16 x,u16 y,u8 *ch,u8 fontsize,u16 chlen,u16 sum,u16 charcolor,u16 bkcolor);	//LCD显示一个数字串函数，显示位数、字体、背景颜色随机设定

#endif

