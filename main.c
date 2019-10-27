 #include<stm32f10x.h>
#include "sys.h"
#include "delay.h"
#include "oled_iic.h"
#include "vl6180x.h"//vl6180x stands for YL_69
#include "vl6180x_1.h"
#include "myiic.h"
#include "stdio.h"
#include "myiic_1.h"
#include "car.h"
#include "timer.h"
#include "PWM_Output.h"
#include "usart.h"
#include "adc.h"


extern float adc_temp;
extern float battery;

int main(void)
{	
	extern const unsigned char BMP_tired[];
	extern const unsigned char BMP_happy[];
	extern const unsigned char BMP_ill[];
	extern const unsigned char BMP_thirsty[];
	extern const unsigned char BMP_cool[];
	
	u8 data_id;
	u8 flag;
	u8 distance;
	u16 light;
	u16 light_1;
	u32 light_time;
	float light_intensity;//����ʱ�����
	u8 light_demand;
	u32 dark_time;
	u16 adcx;

	u8 send_buf[20];
	
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
	//LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�
 	//Adc_Init();		  		//ADC��ʼ��
	
	delay_init();//��ʱ������ʼ��
	delay_ms(1000);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	//oled��ʼ��
	OLED_Init();
  OLED_CS();//����	
	//OLED_P8x16Str(0,0,"hello!");
	
	//VL6180X��ʼ��
	IIC_Init();
	IIC_1_Init();
	VL6180X_Init();
	VL6180X_1_Init();
	Sys_Delay_Init(72);			
  
/****ADC�ɼ���ʼ��*******************/
	NVIC_Init_Adc();
	PulseSenosrInit();
	
	
  Timer_Init();
	TIM3_PWM_Init(2000);
	Car_GPIO_Config();
	light_time = 3;
	while(1)
	{
		
				
		/*********************************
		*********ʪ�ȴ�����ת��************
		**********************************/
		
		//light_demand = 5;
		flag += 1;
		if (flag == 40)
		{
			flag = 0;
			OLED_CS();//����	
		}
		
//		adcx=Get_Adc_Average(ADC_Channel_1,10);
//		adc_temp=(float)adcx*(3.3/4096);
//		adc_temp = -20.622*adc_temp +99;
		
		adc_pros();  //�õ�ʪ�Ⱥ͵�ص�ѹ
		if(adc_temp > 40)
		{
			GPIO_SetBits(GPIOA,GPIO_Pin_8);
		}
		else
		{
			GPIO_ResetBits(GPIOA,GPIO_Pin_8);
    }
		
		/*********************************
		*********���պ;������************
		**********************************/
		data_id = VL6180X_CheckID();  // 0x03
		distance = RangePollingRead();
		//sprintf(send_buf,"dis:%d   ",distance);
		//OLED_P8x16Str(0,2,send_buf);
		
		light = ligthPollingRead();
		//sprintf(send_buf,"lightf:%d   ",light);
		//OLED_P8x16Str(0,4,send_buf);
		light_1 = ligthPollingRead_1();
//		sprintf(send_buf,"light:%d   ",light_1);
//		OLED_P8x16Str(0,6,send_buf);
//			if (light_1 < 30000)
//			{
//				dark_time += 1;
//			}
//			else
//			{
//				light_time += 1;	
//			}		
//			if(dark_time >= 200) 
//				light_time = 0;
		/*********************************
		***********��ʾemoji**************
		**********************************/
		if (flag <= 20)
		{
			sprintf(send_buf,"battery:%.lf %%  ",battery);
			OLED_P8x16Str(0,0,send_buf);
			sprintf(send_buf,"light:%d   ",light_1);
			OLED_P8x16Str(0,2,send_buf);
			sprintf(send_buf,"moisture:%.1lf   ",adc_temp);
			OLED_P8x16Str(0,4,send_buf);
			if (light_1 < 30000)
			{
				dark_time += 1;
			}
			else
			{
				light_time += 1;	
			}		
			if(dark_time >= 200) 
			{
				light_time = 0;
				dark_time  = 0;
			}		
			if (light_time >= 100)
			{
				sprintf(send_buf,"intensity:100 %% ");
				OLED_P8x16Str(0,6,send_buf);
			}
			else
			{
				sprintf(send_buf,"intensity:%d %% ",light_time);
			  OLED_P8x16Str(0,6,send_buf);		
			}
		} 	
		else 
		{
			if (flag == 21)
				OLED_CS();//����
			if (light_1 < 30000)
				{
					dark_time += 1;
				}
			else
			{
				light_time += 1;	
			}		
			if(dark_time >= 200) 
			{	
			  light_time = 0;
				dark_time  = 0;
			}		
			if(adc_temp <= 40)
			{
				if (light_1 < 30000)
				{
					if(light_time < 100)
						OLED_DrawBMP(0,0,128,8,BMP_ill);
					else
						OLED_DrawBMP(0,0,128,8,BMP_thirsty);
				}
				else
				{
					if(light_time < 100)
						OLED_DrawBMP(0,0,128,8,BMP_ill);
					else
						OLED_DrawBMP(0,0,128,8,BMP_thirsty);
				}			
			}
			else 
			{
				if (light_1 < 30000)
				{
					if(light_time < 100)
						OLED_DrawBMP(0,0,128,8,BMP_tired);
					else
						OLED_DrawBMP(0,0,128,8,BMP_cool);
			
				}
				else
				{
					if(light_time < 100)
						OLED_DrawBMP(0,0,128,8,BMP_happy);
					else
						OLED_DrawBMP(0,0,128,8,BMP_cool);
			
				}			
			}
			
			
			
			
			
			
//			if( adc_temp < 40)//ȱˮ
//			{
//				//OLED_CS();//����	
//				OLED_DrawBMP(0,0,128,8,BMP_thirsty);
//			}
//			else if(light_1 < 30000 && (adc_temp >= 40) && (dark_time >= 100))//ȱ��
//			{
//				//OLED_CS();//����	
//				OLED_DrawBMP(0,0,128,8,BMP_tired);
//			}
//			else if ( (adc_temp >= 40) && (light_time < 100) && (dark_time <= 100) )//��ȱˮҲ��ȱ��
//			{
//				//OLED_CS();//����
//				OLED_DrawBMP(0,0,128,8,BMP_happy);
//			}
//			else if ((adc_temp >= 40) && (light_time >= 100) && (dark_time <= 100))//��ȱˮҲ��ȱ���ҽ��ܹ���ʱ�����
//			{
//				OLED_DrawBMP(0,0,128,8,BMP_cool);
//			}
//			else
//				OLED_DrawBMP(0,0,128,8,BMP_ill);//ȱ��+ȱˮ
		}
		
		

//			
		
		
		/*********************************
		**************����****************
		**********************************/
		if (distance > 200)
		{
//			Default();
//			TIM3_PWMoutPA6_Config(1500);a
//			TIM3_PWMoutPA7_Config(1500);
//			TIM3_PWMoutPB0_Config(1500);
//			TIM3_PWMoutPB1_Config(1500);
			
			if ((light_1 > 20000) ||(light_1 == 20000) )
			{
				
//				sprintf(send_buf,"intensity:%d %   ",light_time);
//				OLED_P8x16Str(0,6,send_buf);				
				Spin();
				TIM3_PWMoutPA6_Config(500);
			  TIM3_PWMoutPA7_Config(500);
				TIM3_PWMoutPB0_Config(2000);
				TIM3_PWMoutPB1_Config(2000);
				delay_ms(1000);
				Default();
				TIM3_PWMoutPA6_Config(2000);
				TIM3_PWMoutPA7_Config(2000);
				TIM3_PWMoutPB0_Config(2000);
				TIM3_PWMoutPB1_Config(2000);
				delay_ms(5000);
			}
			
			else if ((light > light_1) && (light_1 < 30000)) //ǰ������ǿ�ȸ���ǰ��
			{
				//dark_time += 1;
				Default();	
				TIM3_PWMoutPA6_Config(1500);
				TIM3_PWMoutPA7_Config(1500);
				TIM3_PWMoutPB0_Config(1500);
				TIM3_PWMoutPB1_Config(1500);
			}
			else 
			{
				//dark_time += 1;
				Default();
				TIM3_PWMoutPA6_Config(1500);
				TIM3_PWMoutPA7_Config(1500);
				TIM3_PWMoutPB0_Config(1500);
				TIM3_PWMoutPB1_Config(1500);
//				TIM3_PWMoutPA6_Config(1500);
//				TIM3_PWMoutPA7_Config(1500);
//				TIM3_PWMoutPB0_Config(1500);
//				TIM3_PWMoutPB1_Config(1500);
			}
		}
		else if (distance < 50)//���ϣ�����̫С
		{
			//dark_time += 1;
			TIM3_PWMoutPA6_Config(2000);
			TIM3_PWMoutPA7_Config(2000);
			TIM3_PWMoutPB0_Config(2000);
			TIM3_PWMoutPB1_Config(2000);
			Back();
			delay_ms(600);
			Default();
			//delay_ms(100);
		}
		else//���ϣ���ת
		{
			//dark_time += 1;
//			Default();
//			TIM3_PWMoutPA6_Config(2000);
//			TIM3_PWMoutPA7_Config(2000);   
//			TIM3_PWMoutPB0_Config(1500);
//			TIM3_PWMoutPB1_Config(1500);
//			Right();
			  Spin();
				TIM3_PWMoutPA6_Config(500);
			  TIM3_PWMoutPA7_Config(500);
				TIM3_PWMoutPB0_Config(2000);
				TIM3_PWMoutPB1_Config(2000);
				delay_ms(300);
			
		}
		
	}
}



