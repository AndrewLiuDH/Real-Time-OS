#ifndef __LED_H__
#define __LED_H__
#include "sys.h"

void LED_Init(void);
void LED_flash(void);

#define LED1 PHout(2)
#define LED2 PHout(3)
#define LED3 PHout(4)
#define LED4 PHout(5)

#endif

