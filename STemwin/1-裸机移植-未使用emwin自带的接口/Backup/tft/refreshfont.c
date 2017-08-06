/**********************************************************
* @ File name -> refreshfont.c
* @ Version   -> V1.0
* @ Date      -> 05-03-2014
* @ Brief     -> �ֿ���͸��º���

 V1.
* @ Revise    ->
**********************************************************/

#include "refreshfont.h"

/**********************************************************
                      ������ر���
**********************************************************/

u32 FONT_InfoADDR = (1024*6 + 500) * 1024;	//6MB + 500KB��ʼ���ֿ���Ϣ���ֿ����ݴ洢�ط�

_Font_Info ft_info;	//�ֿ������Ϣ�ṹ��

//��SD���е�·��
const u8 *GBK12_SDPATH="0:/SYSTEM/FONT/GBK12.FON";	//GBK12.FON�ֿ���λ��
const u8 *GBK16_SDPATH="0:/SYSTEM/FONT/GBK16.FON";	//GBK16.FON�ֿ���λ��
const u8 *UNIGBK_SDPATH="0:/SYSTEM/FONT/UNIGBK.BIN";	//UNIGBK.BIN�ֿ���λ��
const u8 *Hz65k_SDPATH="0:/SYSTEM/FONT/Hz65k.o";	//����GB2312�ֿ�

//��SPI-FLASH�е�·��
const u8 *GBK12_FLASHPATH="1:/SYSTEM/FONT/GBK12.FON";	//GBK12.FON�ֿ���λ��
const u8 *GBK16_FLASHPATH="1:/SYSTEM/FONT/GBK16.FON";	//GBK16.FON�ֿ���λ��
const u8 *UNIGBK_FLASHPATH="1:/SYSTEM/FONT/UNIGBK.BIN";	//UNIGBK.BIN�ֿ���λ��
const u8 *Hz65k_FLASHPATH="1:/SYSTEM/FONT/Hz65k.o";	//����GB2312�ֿ�

/**********************************************************
* �������� ---> ��ʾ�����ֿ���Ȱٷֱ�
* ��ڲ��� ---> (x, y)����ʾ����
*               size����ʾ�����С
*               ftsize�����µ�ǰ�����С
*               filesize�������ֿ��С
* ������ֵ ---> none
* ����˵�� ---> none
**********************************************************/
void Font_updsche(u16 x, u16 y, u8 size, u32 ftsize, u32 filesize)
{
	float pros;
	u8 i=130;

	pros = (float)(ftsize / filesize);	//�õ��ٷֱ�
	pros *= 100;

	if(i != pros)
	{
		i = pros;
		if(i >= 100)	i = 100;
		LCD_Display_Array_No0(x, y, i, 3, 0);	//��ʾ�ٷֱ�
		LCD_Display_String_BK(x+3*size/2, y, "%", size, RED, WHITH);
	}
}
/**********************************************************
* �������� ---> �����ֿ�����
* ��ڲ��� ---> (x, y)����ʾ����
*               size����ʾ�����С
*               ftsize�����µ�ǰ�����С
*               filesize�������ֿ��С
* ������ֵ ---> none
* ����˵�� ---> none
**********************************************************/
