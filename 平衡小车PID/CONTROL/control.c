#include "control.h"	
#include "filter.h"	
  /**************************************************************************
���ߣ�ƽ��С��֮��
�ҵ��Ա�С�꣺http://shop114407458.taobao.com/
**************************************************************************/
int Balance_Pwm,Velocity_Pwm,Turn_Pwm;
u8 Flag_Target;
u32 Flash_R_Count;
int Voltage_Temp,Voltage_Count,Voltage_All;
/**************************************************************************
�������ܣ����еĿ��ƴ��붼��������
         5ms��ʱ�ж���MPU6050��INT���Ŵ���
         �ϸ�֤���������ݴ����ʱ��ͬ��				 
**************************************************************************/
int EXTI15_10_IRQHandler(void) 
{    
	 if(INT==0)		
	{   
		  EXTI->PR=1<<12;                                                      //����жϱ�־λ   
		   Flag_Target=!Flag_Target;
		  if(Flag_Target==1)                                                  //5ms��ȡһ�������Ǻͼ��ٶȼƵ�ֵ�����ߵĲ���Ƶ�ʿ��Ը��ƿ������˲��ͻ����˲���Ч��
			{
				Get_Angle(Way_Angle);                                               //===������̬		
			  return 0;	  
			}                                                                   //10ms����һ�Σ�Ϊ�˱�֤M�����ٵ�ʱ���׼�����ȶ�ȡ����������
			
			Voltage_Temp=Get_battery_volt();		                                //=====��ȡ��ص�ѹ		
			Voltage_Count++;                                                    //=====ƽ��ֵ������
			Voltage_All+=Voltage_Temp;                                          //=====��β����ۻ�
			if(Voltage_Count==100) Voltage=Voltage_All/100,Voltage_All=0,Voltage_Count=0;//=====��ƽ��ֵ	
			
			Encoder_Left=Read_Encoder(2);                                      //===��ȡ��������ֵ����Ϊ�����������ת��180�ȵģ����Զ�����һ��ȡ������֤�������һ��
			Encoder_Right=Read_Encoder(4);                                      //===��ȡ��������ֵ 
	  	Get_Angle(Way_Angle);                                               //===������̬	
  		Led_Flash(100);                                 								     //===LED��˸;����ģʽ 1s�ı�һ��ָʾ�Ƶ�״̬	
 			Balance_Pwm =balance(Angle_Balance,Gyro_Balance);                   //===ƽ��PID����	
		  Velocity_Pwm=velocity(Encoder_Left,Encoder_Right);                  //===�ٶȻ�PID����	 ��ס���ٶȷ�����������������С�����ʱ��Ҫ����������Ҫ���ܿ�һ��
 		  Moto1=Balance_Pwm-Velocity_Pwm-Turn_Pwm;                            //===�������ֵ������PWM
 	  	Moto2=Balance_Pwm-Velocity_Pwm+Turn_Pwm;                            //===�������ֵ������PWM
   		Xianfu_Pwm();                                                       //===PWM�޷�
 			Set_Pwm(Moto1,Moto2);                                               //===��ֵ��PWM�Ĵ���  
	}       	
	 return 0;	  
} 

/**************************************************************************
�������ܣ�ֱ��PD����
��ڲ������Ƕȡ����ٶ�
����  ֵ��ֱ������PWM
��    �ߣ�ƽ��С��֮��
**************************************************************************/
int balance(float Angle,float Gyro)
{  
   float Bias;
	 int balance;
	 Bias=Angle-ZHONGZHI;    									   //===���ƽ��ĽǶ���ֵ �ͻ�е���
	 balance=Balance_Kp*Bias+Gyro*Balance_Kd;   //===����ƽ����Ƶĵ��PWM  PD����   kp��Pϵ�� kd��Dϵ�� 
	 return balance;
}

