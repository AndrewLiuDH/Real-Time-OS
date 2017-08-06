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

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE); //使能CRC时钟
	WM_SetCreateFlags(WM_CF_MEMDEV);//开启 STemWin 存储设备
	GUI_Init();
//	GUI_SetBkColor(GUI_BLUE); //设置背景颜色
//	GUI_SetColor(GUI_YELLOW); //设置颜色
	GUI_Clear(); //清屏
	LCD_LED=1;
//	GUI_SetFont(&GUI_Font24_ASCII); //设置字体
	GUI_DispStringAt("Hello Word!",30,120); //在指定位置显示字符串	
	
	while(1);
}


