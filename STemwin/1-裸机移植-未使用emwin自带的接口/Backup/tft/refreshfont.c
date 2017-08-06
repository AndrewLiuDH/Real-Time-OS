/**********************************************************
* @ File name -> refreshfont.c
* @ Version   -> V1.0
* @ Date      -> 05-03-2014
* @ Brief     -> 字库检测和更新函数

 V1.
* @ Revise    ->
**********************************************************/

#include "refreshfont.h"

/**********************************************************
                      定义相关变量
**********************************************************/

u32 FONT_InfoADDR = (1024*6 + 500) * 1024;	//6MB + 500KB开始是字库信息和字库数据存储地方

_Font_Info ft_info;	//字库基本信息结构体

//在SD卡中的路径
const u8 *GBK12_SDPATH="0:/SYSTEM/FONT/GBK12.FON";	//GBK12.FON字库存放位置
const u8 *GBK16_SDPATH="0:/SYSTEM/FONT/GBK16.FON";	//GBK16.FON字库存放位置
const u8 *UNIGBK_SDPATH="0:/SYSTEM/FONT/UNIGBK.BIN";	//UNIGBK.BIN字库存放位置
const u8 *Hz65k_SDPATH="0:/SYSTEM/FONT/Hz65k.o";	//国标GB2312字库

//在SPI-FLASH中的路径
const u8 *GBK12_FLASHPATH="1:/SYSTEM/FONT/GBK12.FON";	//GBK12.FON字库存放位置
const u8 *GBK16_FLASHPATH="1:/SYSTEM/FONT/GBK16.FON";	//GBK16.FON字库存放位置
const u8 *UNIGBK_FLASHPATH="1:/SYSTEM/FONT/UNIGBK.BIN";	//UNIGBK.BIN字库存放位置
const u8 *Hz65k_FLASHPATH="1:/SYSTEM/FONT/Hz65k.o";	//国标GB2312字库

/**********************************************************
* 函数功能 ---> 显示更新字库进度百分比
* 入口参数 ---> (x, y)：显示坐标
*               size：显示字体大小
*               ftsize：更新当前字体大小
*               filesize：整个字库大小
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
void Font_updsche(u16 x, u16 y, u8 size, u32 ftsize, u32 filesize)
{
	float pros;
	u8 i=130;

	pros = (float)(ftsize / filesize);	//得到百分比
	pros *= 100;

	if(i != pros)
	{
		i = pros;
		if(i >= 100)	i = 100;
		LCD_Display_Array_No0(x, y, i, 3, 0);	//显示百分比
		LCD_Display_String_BK(x+3*size/2, y, "%", size, RED, WHITH);
	}
}
/**********************************************************
* 函数功能 ---> 更新字库数据
* 入口参数 ---> (x, y)：显示坐标
*               size：显示字体大小
*               ftsize：更新当前字体大小
*               filesize：整个字库大小
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
