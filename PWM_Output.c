#include<stm32f10x.h>
#include<stm32f10x_tim.h>
#include"PWM_Output.h"
/*********************************************************************************
 * �ļ���  ��PWMoutput.c
 * ����    ��ʹ��TIM3�����·�ɵ�PWM        
 * Ӳ�����ӣ�---------------------
 *          |  PA.06: (TIM3_CH1)  |
 *          |  PA.07: (TIM3_CH2)  |
 *      	  |  PB.00: (TIM3_CH3)  | 
 *    		  |  PB.01: (TIM3_CH4)  |
 *           ---------------------    			
 * ��汾  ��ST3.5.0
**********************************************************************************/

/**@����1:TIM3_GPIO_Config()**************************/
/**
 * ����  ������TIM3�������PWMʱ�õ���I/O
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����
 **/
static void TIM3_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

	/* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 	//PCLK1����2��Ƶ����ΪTIM3��ʱ��Դ����72MHz

  /* GPIOA and GPIOB clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE); 

  /*GPIOA Configuration: TIM3 channel 1 and 2 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // �����������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /*GPIOB Configuration: TIM3 channel 3 and 4 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // �����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	

}

/**@����2:TIM3_Mode_Config()**************************/
/**
 * ����  ������TIM3�����PWM�źŵ�ģʽ�������ڡ�����
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����
 **/
static void TIM3_Mode_Config(u32 TimeingPeriod)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
/* -----------------------------------------------------------------------
    TIM3 Configuration: generate 4 PWM signals with 4 different duty cycles:
    TIM3CLK = 72 MHz, Prescaler = 0x0, TIM3 counter clock = 72 MHz
    TIM3 ARR Register = 999 => TIM3 Frequency = TIM3 counter clock/(ARR + 1)
    TIM3 Frequency = 72 KHz.
  ----------------------------------------------------------------------- */

  /* Time base configuration */		 
  TIM_TimeBaseStructure.TIM_Period = TimeingPeriod-1;       //����ʱ����0������999����Ϊ1000�Σ�Ϊһ����ʱ����
  TIM_TimeBaseStructure.TIM_Prescaler = 0;	    //����Ԥ��Ƶ����Ԥ��Ƶ����Ϊ72MHz
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;	//����ʱ�ӷ�Ƶϵ��������Ƶ
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //���ϼ���ģʽ
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);                   //ʹ�ܶ�ʱ��3	
}

/*+++++++++++++++++++++++++++++ռ�ձ�����ģ��+++++++++++++++++++++++++++++++++++++++++++++++++
    TIM3 Channel1 duty cycle = (TIM3_CCR1/ TIM3_ARR)* 100 = 50%
    TIM3 Channel2 duty cycle = (TIM3_CCR2/ TIM3_ARR)* 100 = 37.5%
    TIM3 Channel3 duty cycle = (TIM3_CCR3/ TIM3_ARR)* 100 = 25%
    TIM3 Channel4 duty cycle = (TIM3_CCR4/ TIM3_ARR)* 100 = 12.5%
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void TIM3_PWMoutPA6_Config(u16 CCR1_Val)
{
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	/* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //����ΪPWMģʽ1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;	   //��������ֵ�������������������ֵʱ����ƽ��������
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ

  TIM_OC1Init(TIM3, &TIM_OCInitStructure);	 //ʹ��ͨ��1
  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM3, ENABLE);			 // ʹ��TIM3���ؼĴ���ARR
}
void TIM3_PWMoutPA7_Config(u16 CCR2_Val)
{

	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* PWM1 Mode configuration: Channel2 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //����ΪPWMģʽ1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR2_Val;	  //����ͨ��2�ĵ�ƽ����ֵ���������һ��ռ�ձȵ�PWM
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ

  TIM_OC2Init(TIM3, &TIM_OCInitStructure);	  //ʹ��ͨ��2
  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM3, ENABLE);			 // ʹ��TIM3���ؼĴ���ARR
}
void TIM3_PWMoutPB0_Config(	u16 CCR3_Val)
{
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	/* PWM1 Mode configuration: Channel2 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //����ΪPWMģʽ1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR3_Val;	  //����ͨ��3�ĵ�ƽ����ֵ���������һ��ռ�ձȵ�PWM
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ

  TIM_OC3Init(TIM3, &TIM_OCInitStructure);	  //ʹ��ͨ��3
  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM3, ENABLE);			 // ʹ��TIM3���ؼĴ���ARR

}
void TIM3_PWMoutPB1_Config(u16 CCR4_Val)
{
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	/* PWM1 Mode configuration: Channel2 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //����ΪPWMģʽ1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR4_Val;	  //����ͨ��4�ĵ�ƽ����ֵ���������һ��ռ�ձȵ�PWM
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ

  TIM_OC4Init(TIM3, &TIM_OCInitStructure);	  //ʹ��ͨ��4
  TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM3, ENABLE);			 // ʹ��TIM3���ؼĴ���ARR
}

/**@����3:TIM3_PWM_Init()**************************/
/**
 * ����  ��TIM3 ���PWM�źų�ʼ��
 * ����  �����������PWM�����йصĶ�ʱʱ�����Timeing_Period
 * ���  ����
 * ����  ���ⲿ����
 **/
void TIM3_PWM_Init(u32 Timeing_Period)
{
	TIM3_GPIO_Config();
	TIM3_Mode_Config(Timeing_Period);	
}

/*********************************************END OF FILE**********************************/