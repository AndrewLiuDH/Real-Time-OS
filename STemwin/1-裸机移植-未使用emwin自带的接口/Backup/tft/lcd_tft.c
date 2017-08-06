/**********************************************************
* @ File name -> lcd_tft.c
* @ Version   -> V1.0.2
* @ Date      -> 02-25-2014
* @ Brief     -> TFT�����ϲ�Ӧ�ú���

 V1.0.1
* @ Revise    -> A����LCD_Display_String_BK1���������޸�
*                B���޸�Ϊ��λ����ʾѡ��Ĭ�ϲ�������ʾ
*                C�����������޸�ΪLCD_Display_String_Num

 V1.0.2
* @ Revise    -> �������ִ���ʾ����Сbug��ԭ���ǴӸ�λ��ʼ������ʾNλ�����ַ�
*                ����������λ��ʼ������ʾNλ�����ַ�
*                ������ʾ00336.85���������ʾ8λ��ȫ����ʾ����λΪ0Ҳ��ʾ������ʾ5Ϊ����36.85��С���㶼�����ʾλ����
**********************************************************/

#include "lcd_tft.h"

/**********************************************************
                        ����ֿ�
**********************************************************/

#include "fonts.h"	//ASCII�ַ����ֿ�

/**********************************************************
                     ������ر�������
**********************************************************/

lcd_dev LCD_Manage;	//����LCD����ȫ�ֱ���

u16 Point_color = RED;	//Ĭ�ϵ����ɫ
u16 Back_color = BLACK;	//Ĭ�ϱ�����ɫ

