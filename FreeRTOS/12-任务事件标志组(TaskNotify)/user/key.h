#ifndef __KEY_H__
#define __KEY_H__

#include "sys.h"

#define KEY0 PEin(7)
#define KEY1 PEin(8)
#define KEY2 PEin(9)
#define KEY3 PEin(10)

void KEY_init(void);
u8 Key_Scan(u8 mode);


#endif