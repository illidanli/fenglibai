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
#include "motor_pwm.h"
#include "motor_control.h"
#include "stdio.h"
/*------------------------------------------
 				全局变量				
------------------------------------------*/ 
extern M1TypeDef M1;
extern M2TypeDef M2;
/*------------------------------------------
 函数功能:配置TIM2复用输出PWM时用到的I/O
 函数说明:PA0 - TIM2_CH1 - M4_PWM
	   	  PA1 - TIM2_CH2 - M1_PWM
	   	  PA2 - TIM2_CH3 - M2_PWM
		  PA3 - TIM2_CH4 - M3_PWM
		  
		  - M1_DIR  -> PD14
 		  - M1_STOP -> PD12

		  - M2_DIR  -> PD15
 		  - M2_STOP -> PD13
		  
		  - M3_DIR  -> PD10
 		  - M3_STOP -> PD11
		  
		  - M4_DIR  -> PD8
 		  - M4_STOP -> PD9				
------------------------------------------*/								   
void PWM_GPIO_Config(void) 	  
{
	 GPIO_InitTypeDef GPIO_InitStructure;		  
	 
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,  ENABLE); 	
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
	  
	 GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_Init(GPIOA, &GPIO_InitStructure);
}
/*------------------------------------------
 函数功能:配置TIM2输出的PWM信号的模式
 函数说明:- TIM2通道4输出PWM
 		  - PWM模式1
 		  - 极性低电平
		  - PWM频率 = 24kHz				
------------------------------------------*/
void PWM_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
		  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 	

	TIM_TimeBaseStructure.TIM_Prescaler =  0; 			        //时钟预分频
	TIM_TimeBaseStructure.TIM_CounterMode =  TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseStructure.TIM_Period = 3000;				    //自动重装值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	    //时钟分频1
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode =  TIM_OCMode_PWM2;         
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_Pulse = 0;     
	TIM_OC1Init(TIM2,&TIM_OCInitStructure);
	
	TIM_OCInitStructure.TIM_OCMode =  TIM_OCMode_PWM2;         
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_Pulse = 0;     
	TIM_OC2Init(TIM2,&TIM_OCInitStructure);				        
	
	TIM_OCInitStructure.TIM_OCMode =  TIM_OCMode_PWM2;           
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_Pulse = 0;     
	TIM_OC3Init(TIM2,&TIM_OCInitStructure);	
	
	TIM_OCInitStructure.TIM_OCMode =  TIM_OCMode_PWM2;           
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_Pulse = 0;     
	TIM_OC4Init(TIM2,&TIM_OCInitStructure);			         
	
	TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Enable); //使能TIM2在CCR2上的预装载寄存器
	TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable); //使能TIM2在CCR2上的预装载寄存器
	TIM_OC3PreloadConfig(TIM2,TIM_OCPreload_Enable); //使能TIM2在CCR3上的预装载寄存器
	TIM_OC4PreloadConfig(TIM2,TIM_OCPreload_Enable); //使能TIM2在CCR4上的预装载寄存器

	TIM_SetCompare1(TIM2,0);
	TIM_SetCompare2(TIM2,0);
	TIM_SetCompare3(TIM2,0);
	TIM_SetCompare4(TIM2,0);

	TIM_Cmd(TIM2,ENABLE);	//使能TIM2
}
/*------------------------------------------
 函数功能:电机运动方向管脚配置
 函数说明:- M1_DIR  -> PD14
 		  - M1_STOP -> PD12

		  - M2_DIR  -> PD15
 		  - M2_STOP -> PD13
		  
		  - M3_DIR  -> PD10
 		  - M3_STOP -> PD11
		  
		  - M4_DIR  -> PD8
 		  - M4_STOP -> PD9					
------------------------------------------*/ 
void MOTOR_DIR_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   // 推挽输出    
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	M1_Forward;
	M2_Forward;
	M3_Forward;
	M4_Forward;
	
	M1_STOP;
	M2_STOP;
	M3_STOP;
	M4_STOP;	 
}
/*------------------------------------------
 函数功能:PWM输出初始化				
------------------------------------------*/
void PWM_Init(void) 
{
	PWM_GPIO_Config();
	PWM_Mode_Config();
	MOTOR_DIR_GPIO_Config();		 
}
/*------------------------------------------
 函数功能:电机1正方向运动
 函数参数:CCR2_VAL占空比计数值
 函数说明:CCR2_VAL越大转速越快 				
------------------------------------------*/
void PWM_M1_Forward(uint16_t val)
{   	
	M1_Backward;
	M1_STOP;
	TIM_SetCompare2(TIM2,val);     //值越大转速越快
}
/*------------------------------------------
 函数功能:电机1反方向运动
 函数参数:CCR2_VAL占空比计数值
 函数说明:CCR2_VAL越大转速越快 				
------------------------------------------*/
void PWM_M1_Backward(uint16_t val)
{   	
	M1_Forward;
	M1_RELEASE;
	TIM_SetCompare2(TIM2,val);     //值越大转速越快
}
/*------------------------------------------
 函数功能:电机2正方向运动
 函数参数:CCR3_VAL占空比计数值
 函数说明:CCR3_VAL越大转速越快 				
------------------------------------------*/
void PWM_M2_Forward(uint16_t val)
{   
	M2_Backward;
	M2_STOP;
	TIM_SetCompare3(TIM2,val);     //值越大转速越快
}
/*------------------------------------------
 函数功能:电机2反方向运动
 函数参数:CCR3_VAL占空比计数值
 函数说明:CCR3_VAL越大转速越快 				
------------------------------------------*/
void PWM_M2_Backward(uint16_t val)
{   	
	M2_Forward;
	M2_RELEASE;
	TIM_SetCompare3(TIM2,val);     //值越大转速越快
}
/*------------------------------------------
 函数功能:电机3正方向运动
 函数参数:CCR4_VAL占空比计数值
 函数说明:CCR4_VAL越大转速越快 				
------------------------------------------*/
void PWM_M3_Forward(uint16_t val)
{   	
	M3_Forward;
	M3_RELEASE;
	TIM_SetCompare4(TIM2,val);     //值越大转速越快
}
/*------------------------------------------
 函数功能:电机3反方向运动
 函数参数:CCR4_VAL占空比计数值
 函数说明:CCR4_VAL越大转速越快 				
------------------------------------------*/
void PWM_M3_Backward(uint16_t val)
{   
	M3_Backward;
	M3_STOP;
	TIM_SetCompare4(TIM2,val);     //值越大转速越快
}
/*------------------------------------------
 函数功能:电机4正方向运动
 函数参数:CCR1_VAL占空比计数值
 函数说明:CCR1_VAL越大转速越快 				
------------------------------------------*/
void PWM_M4_Forward(uint16_t val)
{   
	M4_Forward;
	M4_RELEASE;
	TIM_SetCompare1(TIM2,val);     //值越大转速越快
}
/*------------------------------------------
 函数功能:电机4反方向运动
 函数参数:CCR1_VAL占空比计数值
 函数说明:CCR1_VAL越大转速越快 				
------------------------------------------*/
void PWM_M4_Backward(uint16_t val)
{   	
	M4_Backward;
	M4_STOP;
	TIM_SetCompare1(TIM2,val);     //值越大转速越快
}


