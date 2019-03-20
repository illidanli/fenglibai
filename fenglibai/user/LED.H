#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

#define LED1ON  	GPIOE->BRR  = GPIO_Pin_8
#define LED1OFF 	GPIOE->BSRR = GPIO_Pin_8
#define LED1Toggle	GPIOE->ODR ^= GPIO_Pin_8

#define LED2ON  	GPIOE->BRR  = GPIO_Pin_9
#define LED2OFF 	GPIOE->BSRR = GPIO_Pin_9
#define LED2Toggle	GPIOE->ODR ^= GPIO_Pin_9

#define BEEPON		GPIOE->BRR  = GPIO_Pin_7
#define BEEPOFF		GPIOE->BSRR = GPIO_Pin_7

void LED_GPIO_Config(void);

#endif

