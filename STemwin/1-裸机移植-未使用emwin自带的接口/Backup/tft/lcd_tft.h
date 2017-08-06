/**********************************************************
* @ File name -> lcd_tft.h
* @ Version   -> V1.0.2
* @ Date      -> 02-25-2014
* @ Brief     -> TFT�����ϲ�Ӧ�ú���ͷ�ļ�

 V1.0.1
* @ Revise    -> A����LCD_Display_String_BK1���������޸�
*                B���޸�Ϊ��λ����ʾѡ��Ĭ�ϲ�������ʾ
*                C�����������޸�ΪLCD_Display_String_Num

 V1.0.2
* @ Revise    -> �������ִ���ʾ����Сbug��ԭ���ǴӸ�λ��ʼ������ʾNλ�����ַ�
*                ����������λ��ʼ������ʾNλ�����ַ�
*                ������ʾ00336.85���������ʾ8λ��ȫ����ʾ����λΪ0Ҳ��ʾ������ʾ5Ϊ����36.85��С���㶼�����ʾλ����
**********************************************************/

#ifndef _lcd_tft_h_
#define _lcd_tft_h_

/**********************************************************
                     �ⲿ����ͷ�ļ�                        
**********************************************************/

#include "lcd_drive.h"	//LCD�����ײ�ӿ�Ӧ�ú���

#include "fontupd.h"	//������ģ������Դ����

/**********************************************************
                ����TFT_LCD������ؽṹ
**********************************************************/

//LCD�����ṹ
typedef struct
{
	u16 width;	//LCD��ʾ���
	u16 height;	//LCD��ʾ�߶�
	u16 ID;	//LCD�ͺ�
	u8 dir;	//LCD�Ǻ�����ʾ����������ʾ��0��Ϊ������1��Ϊ����
	u8 wgramcmd;	//дGRAM����
	u8 setxcmd;	//����x����
	u8 setycmd;	//����y����
}lcd_dev;

extern lcd_dev LCD_Manage;

extern u16 Point_color;	//Ĭ�ϵ����ɫΪ��ɫ
extern u16 Back_color;	//Ĭ�ϱ�����ɫΪ��ɫ

/**********************************************************
                     ����Ͳ����궨��
**********************************************************/

//LCD����궨��
#define Display_OFF			0x28	//�ر���ʾ��
#define Display_ON			0x29	//����ʾ��

#define LCD_EnterSleep		0x10	//LCD����˯��ģʽ
#define LCD_ExitSleep		0x11	//LCD�˳�˯��ģʽ

#define LCD_SofReset		0x01	//LCD�����λ��������

//LCDɨ�跽����
#define L2R_U2D			0	//�����ң����ϵ���
#define L2R_D2U			1	//�����ң����µ���
#define R2L_U2D			2	//���ҵ��󣬴��ϵ���
#define R2L_D2U			3	//���ҵ��󣬴��µ���

#define U2D_L2R			4	//���ϵ��£�������
#define U2D_R2L			5	//���ϵ��£����ҵ���
#define D2U_L2R			6	//���µ��ϣ�������
#define D2U_R2L			7	//���µ��ϣ����ҵ���

#define DEF_Scan_DIR	L2R_U2D	//Ĭ��ɨ�跽��Ϊ�����ң����ϵ���	

//������ɫֵ
#define RED			0xf800	//��ɫ
#define BLUE		0x01cf	//��ɫ	0x001f	
#define WHITE		0xffff	//��ɫ
#define BLACK		0x0000	//��ɫ
#define GREEN		0x07e0	//��ɫ
#define YELLOW		0xffe0	//��ɫ
#define ORANGE		0x05bf	//��ɫ
#define GREY		0xefbd	//��ɫ	0xf7de

#define BLUE2		0x051f
#define MAGENTA		0xf81f	//���
#define CYAN		0x7fff	//����ɫ

//�������У�16 x 16�Ļ��ͼ�8���ɻ��߿���ȡ��ֵ
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
#define line8x8_29	232		//���һ�о͵���
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
                     �Ĵ�����ַ�궨��
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
                       �ⲿ���ܺ���                      
**********************************************************/

void LCDTFT_Init(void);    //LCD��ʼ��

/* �������API */
void LCD_Clear(u16 color);    //LCD��������
void LCD_Area_Color(u16 x1,u16 y1,u16 x2,u16 y2,u16 color); //LCDָ�����������ɫ����

/* ��ͼ���API */
u16 LCD_ReadPointColor(u16 x,u16 y);	//��ָ�������ɫֵ
void LCD_DrawPoint(u16 x,u16 y);	//LCD���㺯��
void LCD_DrawPoint_Color(u16 x,u16 y,u16 color);	//LCD���㺯������ɫֵ���趨д�루���ٻ��㣩
void LCD_Draw_Line(u16 x1,u16 y1,u16 x2,u16 y2);      //LCD���ߺ���
void LCD_Draw_Quad(u16 x1,u16 y1,u16 x2,u16 y2);      //LCD�����κ���
void LCD_Draw_Circle(u16 x0,u16 y0,u16 r);      //LCD�е㷨��Բ����

/* ��ʾ�ַ����������API */
void LCD_Draw_Char(u16 x,u16 y,u8 ch,u8 size,u8 mode);  //LCDָ��λ����ʾһ���ַ�����
void LCD_Draw_Char_BK(u16 x,u16 y,u8 ch,u8 size,u16 charcolor,u16 bkcolor);     //LCDָ��λ����ʾһ���ַ��ַ���������ɫ����趨
void LCD_Draw_GB_BK(u16 x,u16 y,u8 *ch,u8 fontsize,u16 charcolor,u16 bkcolor);    //LCD��ʾһ�����֣�������ɫ����趨
void LCD_Display_String_BK(u16 x,u16 y,u8* ch,u8 size,u16 charcolor,u16 bkcolor);     //LCD��ʾ���ֻ��ַ���������ɫ����趨

/* ��ʾ�����й�API */
void LCD_Display_Array_No0(u16 x,u16 y,u32 arr,u8 size,u8 sum,u8 mode);   //LCD��ʾ���֣���λΪ0ʱ����ʾ
void LCD_Display_Array_Yes0(u16 x,u16 y,u32 arr,u8 size,u8 sum,u8 mode);  //LCD��ʾ���֣���λΪ0ʱ��ʾ0
void LCD_Display_String_Num(u16 x,u16 y,u8 *ch,u8 fontsize,u16 chlen,u16 sum,u16 charcolor,u16 bkcolor);	//LCD��ʾһ�����ִ���������ʾλ�������塢������ɫ����趨

#endif

