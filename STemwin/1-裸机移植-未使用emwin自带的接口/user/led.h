#ifndef __LED_H__
#define __LED_H__

#include "sys.h"

void LED_Init(void);
void LED_flash(void);

#define LED1 PDout(0)
#define LED2 PDout(1)
#define LED3 PDout(3)
#define LED4 PDout(4)


#endif

