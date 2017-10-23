#ifndef __LED_H__
#define __LED_H__

#include "sys.h"

void LED_Init(void);
void LED_flash(void);

#define LED PBout(1)

#endif

