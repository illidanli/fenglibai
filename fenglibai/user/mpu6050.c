/*-----------------------------------------------------------------------------------
												STM32F401μ?MPU6050?y?ˉ3ìDò
	
	?ò?é: ?￡?aê±DòIIC,?úí·???t?D????o?IO?ú?′?éê1ó?,?éò??2D???.
	×÷??: BoX
	°?±?: V1.1
	??ì¨: STM32F401RE-Nucleo Board
	è??ú: 2014?ê6??24è?
------------------------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "mpu6050.h"
#include "iic_analog.h"
#include "math.h"
/*------------------------------------------
							êy?Yê?3???ê?
							
	ORIGINAL_OUTPUT	
	0 - ê?3??-ê?êy?Y,2??óè?o?′|àí
	1 - ê?3?′|àíoóμ?êy?Y,?ó?ù?èμ￥???ag,???ù?è
		  μ￥???adeg/s
			
	ACC_FULLSCALE 	
	2  - è?á?3ì?y?o2G
	4  - è?á?3ì?y?o4G
	8  - è?á?3ì?y?o8G
	16 - è?á?3ì?y?o16G
	
	GYRO_FULLSCALE 	
	250  - è?á?3ì?y?o250??/s
	500  - è?á?3ì?y?o500??/s
	1000 - è?á?3ì?y?o1000??/s
	2000 - è?á?3ì?y?o2000??/s
------------------------------------------*/
#define ORIGINAL_OUTPUT			 (0)
#define ACC_FULLSCALE        	 (2)
#define GYRO_FULLSCALE			 (250)
/*------------------------------------------
								ì??t±àò?
------------------------------------------*/
#if ORIGINAL_OUTPUT == 0
	#if  ACC_FULLSCALE  == 2
		#define AccAxis_Sensitive (float)(16384)
	#elif ACC_FULLSCALE == 4
		#define AccAxis_Sensitive (float)(8192)
	#elif ACC_FULLSCALE == 8
		#define AccAxis_Sensitive (float)(4096)
	#elif ACC_FULLSCALE == 16
		#define AccAxis_Sensitive (float)(2048)
	#endif 
		
	#if   GYRO_FULLSCALE == 250
		#define GyroAxis_Sensitive (float)(131.0)
	#elif GYRO_FULLSCALE == 500
		#define GyroAxis_Sensitive (float)(65.5)
	#elif GYRO_FULLSCALE == 1000
		#define GyroAxis_Sensitive (float)(32.8)
	#elif GYRO_FULLSCALE == 2000
		#define GyroAxis_Sensitive (float)(16.4)
	#endif
		
#else
	#define AccAxis_Sensitive  (1)
	#define GyroAxis_Sensitive (1)
