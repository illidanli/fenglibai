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
#include "motor_control.h"
#include "motor_pid.h"
#include "motor_pwm.h"
#include "Timer.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "stdlib.h"
#include "usart1.h"
#include "indkey.h"
#include "ahrs.h"
#include "mpu6050.h"
/*------------------------------------------
 				全局变量				
------------------------------------------*/ 
static uint16_t LED6Cnt = 0;
uint8_t CurMode = 0;

extern M1TypeDef  M1;
extern M2TypeDef  M2;

extern PIDTypdDef M1PID;
extern PIDTypdDef M2PID;

extern MPU6050_AxisTypeDef    Axis;  //MPU6050数据结构体
extern AHRS_EulerAngleTypeDef EulerAngle;

extern uint8_t Item;
extern char buf[512];	 //液晶屏数据缓冲区
/*-----------------------------------------------
 函数功能: TIM1定时器为PID采样计算提供稳定中断
 函数参数: ARR寄存器值0-65535,预分频值0-65535
 参 考 值: TIM1_Config(9999,71)
	       计数频率1MHz,中断频率100Hz
		   计数器每1us加1,中断每10ms产生一次		  			  
-----------------------------------------------*/
void TIM1_Config(uint16_t Period,uint16_t Prescaler)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef        NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);	
	
	TIM_TimeBaseStructure.TIM_Prescaler = Prescaler; 			//时钟预分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseStructure.TIM_Period = Period;			        //自动重装值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	    //时钟分频1
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;			
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM1,TIM_FLAG_Update);
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);  					
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;   //抢占优先级2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  	    //响应优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	            
	NVIC_Init(&NVIC_InitStructure);	
}
/*-----------------------------------------------
 函数功能: TIM5定时器为PID采样计算提供稳定中断
 函数参数: ARR寄存器值0-65535,预分频值0-65535
 参 考 值: TIM5_Config(999,71)
	       计数频率1MHz,中断频率1000Hz
		   计数器每1us加1,中断每1ms产生一次		  			  
-----------------------------------------------*/
void TIM5_Config(unsigned short int Period,unsigned short int Prescaler)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef        NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);	
	
	TIM_TimeBaseStructure.TIM_Prescaler = Prescaler; 			//时钟预分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseStructure.TIM_Period = Period;			        //自动重装值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	    //时钟分频1
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;			
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM5,TIM_FLAG_Update);
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);  					
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   //抢占优先级2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  	    //响应优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	            
	NVIC_Init(&NVIC_InitStructure);		
}
/*-----------------------------------------------
 函数功能:TIM1中断服务程序
 函数说明:10ms进一次中断
-----------------------------------------------*/
void TIM1_UP_IRQHandler(void)
{		
	LED6Cnt++;

	if(LED6Cnt%10 == 0)	   //100ms扫描一次
	{	
		printf("% 2.1f % 2.1f\r\n",EulerAngle.Pitch,EulerAngle.Roll);
		KeyScan();
	}
	
	TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
}
/*-----------------------------------------------
 函数功能:TIM5中断服务程序
 函数说明:每5ms进入一次中断,采样率200Hz
 实测运行时间: 3.93ms
-----------------------------------------------*/
#define H (0.88f)  //万向节距地面的高度(米)
void TIM5_IRQHandler(void)
{  	
	uint8_t i = 0;
	float pitch_temp1 = 0.0;
	float roll_temp1 = 0.0;
	float pitch_temp2 = 0.0;
	float roll_temp2 = 0.0;
	static float pitch_sum = 0.0;
	static float roll_sum = 0.0;
		
	GPIOE->BSRR = GPIO_Pin_3;
	if(TIM_GetITStatus(TIM5,TIM_IT_Update) == SET)
	{			
		for(i=0;i<3;i++)
		{
			Angle_Calculate();		
			
			pitch_temp1 = (atan(Axis.AccX/Axis.AccZ)*57.2958-0.4);   //计算Pitch角度 0.4为静态偏差角
			roll_temp1  = (atan(Axis.AccY/Axis.AccZ)*57.2958-0.3);   //计算Roll角度  0.3为静态偏差角
			
			pitch_sum += pitch_temp1;
			roll_sum  += roll_temp1;
		}
		
		pitch_temp1 = pitch_sum / 3.0;	 //取出平均值
		roll_temp1  = roll_sum  / 3.0;	 //取出平均值

		pitch_sum = 0.0;
		roll_sum = 0.0;
		
		EulerAngle.Pitch = Kalman_Filter1(pitch_temp1,Axis.GyroY);       //卡尔曼滤波器
		EulerAngle.Roll  = Kalman_Filter2(roll_temp1,-Axis.GyroX);       //卡尔曼滤波器
		
		M1.CurPos = EulerAngle.Pitch; 
		M2.CurPos = EulerAngle.Roll;						   
		
		//计算速度
		M1.CurSpeed = M1.CurPos - M1.PrevPos;
		M1.PrevPos = M1.CurPos;				
		
		M2.CurSpeed = M2.CurPos - M2.PrevPos;
		M2.PrevPos = M2.CurPos;	

		switch(CurMode)	//根据题目选择函数
		{	
			case 1: Mode_1(); break;
			case 2: Mode_2(); break;
			case 3: Mode_3(); break;
			case 4: Mode_4(); break;
			case 5: Mode_5(); break;
			case 6: Mode_6(); break;
			default:break;
		}
				
		TIM_ClearITPendingBit(TIM5,TIM_IT_Update);		
	}
	GPIOE->BRR = GPIO_Pin_3;					 	
}

