#include "Global.h"
#include "lcd.h"
#include "WM.h"
#include "GUI.h"
#include "sys.h"
#include "usart.h"
#include "led.h"
#include "delay.h"
#include "lcd9341.h"
#include "display.h"

int main(void)
{
	LED_Init();
	USART1_init();

	SysTick_Config(SystemCoreClock / 1000);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE); //ʹ��CRCʱ��
	WM_SetCreateFlags(WM_CF_MEMDEV);//���� STemWin �洢�豸
	GUI_Init();
//	GUI_SetBkColor(GUI_BLUE); //���ñ�����ɫ
//	GUI_SetColor(GUI_YELLOW); //������ɫ
	GUI_Clear(); //����
	LCD_LED=1;
//	GUI_SetFont(&GUI_Font24_ASCII); //��������
	GUI_DispStringAt("Hello Word!",30,120); //��ָ��λ����ʾ�ַ���	
	
	while(1);
}


