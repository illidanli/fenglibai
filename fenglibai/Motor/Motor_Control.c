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
#include "motor_pwm.h"
#include "motor_pid.h"
#include "stdlib.h"
#include "stdio.h"
#include "delay.h"
#include "math.h"
#include "mpu6050.h"
#include "ahrs.h"
/*------------------------------------------
 				全局变量				
------------------------------------------*/
M1TypeDef M1;
M2TypeDef M2;

extern PIDTypdDef M1PID;
extern PIDTypdDef M2PID;

extern MPU6050_AxisTypeDef    Axis;  //MPU6050数据结构体
extern AHRS_EulerAngleTypeDef EulerAngle;
float R = 35.0; 					 //半径设置(cm)
float angle = 40.0;					 //摆动角度设置(°)
uint8_t RoundDir = 0; 				 //正反转控制
/*------------------------------------------
 函数功能:控制器软件复位
 函数说明:强制复位			
------------------------------------------*/
void MCU_Reset(void) 
{  
	__set_FAULTMASK(1);   // 关闭所有中断
 	NVIC_SystemReset();   // 复位
}
/*------------------------------------------
 函数功能:初始化M1结构体参数
 函数说明:			
------------------------------------------*/
void M1TypeDef_Init(void)
{
	M1.CurPos    = 0.0;
	M1.PrevPos   = 0.0;
	M1.CurAcc    = 0.0;
	M1.PrevSpeed = 0.0;
 	M1.Offset    = 0.1;   //允许偏差量
	M1.CurSpeed  = 0.0;  //当前速度矢量
	M1.PWM = 0;	         //PWM
}
/*------------------------------------------
 函数功能:初始化M2结构体参数
 函数说明:			
------------------------------------------*/
void M2TypeDef_Init(void)
{
	M2.CurPos    = 0.0;
	M2.PrevPos   = 0.0;
	M2.CurAcc    = 0.0;
	M2.PrevSpeed = 0.0;
 	M2.Offset    = 0.1;   //允许偏差量
	M2.CurSpeed  = 0.0;  //当前速度矢量
	M2.PWM = 0;	         //PWM		
}
/*------------------------------------------
 函数功能:
------------------------------------------*/
void Mode_0(void)
{
	
		
}
/*------------------------------------------
 函数功能:第1问PID计算
 函数说明:
------------------------------------------*/
void Mode_1(void)
{
	const float priod = 1410.0;  //单摆周期(毫秒)
	static uint32_t MoveTimeCnt = 0;
	float set_y = 0.0;
	float A = 0.0;
	float Normalization = 0.0;
	float Omega = 0.0;
				
	MoveTimeCnt += 5;							 //每5ms运算1次
	Normalization = (float)MoveTimeCnt / priod;	 //对单摆周期归一化
	Omega = 2.0*3.14159*Normalization;			 //对2π进行归一化处理
	A = atan((R/88.0f))*57.2958f;				 //根据摆幅求出角度A,88为摆杆距离地面长度cm
	set_y = A*sin(Omega);                        //计算出当前摆角 	
		
	PID_M1_SetPoint(0);			//X方向PID定位目标值0
	PID_M1_SetKp(60);	
	PID_M1_SetKi(0.79);	 
	PID_M1_SetKd(800);
	
	PID_M2_SetPoint(set_y);		//Y方向PID跟踪目标值sin
	PID_M2_SetKp(60);    
	PID_M2_SetKi(0.79);		
	PID_M2_SetKd(800); 	 
	
	M1.PWM = PID_M1_PosLocCalc(M1.CurPos);	//Pitch
	M2.PWM = PID_M2_PosLocCalc(M2.CurPos); //Roll
	
	if(M1.PWM > POWER_MAX)  M1.PWM =  POWER_MAX;
	if(M1.PWM < -POWER_MAX) M1.PWM = -POWER_MAX;	
	
	if(M2.PWM > POWER_MAX)  M2.PWM = POWER_MAX;
	if(M2.PWM < -POWER_MAX) M2.PWM = -POWER_MAX;		
	
	MotorMove(M1.PWM,M2.PWM);
}
/*------------------------------------------
 函数功能:第2问PID计算
 函数说明:
------------------------------------------*/
void Mode_2(void)
{
	const float priod = 1410.0;  //单摆周期(毫秒)
	static uint32_t MoveTimeCnt = 0;
	float set_x = 0.0;
	float A = 0.0;
	float Normalization = 0.0;
	float Omega = 0.0;				
	MoveTimeCnt += 5;							 //每5ms运算1次
	Normalization = (float)MoveTimeCnt / priod;	 //对单摆周期归一化
	Omega = 2.0*3.14159*Normalization;			 //对2π进行归一化处理
	A = atan((R/88.0f))*57.2958f;//根据摆幅求出角度A,88为摆杆离地高度
	set_x = A*sin(Omega);                        //计算出当前摆角 			
	PID_M1_SetPoint(set_x);	//X方向PID跟踪目标值sin
	PID_M1_SetKp(60);	
	PID_M1_SetKi(0.79);	 
	PID_M1_SetKd(800);	
	PID_M2_SetPoint(0);		//Y方向PID定位目标值0
	PID_M2_SetKp(60);    
	PID_M2_SetKi(0.79);		
	PID_M2_SetKd(800); 	 	
	M1.PWM = PID_M1_PosLocCalc(M1.CurPos);	//X方向PID计算
	M2.PWM = PID_M2_PosLocCalc(M2.CurPos);  //Y方向PID计算	
	if(M1.PWM > POWER_MAX) M1.PWM  =  POWER_MAX;//输出限幅
	if(M1.PWM < -POWER_MAX) M1.PWM = -POWER_MAX; 	
	if(M2.PWM > POWER_MAX) M2.PWM  =  POWER_MAX;
	if(M2.PWM < -POWER_MAX) M2.PWM = -POWER_MAX;			
	MotorMove(M1.PWM,M2.PWM);//电机输出
}
/*------------------------------------------
 函数功能:第3问PID计算
 函数说明:
------------------------------------------*/ 
void Mode_3(void)
{
	const float priod = 1410.0;  //单摆周期(毫秒)
	             //相位补偿 0, 10   20   30   40   50   60   70   80   90   100  110  120  130  140  150  160  170 180
	const float Phase[19]= {0,-0.1,-0.05,0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,0.05,0.05,0.05,0.07,0};
	static uint32_t MoveTimeCnt = 0;
	float set_x = 0.0;
	float set_y = 0.0;
	float Ax = 0.0;
	float Ay = 0.0;
	float A = 0.0;
	uint32_t pOffset = 0;
	float Normalization = 0.0;
	float Omega = 0.0;
	
	pOffset = (uint32_t)(angle/10.0f);			 //相位补偿数组下标
	MoveTimeCnt += 5;							 //每5ms运算1次
	Normalization = (float)MoveTimeCnt / priod;	 //对单摆周期归一化
	Omega = 2.0*3.14159*Normalization;			 //对2π进行归一化处理
	A = atan((R/88.0f))*57.2958f;//根据摆幅求出角度A,88为摆杆离地高度                   						
	Ax = A*cos(angle*0.017453);	 //计算出X方向摆幅分量0.017453为弧度转换
	Ay = A*sin(angle*0.017453);	 //计算出Y方向摆幅分量
	set_x = Ax*sin(Omega); 		 //计算出X方向当前摆角
	set_y = Ay*sin(Omega+Phase[pOffset]); //计算出Y方向当前摆角
		
	PID_M1_SetPoint(set_x);	//X方向PID跟踪目标值sin
	PID_M1_SetKp(60);	
	PID_M1_SetKi(0.79);	 
	PID_M1_SetKd(800);

	PID_M2_SetPoint(set_y);	//Y方向PID跟踪目标值sin
	PID_M2_SetKp(60);    
	PID_M2_SetKi(0.79);		
	PID_M2_SetKd(800); 	 
	
	M1.PWM = PID_M1_PosLocCalc(M1.CurPos);	//Pitch
	M2.PWM = PID_M2_PosLocCalc(M2.CurPos);  //Roll
	
	if(M1.PWM > POWER_MAX)  M1.PWM =  POWER_MAX;
	if(M1.PWM < -POWER_MAX) M1.PWM = -POWER_MAX;
			 	
	if(M2.PWM > POWER_MAX)  M2.PWM =  POWER_MAX;
	if(M2.PWM < -POWER_MAX) M2.PWM = -POWER_MAX;		

	MotorMove(M1.PWM,M2.PWM);	
}
/*------------------------------------------
 函数功能:第4问PID计算
 函数说明:
------------------------------------------*/ 
void Mode_4(void)
{	
	if(abs(M1.CurPos)<45.0 && abs(M2.CurPos)<45.0)	//小于45度才进行制动
	{		
		PID_M1_SetPoint(0);	  //X方向PID定位目标值0
		PID_M1_SetKp(85); 		
		PID_M1_SetKi(0);     
		PID_M1_SetKd(2000);

		PID_M2_SetPoint(0);	  //Y方向PID定位目标值0
		PID_M2_SetKp(85);  		
		PID_M2_SetKi(0);    
		PID_M2_SetKd(2000);
			
		M1.PWM = PID_M1_PosLocCalc(M1.CurPos); //Pitch
		M2.PWM = PID_M2_PosLocCalc(M2.CurPos); //Roll
		
		if(M1.PWM > POWER_MAX)  M1.PWM =  POWER_MAX;
		if(M1.PWM < -POWER_MAX) M1.PWM = -POWER_MAX;

		if(M2.PWM > POWER_MAX)  M2.PWM =  POWER_MAX;
		if(M2.PWM < -POWER_MAX) M2.PWM = -POWER_MAX;
	}
	else	
	{
	 	M1.PWM = 0;
		M2.PWM = 0;	
	}
	
	MotorMove(M1.PWM,M2.PWM);
}
/*------------------------------------------
 函数功能:第5问PID计算
 函数说明:
------------------------------------------*/
void Mode_5(void)
{
	const float priod = 1410.0;  //单摆周期(毫秒)
	static uint32_t MoveTimeCnt = 0;
	float set_x = 0.0;
	float set_y = 0.0;
	float A = 0.0;
	float phase = 0.0;
	float Normalization = 0.0;
	float Omega = 0.0;
	
	MoveTimeCnt += 5;							 //每5ms运算1次
	Normalization = (float)MoveTimeCnt / priod;	 //对单摆周期归一化
	Omega = 2.0*3.14159*Normalization;			 //对2π进行归一化处理				
	A = atan((R/88.0f))*57.2958f;    //根据半径求出对应的振幅A
	
	if(RoundDir == 0)       	  
		phase = 3.141592/2.0;		 //逆时针旋转相位差90° 
	else if(RoundDir == 1)  
		phase = (3.0*3.141592)/2.0;	 //顺时针旋转相位差270°
	
	set_x = A*sin(Omega);			 //计算出X方向当前摆角
	set_y = A*sin(Omega+phase); 	 //计算出Y方向当前摆角
	 
	PID_M1_SetPoint(set_x);	//X方向PID跟踪目标值sin
	PID_M1_SetKp(60);	
	PID_M1_SetKi(0.79);	 
	PID_M1_SetKd(800);

	PID_M2_SetPoint(set_y);	//Y方向PID跟踪目标值cos
	PID_M2_SetKp(60);    
	PID_M2_SetKi(0.79);		
	PID_M2_SetKd(800); 		 
	
	M1.PWM = PID_M1_PosLocCalc(M1.CurPos); //Pitch
	M2.PWM = PID_M2_PosLocCalc(M2.CurPos); //Roll
	
	if(M1.PWM > POWER_MAX)  M1.PWM =  POWER_MAX;
	if(M1.PWM < -POWER_MAX) M1.PWM = -POWER_MAX;
			 	
	if(M2.PWM > POWER_MAX)  M2.PWM =  POWER_MAX;
	if(M2.PWM < -POWER_MAX) M2.PWM = -POWER_MAX;		

	MotorMove(M1.PWM,M2.PWM);
	
}
/*------------------------------------------
 函数功能:第6问PID计算
 函数说明:
------------------------------------------*/
void Mode_6(void)
{

}
/*------------------------------------------
 函数功能:电机底层驱动函数
 函数说明:
------------------------------------------*/
void MotorMove(int32_t pwm1,int32_t pwm2)
{
	if(pwm1 > 0)
	{
	 	PWM_M2_Forward(pwm1);
		PWM_M4_Backward(pwm1);
	}
	else if(pwm1 < 0)
	{
	 	PWM_M2_Backward(abs(pwm1));
		PWM_M4_Forward(abs(pwm1));	
	}

	if(pwm2 > 0)
	{
	 	PWM_M1_Forward(pwm2);
		PWM_M3_Backward(pwm2);
	}
	else if(pwm2 < 0)
	{
	 	PWM_M1_Backward(abs(pwm2));
		PWM_M3_Forward(abs(pwm2));	
	} 	
}