/**************************************************************************
�������ܣ��ٶ�PI���� �޸�ǰ������ң���ٶȣ�����Target_Velocity��
��ڲ��������ֱ����������ֱ�����
����  ֵ���ٶȿ���PWM
��    �ߣ�ƽ��С��֮��
**************************************************************************/
int velocity(int encoder_left,int encoder_right)
{  
     static float Velocity,Encoder_Least,Encoder,Movement;
	  static float Encoder_Integral,Target_Velocity=80;
	  //=============ң��ǰ�����˲���=======================// 
		if(1==Flag_Qian)    	Movement=Target_Velocity;	      //===ǰ����־λ��1 
		else if(1==Flag_Hou)	Movement=-Target_Velocity;         //===���˱�־λ��1
	  else  Movement=0;	
	  if(Bi_zhang==1&&Flag_Left!=1&&Flag_Right!=1)        //�������ģʽ
		{
		   if(Distance<500)  Movement=-Target_Velocity;
		}	
		if(Bi_zhang==2&&Flag_Left!=1&&Flag_Right!=1)        //�������ģʽ
		{
		   if(Distance>100&&Distance<300)  Movement=Target_Velocity;
		}
   //=============�ٶ�PI������=======================//	
		Encoder_Least =(encoder_left+encoder_right)-0;                    //===��ȡ�����ٶ�ƫ��==�����ٶȣ����ұ�����֮�ͣ�-Ŀ���ٶȣ��˴�Ϊ�㣩 
		Encoder *= 0.8;		                                                //===һ�׵�ͨ�˲���       
		Encoder += Encoder_Least*0.2;	                                    //===һ�׵�ͨ�˲���    
		Encoder_Integral +=Encoder;                                       //===���ֳ�λ�� ����ʱ�䣺10ms
		Encoder_Integral=Encoder_Integral-Movement;                       //===����ң�������ݣ�����ǰ������
		if(Encoder_Integral>5500)  	Encoder_Integral=5500;             		//===�����޷�����ֹ����̫����������������Ŀ���ʧ��
		if(Encoder_Integral<-5500)	Encoder_Integral=-5500;               //===�����޷�	
		Velocity=Encoder*Velocity_Kp+Encoder_Integral*Velocity_Ki;        //===�ٶȿ���	
	  return Velocity;
}


/**************************************************************************
�������ܣ���ֵ��PWM�Ĵ���
��ڲ���������PWM������PWM
����  ֵ����
**************************************************************************/
void Set_Pwm(int moto1,int moto2)
{
   int siqu=500;//��������
			if(moto1<0)			BIN2=0,			BIN1=1;
			else 	          BIN2=1,			BIN1=0;
			PWMB=myabs(moto1)+siqu;
		  if(moto2<0)	AIN1=0,			AIN2=1;
			else        AIN1=1,			AIN2=0;
			PWMA=myabs(moto2)+siqu;	
}
/**************************************************************************
�������ܣ�����PWM��ֵ 
��ڲ�������
����  ֵ����
**************************************************************************/
void Xianfu_Pwm(void)
{	
	  int Amplitude=6900;    //===PWM������7200 ������6900
    if(Moto1<-Amplitude) Moto1=-Amplitude;	
		if(Moto1>Amplitude)  Moto1=Amplitude;	
	  if(Moto2<-Amplitude) Moto2=-Amplitude;	
		if(Moto2>Amplitude)  Moto2=Amplitude;		
}

	
/**************************************************************************
�������ܣ���ȡ�Ƕ� �����㷨�������ǵĵ�У�����ǳ����� 
��ڲ�������ȡ�Ƕȵ��㷨 1��DMP  2�������� 3�������˲�
����  ֵ����
**************************************************************************/
void Get_Angle(u8 way)
{ 
	   	Temperature=Read_Temperature();      //===��ȡMPU6050�����¶ȴ��������ݣ����Ʊ�ʾ�����¶ȡ�
	    if(way==1)                           //===DMP�Ķ�ȡ�����ݲɼ��ж϶�ȡ���ϸ���ѭʱ��Ҫ��
			{	
					Read_DMP();                      //===��ȡ���ٶȡ����ٶȡ����
					Angle_Balance=-Roll;             //===����ƽ�����
					Gyro_Balance=-gyro[0];            //===����ƽ����ٶȣ�ԭʼֵ��
//					Gyro_Turn=gyro[2];               //===����ת����ٶ�
//				  Acceleration_Z=accel[2];         //===����Z����ٶȼ�
			}			
}
/**************************************************************************
�������ܣ�����ֵ����
��ڲ�����int
����  ֵ��unsigned int
**************************************************************************/
int myabs(int a)
{ 		   
	  int temp;
		if(a<0)  temp=-a;  
	  else temp=a;
	  return temp;
}
