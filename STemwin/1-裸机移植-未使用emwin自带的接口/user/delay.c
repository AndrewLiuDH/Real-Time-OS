#include "delay.h"
#include "sys.h"

void delay_us(u16 nus)

{    
	u16 i=0;  
	while(nus--)
	{
		i=20;
		while(i--) ;    
	}
}

void delay_ms(u32 nms)
{    
	u16 i=0;  
	nms*=1000;
	delay_us(nms);
}



































