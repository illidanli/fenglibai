/*-------------------------------------------------------------------------------------------
        		   			风力摆控制系统(2015-8-12)

 硬件平台:
 			主控器: STM32F103VET6 64K RAM 512K ROM
			驱动器: LMD18200T 
		    电源:   DC +12V

 软件平台:
 			开发环境: RealView MDK-ARM uVision4.10
			C编译器 : ARMCC
			ASM编译器:ARMASM
			连接器:   ARMLINK
			底层驱动: 各个外设驱动程序       
 
 时间: 2015年8月12日       
 
 作者: BoX
-------------------------------------------------------------------------------------------*/
#ifndef __MOTOR__PWM__H__
#define __MOTOR__PWM__H__	

#include "stm32f10x.h"


/* 方向1表示向前,0表示向后 */
#define M1_Forward	 	GPIOD->BSRR = GPIO_Pin_14
#define M1_Backward	 	GPIOD->BRR  = GPIO_Pin_14

/* 方向1表示向前,0表示向后 */
#define M2_Forward	 	GPIOD->BSRR = GPIO_Pin_15 
#define M2_Backward	 	GPIOD->BRR  = GPIO_Pin_15

/* 方向1表示向前,0表示向后 */
#define M3_Forward	 	GPIOD->BSRR = GPIO_Pin_10
#define M3_Backward	 	GPIOD->BRR  = GPIO_Pin_10

/* 方向1表示向前,0表示向后 */
#define M4_Forward	 	GPIOD->BSRR = GPIO_Pin_8
#define M4_Backward	 	GPIOD->BRR  = GPIO_Pin_8



/* 电机1用通道2 */
#define M1_STOP			GPIOD->BSRR = GPIO_Pin_12
#define M1_RELEASE      GPIOD->BRR  = GPIO_Pin_12

/* 电机2用通道3 */
#define M2_STOP			GPIOD->BSRR = GPIO_Pin_13
#define M2_RELEASE      GPIOD->BRR  = GPIO_Pin_13

/* 电机3 */
#define M3_STOP			GPIOD->BSRR = GPIO_Pin_11
#define M3_RELEASE      GPIOD->BRR  = GPIO_Pin_11

/* 电机4 */
#define M4_STOP			GPIOD->BSRR = GPIO_Pin_9
#define M4_RELEASE      GPIOD->BRR  = GPIO_Pin_9

void PWM_GPIO_Config(void);

void PWM_M1_Backward(uint16_t val);
void PWM_M1_Forward(uint16_t val);

void PWM_M2_Backward(uint16_t val);
void PWM_M2_Forward(uint16_t val);

void PWM_M3_Backward(uint16_t val);
void PWM_M3_Forward(uint16_t val);

void PWM_M4_Backward(uint16_t val);
void PWM_M4_Forward(uint16_t val);

void PWM_GPIO_Config(void);
void PWM_Init(void);
void PWM_Mode_Config(void);


#endif