#endif
/*------------------------------------------
								è???±?á?
------------------------------------------*/
extern MPU6050_AxisTypeDef    Axis;
/*------------------------------------------
 oˉêy1|?ü: 3?ê??ˉMPU6050 IO?úoí??????′??÷
 oˉêy2?êy: ?T
------------------------------------------*/
void MPU6050_Init(void)
{
	IIC_GPIO_Config(IIC_GOIO_SDA,IIC_SDA,IIC_GPIO_SCL,IIC_SCL);
	
	/* 电源管理 */
	IIC_WriteData( SLAVEADRESS , PWR_MGMT_1   , 0x00 );
	
	/* 陀螺仪采样率,典型值:0x07(125Hz) */
	IIC_WriteData( SLAVEADRESS , SMPLRT_DIV   , 0x07 );
	
	/* 低通滤波器频率,(5Hz-256Hz) */
	IIC_WriteData( SLAVEADRESS , CONFIG       , 0x05 );	//10Hz
	
	/* 陀螺仪自检和测量范围,典型值:0x00(不自检,250deg/s) */
	IIC_WriteData( SLAVEADRESS , GYRO_CONFIG  , 0x00 );
	
	/*加速度自检和测量范围和高通滤波器频率,典型值0x01(不自检2G,5Hz) */
	IIC_WriteData( SLAVEADRESS , ACCEL_CONFIG , 0x01 );
}
/*------------------------------------------
 oˉêy1|?ü: ?áè?MPU6050 X?á?ó?ù?è?μ
 oˉêy2?êy: ?T
------------------------------------------*/
float getAccX(void)
{
	short accX = 0;
	char  accXH = 0 , accXL = 0;

	accXH = IIC_ReadData( SLAVEADRESS , ACCEL_XOUT_H );
	accXL = IIC_ReadData( SLAVEADRESS , ACCEL_XOUT_L );

	accX = (accXH<<8)|accXL;

	return ((float)accX / AccAxis_Sensitive);
}
/*------------------------------------------
 oˉêy1|?ü: ?áè?MPU6050 Y?á?ó?ù?è?μ
 oˉêy2?êy: ?T
------------------------------------------*/
float getAccY(void)
{
	short accY = 0;
	char  accYH = 0 , accYL = 0;

	accYH = IIC_ReadData( SLAVEADRESS , ACCEL_YOUT_H );
	accYL = IIC_ReadData( SLAVEADRESS , ACCEL_YOUT_L );

	accY = (accYH<<8)|accYL;

	return ((float)accY / AccAxis_Sensitive);
}
/*------------------------------------------
 oˉêy1|?ü: ?áè?MPU6050 Z?á?ó?ù?è?μ
 oˉêy2?êy: ?T
------------------------------------------*/
float getAccZ(void)
{
	short accZ = 0;
	char  accZH = 0 , accZL = 0;

	accZH = IIC_ReadData( SLAVEADRESS , ACCEL_ZOUT_H );
	accZL = IIC_ReadData( SLAVEADRESS , ACCEL_ZOUT_L );

	accZ = (accZH<<8)|accZL;

	return ((float)accZ / AccAxis_Sensitive);
}
/*------------------------------------------
 oˉêy1|?ü: ?áè?MPU6050 X?á???ù?è?μ
 oˉêy2?êy: ?T
------------------------------------------*/
float getGyroX(void)
{
	short gyroX = 0;
	char gyroXH = 0 , gyroXL = 0; 
	
	gyroXH = IIC_ReadData( SLAVEADRESS , GYRO_XOUT_H );
	gyroXL = IIC_ReadData( SLAVEADRESS , GYRO_XOUT_H );
	
	gyroX = (gyroXH<<8)|gyroXL;
	
	return ((float)gyroX / GyroAxis_Sensitive);	
}
/*------------------------------------------
 oˉêy1|?ü: ?áè?MPU6050 Y?á???ù?è?μ
 oˉêy2?êy: ?T
------------------------------------------*/
float getGyroY(void)
{
  short gyroY = 0;
	char gyroYH = 0 , gyroYL = 0; 
	
	gyroYH = IIC_ReadData( SLAVEADRESS , GYRO_YOUT_H );
	gyroYL = IIC_ReadData( SLAVEADRESS , GYRO_YOUT_H );
	
	gyroY = (gyroYH<<8)|gyroYL;
	
	return ((float)gyroY / GyroAxis_Sensitive);		
}
/*------------------------------------------
 oˉêy1|?ü: ?áè?MPU6050 Z?á???ù?è?μ
 oˉêy2?êy: ?T
------------------------------------------*/
float getGyroZ(void)
{
  short gyroZ = 0;
	char gyroZH = 0 , gyroZL = 0; 
	
	gyroZH = IIC_ReadData( SLAVEADRESS , GYRO_ZOUT_H );
	gyroZL = IIC_ReadData( SLAVEADRESS , GYRO_ZOUT_H );
	
	gyroZ = (gyroZH<<8)|gyroZL;
	
	return ((float)gyroZ / GyroAxis_Sensitive);		
}
/*------------------------------------------
 oˉêy1|?ü: ?áè???????
 oˉêy2?êy: 57.2958?a180/3.14,×a???a???è
------------------------------------------*/
float getFuYangAngle(void)
{
	float tmp_accx,tmp_accz,tmp_angle = 0.0;

	tmp_accx = getAccX();
	tmp_accz = getAccZ();
	tmp_angle = atan(tmp_accx / tmp_accz) * 57.2958;

	return tmp_angle;
}
/*------------------------------------------
 oˉêy1|?ü: ?áè?oá1???
 oˉêy2?êy: 57.2958?a180/3.14,×a???a???è
------------------------------------------*/
float getHengGunAngle(void)
{
	float tmp_accz,tmp_accy,tmp_angle = 0.0;

	tmp_accz = getAccZ();
	tmp_accy = getAccY();
	tmp_angle = atan(tmp_accy / tmp_accz) * 57.2958;

	return tmp_angle;
}
/*------------------------------------------
 oˉêy1|?ü: ?áè?o??ò??
 oˉêy2?êy: 57.2958?a180/3.14,×a???a???è
------------------------------------------*/
float getHangXiangAngle(void)
{
	float tmp_accz,tmp_accy,tmp_angle = 0.0;

	tmp_accz = getAccZ();
	tmp_accy = getAccY();
	tmp_angle = atan(tmp_accy / tmp_accz) * 57.2958;

	return tmp_angle;
}
/*------------------------------------------
 oˉêy1|?ü: ?áè?MPU6050???è
 oˉêy2?êy: ?T
------------------------------------------*/
short getTemperature(void)
{
 	short temperature = 0;
	char temperatureH = 0 , temperatureL = 0;

	temperatureH = IIC_ReadData( SLAVEADRESS , TEMP_OUT_H );
	temperatureL = IIC_ReadData( SLAVEADRESS , TEMP_OUT_L );

	temperature = (temperatureH<<8)|temperatureL;

	return temperature;
}

void Angle_Calculate(void)
{
 	Axis.AccX  = getAccX();
	Axis.AccY  = getAccY();
	Axis.AccZ  = getAccZ();
	Axis.GyroX = getGyroX();
	Axis.GyroY = getGyroY();
	Axis.GyroZ = getGyroZ();

	
}


