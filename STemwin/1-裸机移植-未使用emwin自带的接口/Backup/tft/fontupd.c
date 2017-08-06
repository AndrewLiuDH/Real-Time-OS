/**********************************************************
* @ File name -> fontupd.c
* @ Version   -> V1.0
* @ Date      -> 13-02-2014
* @ Brief     -> 字库相关函数

 V1.
* @ Revise    ->
**********************************************************/

#include "fontupd.h"

/* 头文件加载定义 */
#if ((_USER_SD_FON==1)||(_USER_BufforFLASH==2))	//使用SD卡字库或者FLASH文件系统字库的话要添加文件系统操作

	#include "ff.h"

#endif

//=========================================================
#if !_USER_SD_FON	//在不使用SD卡作为字库的时候才使用SPI-FLASH或者数组方式
//=========================================================
#if _USER_BufforFLASH==1	//使用SPI-FLASH字库时候用到头文件

	#include "w25q64.h"
	#include "refreshfont.h"	//检测、更新字库函数

#endif	//end _USER_BufforFLASH
//=========================================================

/* 没有定义SD或者FLASH的话则使用数组方式，缺点是太占用MCU程序空间 */

//#include "HzLib_65k.h"	//GB2312国标中文字库
unsigned char const HzLib[] = {
	/* 啊 */
	/*------------------------------------------------------------------------------
	;  源文件 / 文字 : 啊
	;  宽×高（像素）: 15×16
	------------------------------------------------------------------------------*/
	0x00,0x00,0xF7,0x7E,0x95,0x04,0x95,0x04,0x96,0x74,0x96,0x54,0x95,0x54,0x95,0x54,
	0x95,0x54,0xF5,0x54,0x97,0x74,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x14,0x04,0x08,
	/* 阿 */
      /*------------------------------------------------------------------------------
	;  源文件 / 文字 : 阿
	;  宽×高（像素）: 15×16
	------------------------------------------------------------------------------*/
	0x00,0x00,0xF7,0xFC,0x90,0x10,0xA0,0x10,0xC0,0x10,0xA7,0xD0,0x94,0x50,0x94,0x50,
	0x94,0x50,0xD7,0xD0,0xA0,0x10,0x80,0x10,0x80,0x10,0x80,0x10,0x80,0x50,0x80,0x20
	};

/* 结束头文件加载定义 */

#endif	//end _USER_SD_FON

/**********************************************************
                        定义字库路径
**********************************************************/

#if _USER_SD_FON	//使用SD卡字库

//在SD卡中的路径
const uint8_t *GBK12_SDUPD="0:/SYSTEM/FONT/GBK12.FON";	//GBK12.FON字库存放位置
const uint8_t *GBK16_SDUPD="0:/SYSTEM/FONT/GBK16.FON";	//GBK16.FON字库存放位置
const uint8_t *UNIGBK_SDUPD="0:/SYSTEM/FONT/UNIGBK.BIN";	//UNIGBK.BIN字库存放位置
const uint8_t *Hz65k_SDUPD="0:/SYSTEM/FONT/Hz65k.o";	//国标GB2312字库

#else	//使用FLASH或者刷入MCU内部的字库数组

#if _USER_BufforFLASH==2	//使用SPI-FLASH文件系统内的字库

//在SPI-FLASH中的路径
const uint8_t *GBK12_FLASHUPD="1:/SYSTEM/FONT/GBK12.FON";	//GBK12.FON字库存放位置
const uint8_t *GBK16_FLASHUPD="1:/SYSTEM/FONT/GBK16.FON";	//GBK16.FON字库存放位置
const uint8_t *UNIGBK_FLASHUPD="1:/SYSTEM/FONT/UNIGBK.BIN";	//UNIGBK.BIN字库存放位置
const uint8_t *Hz65k_FLASHUPD="1:/SYSTEM/FONT/Hz65k.o";	//国标GB2312字库

#endif	//end _USER_BufforFLASH
#endif	//end _USER_SD_FON


//=========================================================
#if _USER_SD_FON	//如果定义了，则使用SD卡作为中文字库使用
//=========================================================
/**********************************************************
* 函数功能 ---> 取得中文字库数据
* 入口参数 ---> *code：字串开始地址
*               *c_HanZibuff：读取到的字模数据缓存
*               size：字体大小
* 返回数值 ---> none
* 功能说明 ---> 调用此函数前必须先注册FatFs工作区并挂载SD卡
**********************************************************/
void Get_Hzlib(unsigned char *ch, unsigned char *c_HanZibuff,u8 size)
{
	FIL filefont;	//文件
	UINT brf;		//读写变量
	FRESULT res;	//操作函数应答
					  
	unsigned long foffset;	//汉字在字库的首地址
/*	unsigned char i;
	unsigned char qh,ql;
	 
	qh = *ch;
	ql = *(ch+1);
	if(qh<0x81||ql<0x40||ql==0xff||qh==0xff)//非 常用汉字
	{   		    
	    for(i=0;i<(size*2);i++)	*c_HanZibuff++ = 0x00;//填充满格
	    return; //结束访问
	}
	//计算公式
	//当 GBKL<0X7F 时：Hp=((GBKH-0x81)*190+GBKL-0X40)*(size*2)
	//当 GBKL>0X80 时：Hp=((GBKH-0x81)*190+GBKL-0X41)*(size*2)          
	if(ql<0x7f)	ql -= 0x40;//注意!
	else	ql -= 0x41;
	qh -= 0x81;
	//计算汉字地址偏移   
	foffset = (unsigned long)((190*qh+ql)*(size*2));//得到字库中的字节偏移量

	if(size == 16)	res = f_open (&filefont, (const TCHAR*)GBK16_SDUPD, FA_OPEN_EXISTING | FA_READ);	//不可以进行写操作
	else	res = f_open (&filefont, (const TCHAR*)GBK12_SDUPD, FA_OPEN_EXISTING | FA_READ);	//不可以进行写操作
*/
	foffset = (((*ch) - 176)*94 + ((*(ch+1)) - 161))*32;	//计算得到文字在字库中的偏移地址
	
	res = f_open (&filefont, (const TCHAR*)Hz65k_SDUPD, FA_OPEN_EXISTING | FA_READ);	//不可以进行写操作
	if(res != FR_OK)	//打开失败
	{	printf("open SD FON res:%d\r\n", res);
		f_close(&filefont);	//关闭文件
		return;	//结束访问
	}
	else	//打开文件成功
	{
//		res = f_lseek(&filefont, foffset);	//得到文件指针偏移量
		res = f_lseek(&filefont, foffset+0x112);	//得到文件指针偏移量
		res = f_read(&filefont, c_HanZibuff, 32, &brf);	//读取32字节数据	
		res = f_close(&filefont);	//关闭文件
	}
}

