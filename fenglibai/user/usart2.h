#ifndef __USART2_H
#define	__USART2_H

#include "stm32f10x.h"
#include <stdio.h>

void USART2_Config(void);
int fputc(int ch, FILE *f);
void USART2_printf(USART_TypeDef* USARTx, uint8_t *Data,...);
void GpuSend(char * buf1);
void Display_Title(void);
#endif /* __USART2_H */