/**********************************************************
* �������� ---> ��ȡLCD��Ҫ����ʱ
* ��ڲ��� ---> i����Ҫ��ʱ��ֵ
* ������ֵ ---> none
* ����˵�� ---> none
**********************************************************/
void LCD_Delay(u8 i)
{
	while(i--);
}
/**********************************************************
* �������� ---> BGR��ʽת��ΪRGB��ʽ
* ��ڲ��� ---> bgr_color����Ҫת����BGR��ɫֵ
* ������ֵ ---> ���ص���RGBֵ
* ����˵�� ---> 93xx����ֵΪBGR,д��ֵRGB
**********************************************************/
u16 LCD_BGR2RGB(u16 bgr_color)
{
	u16 r,g,b,rgb;

	b = (bgr_color>>0)&0x1f;
	g = (bgr_color>>5)&0x3f;
	r = (bgr_color>>11)&0x1f;	 
	rgb = (b<<11) + (g<<5) + (r<<0);		 

	return rgb;
}
/**********************************************************
* �������� ---> LCD������ʾ
* ��ڲ��� ---> none
* ������ֵ ---> none
* ����˵�� ---> none
**********************************************************/
void LCD_Display_ON(void)
{
      LCD_Write_Register(Display_ON);	//�ر���ʾ
}
/**********************************************************
* �������� ---> LCD�ر���ʾ
* ��ڲ��� ---> none
* ������ֵ ---> none
* ����˵�� ---> none
**********************************************************/
void LCD_Display_OFF(void)
{
      LCD_Write_Register(Display_OFF);	//�ر���ʾ
}
/**********************************************************
* �������� ---> LCD����˯��
* ��ڲ��� ---> none
* ������ֵ ---> none
* ����˵�� ---> none
**********************************************************/
void LCD_Enter_Sleep(void)
{
	LCD_Write_Register(LCD_EnterSleep);	//����˯��
}
/**********************************************************
* �������� ---> LCD�˳�˯��
* ��ڲ��� ---> none
* ������ֵ ---> none
* ����˵�� ---> none
**********************************************************/
void LCD_Exit_Sleep(void)
{
	LCD_Write_Register(LCD_ExitSleep);	//�˳�˯��
}
/**********************************************************
* �������� ---> LCDд��GRAM�����
* ��ڲ��� ---> none
* ������ֵ ---> none
* ����˵�� ---> none
**********************************************************/
void LCD_WriteGRAM(void)
{
	LCD_Write_Register(LCD_Manage.wgramcmd);
}
/**********************************************************
* �������� ---> ����LCD��ʾλ��
* ��ڲ��� ---> (x,y): ����ԭ��
* ������ֵ ---> none
* ����˵�� ---> none
**********************************************************/
void LCD_Set_xy(u16 x,u16 y)
{
      if((x > LCD_Manage.width) || (y > LCD_Manage.height)) return;     //������Χ��ֱ���˳�
      //û������Χ������

      LCD_Write_Register(LCD_Manage.setxcmd);	//����x 
      LCD_Write_Data(x >> 8); 
      LCD_Write_Data(x & 0xff);	 
      LCD_Write_Register(LCD_Manage.setycmd);	//����y
      LCD_Write_Data(y >> 8); 
      LCD_Write_Data(y & 0xff);

/*      
      if(LCD_Manage.dir == 1) //������ʾ
      {
            x = LCD_Manage.width - x - 1;      //��תx,y
            
            LCD_WriteRegisterData(LCD_Manage.setxcmd, x);   //����x����ֵ
            LCD_WriteRegisterData(LCD_Manage.setycmd, y);   //����y����ֵ
      }
      else  //������ʾ
      {
            if(LCD_Manage.ID == 0x9341)
            {
                  LCD_Write_Register(LCD_Manage.setxcmd);	//����x 
                  LCD_Write_Data(x >> 8); 
                  LCD_Write_Data(x & 0xff);	 
                  LCD_Write_Register(LCD_Manage.setycmd);	//����y
                  LCD_Write_Data(y >> 8); 
                  LCD_Write_Data(y & 0xff);
            }
            LCD_WriteRegisterData(LCD_Manage.setxcmd, x);   //����x����ֵ
            LCD_WriteRegisterData(LCD_Manage.setycmd, y);   //����y����ֵ
      }
*/
}
/**********************************************************
* �������� ---> �趨LCD��ʾ��ɨ�跽ʽ
* ��ڲ��� ---> dir��0��������ʾ��1��������ʾ
* ������ֵ ---> none
* ����˵�� ---> none
**********************************************************/
void LCD_Scan_DIR(u8 dire)
{
      u8 dir_reg=0;    //ɨ�跽ʽ�Ĵ���
      u16 dir_val=0;    //ɨ��Ĵ�����ֵ
      u16 temp=0; //���㻺��

      if(LCD_Manage.dir == 1) //����
      {
            switch(dire)
            {
                  //˳ʱ����ת������
                  case L2R_U2D:     //����ʱ--->�����ң����ϵ��·���
                              dire = D2U_L2R;   //�µ��ϣ�����
                              break;

                  case L2R_D2U:     //����ʱ--->�����ң����µ���
                              dire = D2U_R2L;   //�µ��ϣ��ҵ���
                              break;

                  case R2L_U2D:     //����ʱ--->���ҵ��󣬴��ϵ���
                              dire = U2D_L2R;   //�ϵ��£�����
                              break;

                  case R2L_D2U:     //����ʱ--->���ҵ��󣬴��µ���
                              dire = U2D_R2L;   //�ϵ��£��ҵ���
                              break;

                  //��ʱ����ת��֮ǰ������״̬

                  case U2D_L2R:     //����ʱ--->���ϵ��£�������
                              dire = L2R_D2U;   //���ң��µ���
                              break;

                  case U2D_R2L:     //����ʱ--->���ϵ��£����ҵ���
                              dire = L2R_U2D;   //���ң��ϵ���
                              break;

                  case D2U_L2R:     //����ʱ--->���µ��ϣ�������
                              dire = R2L_D2U;   //�ҵ����µ���
                              break;

                  case D2U_R2L:     //����ʱ--->���µ��ϣ����ҵ���
                              dire = R2L_U2D;   //�ҵ����ϵ���
                              break;
            }
      }

      
	switch(dire)
	{	//            I/D1       I/D0       AM
		case L2R_U2D:	//�����ң����ϵ���
			dir_val |= (0<<7) | (0<<6) | (0<<5);	break;
		case L2R_D2U:	//������,���µ���
			dir_val |= (1<<7) | (0<<6) | (0<<5);	break;
		case R2L_U2D:	//���ҵ���,���ϵ���
			dir_val |= (0<<7) | (1<<6) | (0<<5);	break;
		case R2L_D2U:	//���ҵ���,���µ���
			dir_val |= (1<<7) | (1<<6) | (0<<5);	break;	 
		case U2D_L2R:	//���ϵ���,������
			dir_val |= (0<<7) | (0<<6) | (1<<5);	break;
		case U2D_R2L:	//���ϵ���,���ҵ���
			dir_val |= (0<<7) | (1<<6) | (1<<5);	break;
		case D2U_L2R:	//���µ���,������
			dir_val |= (1<<7) | (0<<6) | (1<<5);	break;
		case D2U_R2L:	//���µ���,���ҵ���
			dir_val |= (1<<7) | (1<<6) | (1<<5);	break;	 
	}
		
	dir_reg = 0x36;
	dir_val |= 0x08;//BGR   	   
	LCD_WriteRegisterData(dir_reg,dir_val);

	if(dir_val & 0x20)
	{
		if(LCD_Manage.width < LCD_Manage.height)//����X,Y
		{
			temp = LCD_Manage.width;
			LCD_Manage.width = LCD_Manage.height;
			LCD_Manage.height = temp;
		}
	}
	else  
	{
		if(LCD_Manage.width > LCD_Manage.height)//����X,Y
		{
			temp = LCD_Manage.width;
			LCD_Manage.width = LCD_Manage.height;
			LCD_Manage.height = temp;
		}
	}

	LCD_Write_Register(LCD_Manage.setxcmd); 
	LCD_Write_Data(0);LCD_Write_Data(0);	//���������
	LCD_Write_Data((LCD_Manage.width-1)>>8);LCD_Write_Data((LCD_Manage.width-1)&0XFF);	//�������յ�
	LCD_Write_Register(LCD_Manage.setycmd); 
	LCD_Write_Data(0);LCD_Write_Data(0);	//���������
	LCD_Write_Data((LCD_Manage.height-1)>>8);LCD_Write_Data((LCD_Manage.height-1)&0XFF);	//�������յ�  
}
/**********************************************************
* �������� ---> �趨LCD��ʾ����
* ��ڲ��� ---> dir��0��������ʾ��1��������ʾ
* ������ֵ ---> none
* ����˵�� ---> none
**********************************************************/
void LCD_Display_DIR(u8 dir)
{
	if(dir == 0)	//������ʾ
	{
		LCD_Manage.dir = 0;	//������ʾ
		LCD_Manage.width = 240;	//���������
		LCD_Manage.height = 320;	//���������

		LCD_Manage.wgramcmd = 0x2c;	//GRAM��ʼдָ��
		LCD_Manage.setxcmd = 0x2a;	//����x����
		LCD_Manage.setycmd = 0x2b;	//����y����
	}
	else  //������ʾ
	{
		LCD_Manage.dir = 1;	//������ʾ
		LCD_Manage.width = 320;	//���������
		LCD_Manage.height = 240;	//���������

		LCD_Manage.wgramcmd = 0x2c;	//GRAM��ʼдָ��
		LCD_Manage.setxcmd = 0x2a;	//����x����
		LCD_Manage.setycmd = 0x2b;	//����y����
 
	}
	LCD_Scan_DIR(DEF_Scan_DIR);	//Ĭ��ɨ�跽��
}
/**********************************************************
* �������� ---> ��ָ�������ɫֵ
* ��ڲ��� ---> (x,y)���������ֵ
* ������ֵ ---> ������ɫֵ
* ����˵�� ---> none
**********************************************************/
u16 LCD_ReadPointColor(u16 x,u16 y)
{
	u16 r=0,g=0,b=0;

	if((x > LCD_Manage.width) || (y > LCD_Manage.height))		return 0;	//��������곬����Χ��ֱ���˳�

	LCD_Set_xy(x, y);	//�趨��ȡ��
	LCD_Write_Register(0x2e);	//9341���Ͷ�GRAMָ��

	LCD_GRAM_IN16();
	
	LCD_RS_SET;
	LCD_CS_CLR;
	LCD_RD_CLR;
	LCD_Delay(1);
	r = DATAIN;		// �˴�Ϊ�ٶ���������Ч
	LCD_RD_SET;
	
	LCD_RD_CLR;
	LCD_Delay(1);
	r = DATAIN;		// ��ȡ��ɫֵ, �˴�Ϊ�����������Ч
	LCD_RD_SET;
	
	
	if(LCD_Manage.ID == 0x9341)	//�������Ҫ��ȡ���Σ���
	{
		LCD_RD_CLR;
		b = DATAIN;		//�������һ������
		LCD_RD_SET;
		g = (r&0xff)<<8;	//9341��һ�ζ�ȡ��������RG��ֵ����ռ8λ
	}
	LCD_CS_SET;
	LCD_BUS_OUT16();
	
	//�жϸ�ʽ������RGB��ʽ��ɫֵ
	if(LCD_Manage.ID == 0x9341)	return (((r>>11)<<11) | ((g>>10)<<5) | (b>>11));	//9341��Ҫ��ʽת��һ�·���
	else if((LCD_Manage.ID == 0x9320) || (LCD_Manage.ID == 0x9325) || (LCD_Manage.ID == 0x9328))	return r;	//ֱ�ӷ��ؼ���
	else return LCD_BGR2RGB(r);	//����ת���󷵻�
}
/**********************************************************
* �������� ---> LCD��ʼ��
* ��ڲ��� ---> none
* ������ֵ ---> none
* ����˵�� ---> none
**********************************************************/
void LCDTFT_Init()
{
	LCD_RST_BL_Init();      //��ʼ������ƿ��ƺ͸�λ�ܽſ���

    /******************************************************
	                   ���Ʒ�ʽѡ��
	******************************************************/  
	LCD_GPIO_Init();  //��ʼ��ͨѶ������GPIO

	delay_ms(50);     //�ȴ���ؼĴ�����ɳ�ʼ��
	LCD_WriteRegisterData(0x00, 0x01);    //�����λ
	delay_ms(50);

	LCD_Manage.ID = LCD_ReadRegisterData(0x0000);      //��ȡLCD�ͺ�

	if((LCD_Manage.ID < 0xff) || (LCD_Manage.ID == 0xffff) || (LCD_Manage.ID == 0x9300))
	{	//��ȡID����ȷ
		//���Զ�ȡ9341
		LCD_Write_Register(0xd3);     //���¶�ȡ
		LCD_Read_Data();  //�ٶ�һ��
		LCD_Read_Data();
		LCD_Manage.ID = LCD_Read_Data();    //��ȡ��93
		LCD_Manage.ID <<= 8;
		LCD_Manage.ID |= LCD_Read_Data();   //��ȡ41
		
		printf("\r\nLCD Drive ID is: %x\r\n",LCD_Manage.ID);      //��ӡID������
		if(LCD_Manage.ID != 0x9341)   LCD_Manage.ID = 0x9341;		// ******  ������� MiniSTM32 V3.0
	}

	
	if(LCD_Manage.ID == 0x9341)
	{
		LCD_Write_Register(0xcf);
		LCD_Write_Data(0x00);
		LCD_Write_Data(0xc1);	//�����ɸ�Ϊ81
		LCD_Write_Data(0x30);
		
		LCD_Write_Register(0xed);
		LCD_Write_Data(0x64);
		LCD_Write_Data(0x03);
		LCD_Write_Data(0x12);
		LCD_Write_Data(0x81);

		LCD_Write_Register(0xe8);
		LCD_Write_Data(0x85);
		LCD_Write_Data(0x10);	//�����ɸ�Ϊ00
		LCD_Write_Data(0x7a);	//�����ɸ�Ϊ79
		
		LCD_Write_Register(0xcb);
		LCD_Write_Data(0x39);
		LCD_Write_Data(0x2c);
		LCD_Write_Data(0x00);
		LCD_Write_Data(0x34);
		LCD_Write_Data(0x02);

		LCD_Write_Register(0xf7);
		LCD_Write_Data(0x20);
		
		LCD_Write_Register(0xea);
		LCD_Write_Data(0x00);
		LCD_Write_Data(0x00);
		
		LCD_Write_Register(0xc0);	//power control
		LCD_Write_Data(0x1b);	//VRH[5:0]
	
		LCD_Write_Register(0xc1);	//power control
		LCD_Write_Data(0x01);	//SAP[2:0],BT[3:0](11)
		
		LCD_Write_Register(0xc5);	//VCM control
		LCD_Write_Data(0x30);
		LCD_Write_Data(0x30);
		
		LCD_Write_Register(0xc7);	//VCM control2
		LCD_Write_Data(0xb7);

		///////////////////////////////////////////////////

 		LCD_Write_Register(0x36);	//Memory Access Control
		LCD_Write_Data(0x48);

		///////////////////////////////////////////////////
		//����
		LCD_Write_Register(0x3a);	//����3
		LCD_Write_Data(0x55);	
		///////////////////////////////////////////////////
		
		LCD_Write_Register(0xb1);
		LCD_Write_Data(0x00);
		LCD_Write_Data(0x1a);

		LCD_Write_Register(0xb6);	//Display Function Control
		LCD_Write_Data(0x0a);
		LCD_Write_Data(0xa2);
		
		LCD_Write_Register(0xf2);	//3Gamma Function Disable
		LCD_Write_Data(0x00);
		
		LCD_Write_Register(0x26);	//Gamma curve selected
		LCD_Write_Data(0x01);

		LCD_Write_Register(0xe0);	//Set Gamma
		LCD_Write_Data(0x0f);
		LCD_Write_Data(0x2a);
		LCD_Write_Data(0x28);
		LCD_Write_Data(0x08);
		LCD_Write_Data(0x0e);
		LCD_Write_Data(0x08);
		LCD_Write_Data(0x54);
		LCD_Write_Data(0xa9);
		LCD_Write_Data(0x43);
		LCD_Write_Data(0x0a);
		LCD_Write_Data(0x0f);
		LCD_Write_Data(0x00);
		LCD_Write_Data(0x00);
		LCD_Write_Data(0x00);
		LCD_Write_Data(0x00);

		LCD_Write_Register(0xe1);	//Set Gamma
		LCD_Write_Data(0x00);
		LCD_Write_Data(0x15);
		LCD_Write_Data(0x17);
		LCD_Write_Data(0x07);
		LCD_Write_Data(0x11);
		LCD_Write_Data(0x06);
		LCD_Write_Data(0x2b);
		LCD_Write_Data(0x56);
		LCD_Write_Data(0x3c);
		LCD_Write_Data(0x05);
		LCD_Write_Data(0x10);
		LCD_Write_Data(0x0f);
		LCD_Write_Data(0x3f);
		LCD_Write_Data(0x3f);
		LCD_Write_Data(0x0f);
		
		LCD_Write_Register(0x2b); 
		LCD_Write_Data(0x00);
		LCD_Write_Data(0x00);
		LCD_Write_Data(0x01);
		LCD_Write_Data(0x3f);
		LCD_Write_Register(0x2a); 
		LCD_Write_Data(0x00);
		LCD_Write_Data(0x00);
		LCD_Write_Data(0x00);
		LCD_Write_Data(0xef);	

		LCD_Exit_Sleep();	//�˳�˯�ߣ�����ʾ
		delay_ms(120);
		LCD_Display_ON(); //����ʾ
	}

	LCD_Display_DIR(0);	//������ʾ����
	LCD_BLControl = 1;	//�򿪱���
	
	LCD_Clear(Back_color);	//����Ĭ�ϱ�����ɫ
}
/**********************************************************
* �������� ---> LCD��������
* ��ڲ��� ---> color�������ɫ
* ������ֵ ---> none
* ����˵�� ---> ���ָ����color�������ʾ
**********************************************************/	
void LCD_Clear(u16 color)
{
	u32 index;
	u32 total_point;

	total_point = LCD_Manage.width * LCD_Manage.height;	//������ʾ��������ܵ���

	LCD_Set_xy(0x0000,0x0000);	//��궨������ԭ��[0:0]
	LCD_WriteGRAM();	//GRAM start writing
	for(index = 0;index < total_point;index++)	LCD_WR_Data(color);	//��ʼ�����ɫ
}
/**********************************************************
* �������� ---> LCD���㺯��
* ��ڲ��� ---> (x,y)���������ֵ
* ������ֵ ---> none
* ����˵�� ---> �����ɫ��ǰ��Ļ�����ɫ����
**********************************************************/
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_Set_xy(x,y);	//�趨�������
	LCD_WriteGRAM();	//GRAM start writing
	LCD_WR_Data(Point_color);	//д������ɫֵ
}      
/**********************************************************
* �������� ---> LCD���㺯������ɫֵ���趨д�루���ٻ��㣩
* ��ڲ��� ---> (x,y)���������ֵ
*               color��Ҫд�����ɫֵ
* ������ֵ ---> none
* ����˵�� ---> none
**********************************************************/					
void LCD_DrawPoint_Color(u16 x,u16 y,u16 color)
{
	LCD_Write_Register(LCD_Manage.setxcmd); 
	LCD_Write_Data(x>>8); 
	LCD_Write_Data(x&0XFF);	 
	LCD_Write_Register(LCD_Manage.setycmd); 
	LCD_Write_Data(y>>8); 
	LCD_Write_Data(y&0XFF);

	LCD_RS_CLR;
 	LCD_CS_CLR; 
	DATAOUT(LCD_Manage.wgramcmd);//дָ��  
	LCD_WR_CLR; 
	LCD_WR_SET; 
 	LCD_CS_SET; 
	LCD_Write_Data(Point_color);		//д����
}
/**********************************************************
* �������� ---> LCD���ߺ���
* ��ڲ��� ---> (x1,y1)���������ֵ
*               (x2,y2)�յ�������ֵ
* ������ֵ ---> none
* ����˵�� ---> �ߵ���ɫ��ǰ��Ļ�����ɫ����
**********************************************************/
void LCD_Draw_Line(u16 x1,u16 y1,u16 x2,u16 y2)
{
	u16 i;	//ѭ�����߱���
	int x_err,y_err;
	int diff_x,	/*	x��������	*/
		diff_y,	/*	y��������	*/
		distance;	/*	���߾���	*/

	int inc_x,	/*	x�ử�߷���	*/
		inc_y;	/*	y�ử�߷���	*/

	int brush_x,	/*	�����������	*/
		brush_y;

	brush_x = x1;	//���û����������
	brush_y = y1;

	diff_x = x2 - x1;	//����x�仯��
	diff_y = y2 - y1;	//����y�仯��

	if(diff_x > 0)	inc_x = 1;	//x�᷽����������
	else	if(diff_x == 0)	inc_x = 0;	//x�᷽�򲻱䣬����ֱ��
	else
	{	inc_x = -1;diff_x = -diff_x;	}	//��x�ᷴ����

	if(diff_y > 0)	inc_y = 1;	//y�᷽����������
	else	if(diff_y == 0)	inc_y = 0;	//y�᷽�򲻱䣬��ˮƽ��
	else
	{	inc_y = -1;diff_y = -diff_y;	}	//��y�ᷴ����

	//�����жϣ����޷��жϣ�
	if(diff_x > diff_y)	distance = diff_x;	//������315��С��455�㡢����135��С��225��ֱ��
	else	distance = diff_y;	//������45��С��135�㡢����225��С��315��ֱ��

	for(i = 0;i < distance+1;i++)	//���߿�ʼ
	{
		LCD_DrawPoint(brush_x,brush_y);	//��㿪ʼ
		x_err += diff_x;
		y_err += diff_y;
		
		if(x_err > distance)	//x�᷽�򻭱ʿ���
		{
			x_err -= distance;
			brush_x += inc_x;
		}

		if(y_err > distance)	//y�᷽�򻭱ʿ���
		{
			y_err -= distance;
			brush_y += inc_y;
		}
	}
}
/**********************************************************
* �������� ---> LCD�����κ���
* ��ڲ��� ---> (x1,y1)���������ֵ
*               (x2,y2)�Խ�������ֵ
* ������ֵ ---> none
* ����˵�� ---> �ߵ���ɫ��ǰ��Ļ�����ɫ����
**********************************************************/
void LCD_Draw_Quad(u16 x1,u16 y1,u16 x2,u16 y2)
{
	LCD_Draw_Line(x1,y1,x2,y1);	//��y1Ϊ�ử��
	LCD_Draw_Line(x2,y1,x2,y2);	//��x2Ϊ�ử��
	LCD_Draw_Line(x2,y2,x1,y2);	//��y2Ϊ�ử��
	LCD_Draw_Line(x1,y2,x1,y1);	//��x1Ϊ�ử��
}
/**********************************************************
* �������� ---> LCDָ�����������ɫ����
* ��ڲ��� ---> (x1,y1)���������ֵ
*               (x2,y2)�Խ�������ֵ
*               color�������ɫֵ
* ������ֵ ---> none
* ����˵�� ---> ����Ļ�ϻ�һ���ı��β������Ӧ����ɫ
*               �������С������� = (x2 - x1) * (y2 - y1)
**********************************************************/
void LCD_Area_Color(u16 x1,u16 y1,u16 x2,u16 y2,u16 color)
{
	u16 i,j;
	u16 x_len;
	
	x_len = x2 - x1 + 1;	//����X�᷽������
	
	for(i = y1;i < y2;i++)	//y�᷽���Խ���
	{
		LCD_Set_xy(x1,i);	//�趨������
		LCD_WriteGRAM();	//GRAM start writing
		for(j = 0;j < x_len;j++)	LCD_Write_Data(color);	//���¹��λ�ã�д����ɫ
	}
}
/**********************************************************
* �������� ---> LCD�е㷨��Բ����
* ��ڲ��� ---> (x,y)���������ֵ
*               r��Բ�뾶
* ������ֵ ---> none
* ����˵�� ---> Բ����ɫ��ǰ��Ļ�����ɫ����
**********************************************************/	
void LCD_Draw_Circle(u16 x0,u16 y0,u16 r)
{
	u8 x,y;	//�᷽�����
	float d;
	x = 0;
	y = r;
	d = 5.0/4 - r;	//��ʱ�뻭Բ

	while(x <= y)
	{
		LCD_DrawPoint(x0 + x,y0 + y);	//270��
		LCD_DrawPoint(x0 + x,y0 - y);	//90��
		LCD_DrawPoint(x0 - x,y0 + y);
		LCD_DrawPoint(x0 - x,y0 - y);
		LCD_DrawPoint(x0 + y,y0 + x);	//0��
		LCD_DrawPoint(x0 + y,y0 - x);
		LCD_DrawPoint(x0 - y,y0 + x);	//180��
		LCD_DrawPoint(x0 - y,y0 - x);
		
		if(d < 0)	//y��˥����
		{	d += x * 2.0 + 3;	}
		else		//x��˥����
		{
			d += 2.0 * (x - y) + 5;
			y--;
		}
		x++;
	}
}
/**********************************************************
* �������� ---> LCDָ��λ����ʾһ���ַ�����
* ��ڲ��� ---> (x,y)������ֵ
*               ch��Ҫ��ʾ���ַ�
*               size�������С
*               mode��0���ǵ��ӷ�ʽ��ʾ��1�����ӷ�ʽ��ʾ
* ������ֵ ---> none
* ����˵�� ---> ��Ҫ��ʾһЩ��Ҫ�仯֮���������ʾ
**********************************************************/
void LCD_Draw_Char(u16 x,u16 y,u8 ch,u8 size,u8 mode)
{
	u8 temp;
	u8 i,j;
	u16 y0=y;
	u16 colortemp=Point_color;

	if((x > LCD_Manage.width) || (y > LCD_Manage.height))		return;	//��������곬����Χ��ֱ���˳�

	ch = ch - ' ';	//�õ�ƫ�ƺ��ֵ

	if(!mode) //�ǵ��ӷ�ʽ
	{
		for(i = 0;i < size;i++)	//�п���
		{
			if (size == 16)	temp = ascii_1608[ch][i];	//����1608����
			else temp = ascii_1206[ch][i];	//����1206���� 		 

			for(j = 0;j < 8;j++)	//�п���
			{                				
				if(temp & 0x80)	Point_color = colortemp;	//�ַ���ɫ 
				else	Point_color = Back_color;	//������ɫ
				LCD_DrawPoint(x,y);	//��ʼ���
				temp <<= 1;
				y++;
				if(x >= LCD_Manage.width)	{Point_color = colortemp;return;}	//����Һ����ȣ�ֱ���˳�
				if((y - y0) == size)
				{
					y = y0;
					x++;
					if(x >= LCD_Manage.width)	{Point_color = colortemp;return;}	//����Һ����ȣ�ֱ���˳�
					break;
				}			  
			}     //end for j
		}     //end for i	
	}
	else//���ӷ�ʽ
	{
		for(i = 0;i < size;i++)	//�п���
		{
			if(size == 16)	temp = ascii_1608[ch][i];	//����1608����
			else temp = ascii_1206[ch][i];	//����1206����		 

			for(j = 0;j < 8;j++)	//�п���
			{ 				
				if(temp & 0x80)	LCD_DrawPoint(x,y);//��һ����
				temp <<= 1;
				y++;
				if(x >= LCD_Manage.width)	{Point_color = colortemp;return;}	//����Һ����ȣ�ֱ���˳�
				if((y - y0) == size)
				{
					y = y0;
					x++;
					if(x >= LCD_Manage.width)	{Point_color = colortemp;return;}	//����Һ����ȣ�ֱ���˳�
					break;
				} 					 
			}     //end for j
		}     //end for i
	}
	Point_color = colortemp;	//�ָ�������ɫ	    	   	 	  
}
/**********************************************************
* �������� ---> LCDָ��λ����ʾһ���ַ����ַ���������ɫ����趨
* ��ڲ��� ---> (x,y)������ֵ
*               ch��Ҫ��ʾ���ַ�
*               charcolor���ַ���ɫ
*               bkcolor���ַ�������ɫ
* ������ֵ ---> none
* ����˵�� ---> ��Ҫ��ʾһЩ�̶�����Ҫ�仯���ַ���������
*               ��(x,y)��������ʾһ��8x16��Ӣ���ַ��������ֲ�ָ������ɫ��������ɫ
**********************************************************/
void LCD_Draw_Char_BK(u16 x,u16 y,u8 ch,u8 size,u16 charcolor,u16 bkcolor)
{
	u8 i,j;
	u8 temp=0;
	u16 y0=y;

	if((x > LCD_Manage.width) || (y > LCD_Manage.height))		return;	//��������곬����Χ��ֱ���˳�

	ch = ch - ' ';	//�õ���ַƫ��

	for(i = 0;i < size;i++)	//�п���
	{
		if(size == 16)	temp = ascii_1608[ch][i];	//����1608����
		else temp = ascii_1206[ch][i];	//����1206����

		for(j = 0;j < 8;j++)	//�п���
		{
			if(temp & 0x80)	LCD_DrawPoint_Color(x,y,charcolor); // �ַ���ɫ 
			else	LCD_DrawPoint_Color(x,y,bkcolor); // ������ɫ
			temp <<= 1;
			y++;
			if(x >= LCD_Manage.width)	return;	//����Һ����ȣ�ֱ���˳�
			if((y - y0) == size)
			{
				y = y0;
				x++;
				if(x >= LCD_Manage.width)	return;	//����Һ����ȣ�ֱ���˳�
				break;
			}
		}     //end for j
	}     //end for i
}
/**********************************************************
* �������� ---> LCD��ʾһ�����֣�������ɫ����趨
* ��ڲ��� ---> (x,y)���������ֵ
*               *ch��Ҫ��ʾ���ַ�
*               fontsize�������С
*               charcolor���ַ���ɫ
*               bkcolor���ַ�������ɫ
* ������ֵ ---> none
* ����˵�� ---> ��(x,y)��������ʾһ��16 x 16�����ĺ��ֲ�ָ������ɫ��������ɫ
**********************************************************/
void LCD_Draw_GB_BK(u16 x,u16 y,u8 *ch,u8 fontsize,u16 charcolor,u16 bkcolor)
{
	static u8 FONTbuff[32];	//������ģ���ݻ���
	u8 i,j; 
	u8 temp_char=0;
	int num=0;

	if((x > LCD_Manage.width) || (y > LCD_Manage.height))		return;	//��������곬����Χ��ֱ���˳�
	
	Get_Hzlib(ch, FONTbuff, fontsize);	//��ȡ��ģ����
    
	for(i = 0;i < 16;i++)	//��ʾһ��16 * 16�ĺ���
	{
		temp_char = FONTbuff[num];

		for(j = 0;j < 8;j++)	//��ʾ���ֵ����ߣ�8 * 16
		{
			if((temp_char >> 7 - j) & 0x01 == 0x01)	LCD_DrawPoint_Color(x+j,y+i,charcolor); // �ַ���ɫ
			else	LCD_DrawPoint_Color(x+j,y+i,bkcolor); // ������ɫ
		}     //end for j
		num++;

		temp_char = FONTbuff[num];

		for(j = 0;j < 8;j++)	//��ʾ���ֵ��Ұ�ߣ�8 * 16
		{
			if((temp_char >> 7 - j) & 0x01 == 0x01)	LCD_DrawPoint_Color(x+j+8,y+i,charcolor); // �ַ���ɫ
			else	LCD_DrawPoint_Color(x+j+8,y+i,bkcolor); // ������ɫ
		}     //end for j
		num++;
	} 
}
/**********************************************************
* �������� ---> LCD��ʾ���ֻ��ַ���������ɫ����趨
* ��ڲ��� ---> (x,y)���������ֵ
*               ch[2]��Ҫ��ʾ���ַ�
*               charcolor���ַ���ɫ
*               bkcolor���ַ�������ɫ
* ������ֵ ---> none
* ����˵�� ---> ��(x,y)��������ʾ�ַ�����ָ������ɫ���ж�������ʾ����Ӣ����ʾ
**********************************************************/	
void LCD_Display_String_BK(u16 x,u16 y,u8* ch,u8 size,u16 charcolor,u16 bkcolor)
{
	for(;*ch != '\0';ch++)
	{
		if((*ch & 0x80) == 0)	//��ʾӢ��
		{
			LCD_Draw_Char_BK(x,y,*ch,size,charcolor,bkcolor);
			x += 8;
		}
		else	//��ʾ����
		{
			LCD_Draw_GB_BK(x,y,ch,size,charcolor,bkcolor);
			x += 16;
			ch++;
		}
	}
}
/**********************************************************
* �������� ---> һԪ��η��̼���
* ��ڲ��� ---> x������
*               n��ָ��
* ���ز��� ---> none 
* ����˵�� ---> x��n�η�
**********************************************************/
u32 Equation_Calculate(u8 x,u8 n)
{
	u32 dat=1;
	while(n--)	dat *= x;	//n��x���
	return dat;
}
/**********************************************************
* �������� ---> LCD��ʾ���֣���λΪ0ʱ����ʾ
* ��ڲ��� ---> (x,y)��ʾ��ʼ����
*               arr��Ҫ��ʾ�����֣���Χ0 ~ 4294967295
*               size�������С
*               sum����ʾ����λ�������ֵ�λ������ʾ�����ø�ֵֻ�����θ�λ
*                    ������ʾ����65535����ʾ3λ�Ļ��������535
*               mode����ʾЧ�����á�0�������ӡ�1������
* ���ز��� ---> none 
* ����˵�� ---> ��(x,y)��������ʾ���ֲ�ָ������ɫ
**********************************************************/
void LCD_Display_Array_No0(u16 x,u16 y,u32 arr,u8 size,u8 sum,u8 mode)
{
	u8 m,temp=0;
	u16 arr_temp;

	for(m = 0;m < sum;m++)
	{
		arr_temp = (arr / (Equation_Calculate(10,(sum - m - 1)))) % 10;	//ȡ�����λ����

		if((temp == 0) && (m < (sum -1)))	//�Ƿ����һλ����
		{
			if(arr_temp == 0)	//ȡ�õ�����Ϊ0
			{
				LCD_Draw_Char(x+8*m,y,0x20,size,mode);	//ȡ�õ����ָ�λ��0����ʾ�ո�
				continue;	//����ʾ���λ
			}
			else	temp = 1;
		}
		if(x + 8*m > LCD_Manage.width)	//������ʾ�������
		{
			if(y+16 > LCD_Manage.height)	//������ʾ����ʾ��Χ
			{	break;	}	//�˳���ʣ�����ݲ���ʾ��

			LCD_Draw_Char(x+8*m,y+16,(arr_temp + 0x30),size,mode);	//����һ����ʾ
		}
		else	LCD_Draw_Char(x+8*m,y,(arr_temp + 0x30),size,mode);	//û������Χ���ڱ�����ʾ
	}     //end for m
}
/**********************************************************
* �������� ---> LCD��ʾ���֣���λΪ0ʱ��ʾ0
* ��ڲ��� ---> (x,y)��ʾ��ʼ����
*               arr��Ҫ��ʾ�����֣���Χ0 ~ 4294967295
*               sum����ʾ����λ��
*               mode����ʾЧ�����á�0�������ӡ�1������
* ���ز��� ---> none 
* ����˵�� ---> ��(x,y)��������ʾ���ֲ�ָ������ɫ
**********************************************************/
void LCD_Display_Array_Yes0(u16 x,u16 y,u32 arr,u8 size,u8 sum,u8 mode)
{
	u8 m,temp=0;
	u16 arr_temp;

	for(m = 0;m < sum;m++)
	{
		arr_temp = (arr / (Equation_Calculate(10,(sum - m - 1)))) % 10;	//ȡ�����λ����

		if((temp == 0) && (m < (sum -1)))	//�Ƿ����һλ����
		{
			if(arr_temp == 0)	//ȡ�õ�����Ϊ0
			{
				LCD_Draw_Char(x+8*m,y,(arr_temp + 0x30),size,mode);	//��λ��ʾ0
				continue;
			}
			else	temp = 1;
		}
		if(x + 8*m > LCD_Manage.width)	//������ʾ�������
		{
			if(y+16 > LCD_Manage.height)	//������ʾ����ʾ��Χ
			{	break;	}	//�˳���ʣ�����ݲ���ʾ��

			LCD_Draw_Char(x+8*m,y+16,(arr_temp + 0x30),size,mode);	//����һ����ʾ	 
		}
		else	LCD_Draw_Char(x+8*m,y,(arr_temp + 0x30),size,mode);	//û������Χ���ڱ�����ʾ
	}     //end for m
}
/**********************************************************
* �������� ---> LCD��ʾһ�����ִ���������ʾλ�������塢������ɫ����趨
* ��ڲ��� ---> (x,y)���������ֵ
*               ch[2]��Ҫ��ʾ���ַ�
*               fontsize�������С
*               chlen����ʾ�����ַ����ܳ��ȡ�����ֵ������sizeof()-1������û���ֱ����������
*               sum: ��ʾλ�������ֵ�λ������ʾ�����ø�ֵֻ�����θ�λ
*                    ������ʾ����00655.35����ʾ4λ�Ļ��������5.35
*               charcolor��������ɫ
*               bkcolor����ʾ���屳����ɫ
* ������ֵ ---> none
* ����˵�� ---> A����Ҫ��ʾһ�����ִ��ȣ��ǵ��ӷ�ʽ��ʾ
*               B����ʾС��ʱ���Ǹ���ʾλ������С����
**********************************************************/
void LCD_Display_String_Num(u16 x,u16 y,u8 *ch,u8 fontsize,u16 chlen,u16 sum,u16 charcolor,u16 bkcolor)
{
	u16 i;
	
	if(chlen == sum)	//ȫ����ʾ
	{
		for(;sum > 0;sum--)
		{
			LCD_Draw_Char_BK(x, y, *ch, fontsize, charcolor, bkcolor);	//����ַ�
			x += 8;
			ch++;
		}
	}
	else	//ֻ��ʾ���֣������λ��ʼ����sumλҪ��ʾ
	{
		for(i = chlen-sum;i < chlen;i++)	//�ӵ����ĵ�sum���ַ���ʼ��ʾ
		{
			LCD_Draw_Char_BK(x, y, ch[i], fontsize, charcolor, bkcolor);	//����ַ�
			x += 8;
		}
	}
}