//=========================================================
#else	//否则直接使用定义的数组
//=========================================================

/**********************************************************
* 函数功能 ---> 取得中文字库数据
* 入口参数 ---> *code：字串开始地址
*               *c_HanZibuff：读取到的字模数据缓存
*               size：字体大小
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
void Get_Hzlib(unsigned char *ch, unsigned char *c_HanZibuff,u8 size)
{
#if _USER_BufforFLASH==0	//使用刷入MCU内部的字库数组

	unsigned char i;
	u32 address=0;
	if(size == 16)
	{
		address = ( ((*ch)-176)*94 + ((*(ch+1))-161) )*32;
		for(i = 0;i < address;i++)
		{
			c_HanZibuff[i] = HzLib[i];
		}
	}

#elif _USER_BufforFLASH==1	//否则使用外部SPI-FLASH做字库芯片

	unsigned char i;
	unsigned char qh,ql;					  
	unsigned long foffset;
	 
	qh = *ch;
	ql = *(++ch);
	if(qh<0x81||ql<0x40||ql==0xff||qh==0xff)//非 常用汉字
	{   		    
	    for(i=0;i<(size*2);i++)	*c_HanZibuff++ = 0x00;//填充满格
	    return; //结束访问
	}
	//计算公式
	//当 GBKL<0X7F 时：Hp=((GBKH-0x81)*190+GBKL-0X40)*(size*2)
	//当 GBKL>0X80 时：Hp=((GBKH-0x81)*190+GBKL-0X41)*(size*2)
	          
	if(ql<0x7f)	ql -= 0x40;//注意!
	else	ql -= 0x41;
	qh -= 0x81;   
	foffset=((unsigned long)190*qh+ql)*(size*2);//得到字库中的字节偏移量  		  

	if(size==16)SPI_Flash_Read(c_HanZibuff,foffset+ftinfo.f16addr,32);
	else SPI_Flash_Read(c_HanZibuff,foffset+ftinfo.f12addr,24);

#elif _USER_BufforFLASH==2	//使用SPI-FLASH文件系统内字库

	FIL filefont;	//文件
	UINT brf;		//读写变量
	FRESULT res;	//操作函数应答
					  
	unsigned long foffset;	//汉字在字库的首地址
/*	unsigned char i;
	unsigned char qh,ql;
	 
	qh = *ch;
	ql = *(ch+1);
	if(qh<0x81||ql<0x40||ql==0xff||qh==0xff)//非 常用汉字
	{   		    
	    for(i=0;i<(size*2);i++)	*c_HanZibuff++ = 0x00;//填充满格
	    return; //结束访问
	}
	//计算公式
	//当 GBKL<0X7F 时：Hp=((GBKH-0x81)*190+GBKL-0X40)*(size*2)
	//当 GBKL>0X80 时：Hp=((GBKH-0x81)*190+GBKL-0X41)*(size*2)          
	if(ql<0x7f)	ql -= 0x40;//注意!
	else	ql -= 0x41;
	qh -= 0x81;
	//计算汉字地址偏移   
	foffset = (unsigned long)((190*qh+ql)*(size*2));//得到字库中的字节偏移量

	if(size == 16)	res = f_open (&filefont, (const TCHAR*)GBK16_SDUPD, FA_OPEN_EXISTING | FA_READ);	//不可以进行写操作
	else	res = f_open (&filefont, (const TCHAR*)GBK12_SDUPD, FA_OPEN_EXISTING | FA_READ);	//不可以进行写操作
*/

	foffset = (((*ch) - 176)*94 + ((*(ch+1)) - 161))*32;	//计算得到文字在字库中的偏移地址
	
	res = f_open (&filefont, (const TCHAR*)Hz65k_FLASHUPD, FA_OPEN_EXISTING | FA_READ);	//不可以进行写操作
	if(res != FR_OK)	//打开失败
	{	printf("open SD FON res:%d\r\n", res);
		f_close(&filefont);	//关闭文件
		return;	//结束访问
	}
	else	//打开文件成功
	{
//		res = f_lseek(&filefont, foffset);	//得到文件指针偏移量
		res = f_lseek(&filefont, foffset+0x112);	//得到文件指针偏移量
		res = f_read(&filefont, c_HanZibuff, 32, &brf);	//读取32字节数据	
		res = f_close(&filefont);	//关闭文件
	}

#endif	//end _USER_BufforFLASH
	
}

//=========================================================
#endif	//end _USER_SD_FON
//========================================================= 

