/*-----------------------------------------------
		独立按键端口配置及检测程序
	说明:	已经将端口配置和按键扫描程序封装成型
	        根据程序实际需要更改相应的参数即可。
	时间:	2013年7月22日 BoX编写 
------------------------------------------------*/
#include "indkey.h"
#include "stm32f10x.h"
#include "delay.h"
#include "stdio.h"
#include "mpu6050.h"
#include "ahrs.h"
#include "usart2.h"
/*-----------------------------------------
				全局变量
------------------------------------------*/
uint8_t Item = 0;
uint8_t Q1_Start = 0;
uint8_t Q2_Start = 0;
uint8_t Q3_Start = 0;
uint8_t Q4_Start = 0;
uint8_t Q5_Start = 0;
uint8_t Q6_Start = 0;
uint8_t Q7_Start = 0;

extern MPU6050_AxisTypeDef    Axis;  //MPU6050数据结构体
extern AHRS_EulerAngleTypeDef EulerAngle;
extern char buf[512];	 //液晶屏数据缓冲区
extern float R;
extern float angle;
extern uint8_t RoundDir;
extern uint8_t CurMode; 
/*-----------------------------------------
				KEY IO配置
------------------------------------------*/
void Key_IO_Init(void)	 //按键IO配置
{
  GPIO_InitTypeDef IO_Init;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);	
  IO_Init.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;				    
  IO_Init.GPIO_Mode = GPIO_Mode_IPU;	
  GPIO_Init(GPIOA, &IO_Init);
}
/*-----------------------------------------------
	函数功能:	独立按键检测
	函数参数:	端口组名GPIOx,引脚名GPIO_Pin
	函数回值:	INT8U类型 按键值0,1
------------------------------------------------*/
void KeyScan(void)
{	
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4) == KEY_PRESSED) //K1
	{	
		switch(Item)
		{
			case 2:R+=5.0;
				   if(R >= 35.0) R = 35.0;
				   sprintf(buf,"DS16(6,60,'设置长度:%.1f ',10)\r\n",R);
				   GpuSend(buf);	
			       break;  //第2问按下S4增加距离

			case 3:angle+=10.0;
				   if(angle >= 180.0) 
				   	  angle = 180.0;
				   sprintf(buf,"DS16(6,80,'设置角度:%.1f ',10)\r\n",angle);
				   GpuSend(buf);	
			       break;  //第3问按下S4增加角度;  
			
			case 5:R+=5.0;
				   if(R >= 35.0) R = 35.0;
				   sprintf(buf,"DS16(6,100,'设置半径:%.1f ',10)\r\n",R);
				   GpuSend(buf);
				   break;

			case 6:R+=5.0;
				   if(R >= 35.0) R = 35.0;
				   sprintf(buf,"DS16(6,100,'设置半径:%.1f ',10)\r\n",R);
				   GpuSend(buf);
				   break;
				   

			default:break;
		}				
	}

	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5) == KEY_PRESSED) //K2
	{
		switch(Item)
		{
			case 2:R-=5.0;
				   if(R <= 15.0) R = 15.0;
				   sprintf(buf,"DS16(6,60,'设置长度:%.1f ',10)\r\n",R);
				   GpuSend(buf);	
			       break;  //第2问按下S4增加距离

			case 3:angle-=10.0;
				   if(angle <= 0.0) 
				   	  angle = 0.0;
				   sprintf(buf,"DS16(6,80,'设置角度:%.1f ',10)\r\n",angle);
				   GpuSend(buf);	
			       break;  //第3问按下S4增加角度;  
			
			case 5:R-=5.0;
				   if(R <= 15.0) R = 15.0;
				   sprintf(buf,"DS16(6,100,'设置半径:%.1f ',10)\r\n",R);
				   GpuSend(buf);
				   break;

			case 6:R-=5.0;
				   if(R <= 15.0) R = 15.0;
				   sprintf(buf,"DS16(6,100,'设置半径:%.1f ',10)\r\n",R);
				   GpuSend(buf);
				   break;
				   

			default:break;
		}		
	}
	
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6) == KEY_PRESSED) //K3
	{
		switch(Item)
		{
			case 1:Q1_Start = 1;
				   sprintf(buf,"DS16(6,120,'开始!',10)\r\n");
				   GpuSend(buf);
				   break; 

			case 2:Q2_Start = 1;
				   sprintf(buf,"DS16(6,120,'开始!',10)\r\n");
				   GpuSend(buf);
				   break;

			case 3:Q3_Start = 1;
				   sprintf(buf,"DS16(6,120,'开始!',10)\r\n");
				   GpuSend(buf);
				   break;  

			case 4:Q4_Start = 1;
				   sprintf(buf,"DS16(6,120,'开始!',10)\r\n");
				   GpuSend(buf);break;
				   
			case 5:Q5_Start = 1;
				   RoundDir = !RoundDir;
				   if(RoundDir == 1)
			       	   sprintf(buf,"DS16(6,120,'顺时针旋转!',10)\r\n");
				   else
				   	   sprintf(buf,"DS16(6,120,'逆时针旋转!',10)\r\n");
				   GpuSend(buf);break;
				   
			case 6:Q6_Start = 1;	       
			       sprintf(buf,"DS16(6,120,'开始!',10)\r\n");
				   GpuSend(buf);break;   

			default:break;
		}	
	} 	

	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7) == KEY_PRESSED)  //K4
	{
		Item++;
		if(Item>6)		//共有7问
			Item = 0;
		sprintf(buf,"DS16(6,40,'第%d问',10)\r\n",Item);
		GpuSend(buf);
	}
}

