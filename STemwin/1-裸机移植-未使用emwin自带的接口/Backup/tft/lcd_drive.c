/**********************************************************
* @ File name -> lcd_drive.c
* @ Version   -> V1.0.2
* @ Date      -> 03-09-2014
* @ Brief     -> TFT�����ײ���������

 V1.0.1
* @ Revise    -> ��д���GPIO����LCD�����ĳ�ʼ������

 V1.0.2
* @ Revise    -> �Ż���ֲʱ�ķ����ԣ���������ֻ���޸������ĺ궨�弴��
**********************************************************/

#include "lcd_drive.h"

/**********************************************************
                 |                        |
                 |  ���������ֲʱ�޸���  |
                \|/                      \|/
**********************************************************/

//=========================================================
//����ʹ����ͨGPIO����LCD
//=========================================================

/**********************************************************
* �������� ---> ͨѶ�����ƶ˿ڳ�ʼ��
* ��ڲ��� ---> none
* ������ֵ ---> none
* ����˵�� ---> none
**********************************************************/
void LCD_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE,ENABLE);

	//LCD��������
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
* �������� ---> д���ݵ�����
* ��ڲ��� ---> val��Ҫд�������
* ������ֵ ---> none
* ����˵�� ---> none
**********************************************************/
void LCD_Write_Bus(u16 val)
{
/* ʹ��16bits�������߿��ʱ���߼�����ʱ�� */

//	LCD_BUS_OUT16();	//��������Ϊ�������
	LCD_RS_SET;
	LCD_CS_CLR;
	DATAOUT(val);
	LCD_WR_CLR;
	LCD_WR_SET;
	LCD_CS_SET;
}
/**********************************************************
* �������� ---> ��ȡLCD����
* ��ڲ��� ---> none
* ������ֵ ---> ��ȡ��������
* ����˵�� ---> none
**********************************************************/
u16 LCD_Read_Data(void)
{
	u16 temp=0;	//�������ݻ���

/* ʹ��16bits�������߿��ʱ���߼�����ʱ�� */
	LCD_BUS_IN16();

	LCD_RS_SET;
	LCD_CS_CLR;
	//��ȡ����(���Ĵ���ʱ,������Ҫ��2��)
	LCD_RD_CLR;
	temp=DATAIN;  
	LCD_RD_SET;
	LCD_CS_SET; 

	LCD_BUS_OUT16();
	
	return temp;	//���ض�ȡ��������     
}

/**********************************************************
                /|\                   /|\
                 |   ������ֲʱ�޸���   |
				 |                     |
**********************************************************/

/**********************************************************
* �������� ---> ���⡢��λ���ƶ˿ڳ�ʼ��
* ��ڲ��� ---> none
* ������ֵ ---> none
* ����˵�� ---> none
**********************************************************/
void LCD_RST_BL_Init(void)
{
	/******����ܽ�******/
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE,ENABLE);

	//LCD��������
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
        ��������ص�Ӧ�õ���API�������Ǳ�Ҫ�ɲ��޸�
**********************************************************/


//=========================================================
//����ʹ����ͨGPIO����LCD                  ********************************************** BULA
//=========================================================

/**********************************************************
* �������� ---> д���ݵ�LCD�Ĵ�������Ҫд�����Ĵ�����ַ��
* ��ڲ��� ---> reg_val��Ҫд�������
* ������ֵ ---> none
* ����˵�� ---> none
**********************************************************/
void LCD_Write_Register(u16 reg_val)
{
	LCD_RS_CLR;		//д��ַ  
 	LCD_CS_CLR; 
	DATAOUT(reg_val);
	LCD_WR_CLR;
	LCD_WR_SET;
 	LCD_CS_SET;	
}
/**********************************************************
* �������� ---> д���ݵ�LCD RAM
* ��ڲ��� ---> dat��Ҫд�������
* ������ֵ ---> none
* ����˵�� ---> none
**********************************************************/
void LCD_Write_Data(u16 dat)
{
	LCD_WR_Data(dat);
}
/**********************************************************
* �������� ---> ��LCDĳ���Ĵ���д������
* ��ڲ��� ---> reg��Ҫд�����ݵļĴ�����ַ
*               dat��Ҫд�������
* ������ֵ ---> none
* ����˵�� ---> none
**********************************************************/
void LCD_WriteRegisterData(u16 reg, u16 dat)
{
	LCD_Write_Register(reg);
	LCD_WR_Data(dat);
}
/**********************************************************
* �������� ---> ��ȡLCDĳ���Ĵ�����ֵ
* ��ڲ��� ---> reg��Ҫ��ȡ���ݵļĴ�����ַ
* ������ֵ ---> �Ĵ�����ֵ
* ����˵�� ---> none
**********************************************************/
u16 LCD_ReadRegisterData(u16 reg)
{
	LCD_Write_Register(reg);
	return LCD_Read_Data();
}

//=========================================================
//=========================================================

