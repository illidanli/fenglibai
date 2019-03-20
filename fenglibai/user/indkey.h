#ifndef __INDKEY_H
#define __INDKEY_H

#include "stm32f10x.h"

#define KEY_PRESSED 0  //根据上下拉电阻不同而变
#define KEY_UNPRESS	1  //根据上下拉电阻不同而变

void Key_IO_Init(void);
void KeyScan(void);

#endif
