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
#include "stm32f10x.h"
#include "usart1.h"					 
#include "delay.h"
#include "led.h"
#include "timer.h"
#include "motor_control.h"
#include "motor_pwm.h"
#include "motor_pid.h"
#include "stdlib.h"
#include "indkey.h"
#include "mpu6050.h"
#include "ahrs.h"
#include "math.h"
#include "usart2.h"
/*------------------------------------------
 				全局变量				
------------------------------------------*/ 
extern uint8_t Q1_Start;
extern uint8_t Q2_Start;
extern uint8_t Q3_Start;
extern uint8_t Q4_Start;
extern uint8_t Q5_Start;
extern uint8_t Q6_Start;
extern uint8_t CurMode; 
/*-----------------------------------------
			    初始化外设
------------------------------------------*/ 
void BSP_Init(void)
{
	DelayInit();
	LED_GPIO_Config();
	Key_IO_Init();	
	PID_M1_Init();
	PID_M2_Init();
	M1TypeDef_Init();
	M2TypeDef_Init();	
	PWM_Init();
	TIM1_Config(10000-1,71);  /* TIM1 10ms Inturrupt  */
	TIM5_Config(5000-1,71);   /* TIM5 5ms Inturrupt 采样率200Hz 不能更改*/
	USART1_Config();
	USART1_printf(USART1," 系统启动成功!\r\n");
	USART2_Config();
	Display_Title();
	MPU6050_Init();	          //MPU6050									    
	TIM_Cmd(TIM5,ENABLE);  	  //电机控制都在TIM5完成
	TIM_Cmd(TIM1,ENABLE);
}
/*-----------------------------------------
				主 函 数
------------------------------------------*/ 
int main(void)    
{   
	BSP_Init();
	//角度采样和运动控制都在TIM5_IRQHandler()中完成
	while(1)  
	{  	
		if(Q1_Start == 1) 
		{
			CurMode = 1;
		}
		else if(Q2_Start == 1)
		{ 
			CurMode = 2;
		}
		else if(Q3_Start == 1)
		{
		 	CurMode = 3;
		}
		else if(Q4_Start == 1)
		{
		 	CurMode = 4;
		}
		else if(Q5_Start == 1)
		{
			CurMode = 5;
		}
		else if(Q6_Start == 1)
		{
			CurMode = 6;
		}
		else
		{
			CurMode = 0;
		}	
	}     
}

	
