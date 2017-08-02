#ifndef __LED_H__
#define __LED_H__

#include "sys.h"

void LED_Init(void);
void LED_flash(void);

#define LED PHout(5)

#define LED0 PHout(2)
#define LED1 PHout(3)
#define LED2 PHout(4)
#define LED3 PHout(5)


#endif

