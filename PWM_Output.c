#include<stm32f10x.h>
#include<stm32f10x_tim.h>
#include"PWM_Output.h"
/*********************************************************************************
 * 文件名  ：PWMoutput.c
 * 描述    ：使用TIM3输出四路可调PWM        
 * 硬件连接：---------------------
 *          |  PA.06: (TIM3_CH1)  |
 *          |  PA.07: (TIM3_CH2)  |
 *      	  |  PB.00: (TIM3_CH3)  | 
 *    		  |  PB.01: (TIM3_CH4)  |
 *           ---------------------    			
 * 库版本  ：ST3.5.0
**********************************************************************************/

/**@函数1:TIM3_GPIO_Config()**************************/
/**
 * 描述  ：配置TIM3复用输出PWM时用到的I/O
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用
 **/
static void TIM3_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

	/* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 	//PCLK1经过2倍频后作为TIM3的时钟源等于72MHz

  /* GPIOA and GPIOB clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE); 

  /*GPIOA Configuration: TIM3 channel 1 and 2 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // 复用推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /*GPIOB Configuration: TIM3 channel 3 and 4 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // 复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	

}

/**@函数2:TIM3_Mode_Config()**************************/
/**
 * 描述  ：配置TIM3输出的PWM信号的模式，如周期、极性
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用
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
  TIM_TimeBaseStructure.TIM_Period = TimeingPeriod-1;       //当定时器从0计数到999，即为1000次，为一个定时周期
  TIM_TimeBaseStructure.TIM_Prescaler = 0;	    //设置预分频：不预分频，即为72MHz
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;	//设置时钟分频系数：不分频
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);                   //使能定时器3	
}

/*+++++++++++++++++++++++++++++占空比设置模块+++++++++++++++++++++++++++++++++++++++++++++++++
    TIM3 Channel1 duty cycle = (TIM3_CCR1/ TIM3_ARR)* 100 = 50%
    TIM3 Channel2 duty cycle = (TIM3_CCR2/ TIM3_ARR)* 100 = 37.5%
    TIM3 Channel3 duty cycle = (TIM3_CCR3/ TIM3_ARR)* 100 = 25%
    TIM3 Channel4 duty cycle = (TIM3_CCR4/ TIM3_ARR)* 100 = 12.5%
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void TIM3_PWMoutPA6_Config(u16 CCR1_Val)
{
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	/* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;	   //设置跳变值，当计数器计数到这个值时，电平发生跳变
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //当定时器计数值小于CCR1_Val时为高电平

  TIM_OC1Init(TIM3, &TIM_OCInitStructure);	 //使能通道1
  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM3, ENABLE);			 // 使能TIM3重载寄存器ARR
}
void TIM3_PWMoutPA7_Config(u16 CCR2_Val)
{

	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* PWM1 Mode configuration: Channel2 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR2_Val;	  //设置通道2的电平跳变值，输出另外一个占空比的PWM
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //当定时器计数值小于CCR1_Val时为高电平

  TIM_OC2Init(TIM3, &TIM_OCInitStructure);	  //使能通道2
  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM3, ENABLE);			 // 使能TIM3重载寄存器ARR
}
void TIM3_PWMoutPB0_Config(	u16 CCR3_Val)
{
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	/* PWM1 Mode configuration: Channel2 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR3_Val;	  //设置通道3的电平跳变值，输出另外一个占空比的PWM
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //当定时器计数值小于CCR1_Val时为高电平

  TIM_OC3Init(TIM3, &TIM_OCInitStructure);	  //使能通道3
  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM3, ENABLE);			 // 使能TIM3重载寄存器ARR

}
void TIM3_PWMoutPB1_Config(u16 CCR4_Val)
{
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	/* PWM1 Mode configuration: Channel2 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR4_Val;	  //设置通道4的电平跳变值，输出另外一个占空比的PWM
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //当定时器计数值小于CCR1_Val时为高电平

  TIM_OC4Init(TIM3, &TIM_OCInitStructure);	  //使能通道4
  TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM3, ENABLE);			 // 使能TIM3重载寄存器ARR
}

/**@函数3:TIM3_PWM_Init()**************************/
/**
 * 描述  ：TIM3 输出PWM信号初始化
 * 输入  ：输入参数与PWM周期有关的定时时间参数Timeing_Period
 * 输出  ：无
 * 调用  ：外部调用
 **/
void TIM3_PWM_Init(u32 Timeing_Period)
{
	TIM3_GPIO_Config();
	TIM3_Mode_Config(Timeing_Period);	
}

/*********************************************END OF FILE**********************************/
