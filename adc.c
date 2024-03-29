/*********头文件*******************************************/
#include "adc.h"
#include "oled_iic.h"

/*************全局变量************************/
u16 ADCConvertedValue[CHANNEL_NUM];  //存ADC采到的一个数据
double ams[CHANNEL_NUM];
double current_data[CHANNEL_NUM];  //总共4个通道 采10次
u8 ADC_INT_Flag = 1;
double sum[CHANNEL_NUM];
u8 SAMU_N = 0;
u8 time = 0;
float adc_temp;
float battery;
/***********************************************************/



//定时器初始化
//中断配置
void NVIC_Init_Adc(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel=DMA1_Channel1_IRQn;
 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3; //优先级高
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


void ADC_GPIO_Configuration(void)//ADC配置函数
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1|RCC_APB2Periph_GPIOA,ENABLE);//使能ADC和GPIOB时钟
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1 |GPIO_Pin_5;//管脚0
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;//模拟输入模式
	GPIO_Init(GPIOA,&GPIO_InitStructure);//GPIO组
}

void TIM1_ConfiguraTIon(void)
{

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	TIM_TimeBaseStructure.TIM_Period=2249;//设置3200HZ,312.5us一次TIM2比较的周期
	TIM_TimeBaseStructure.TIM_Prescaler=9;//系统主频72M,这里分频9,相当于7.2M的定时器2时钟
	TIM_TimeBaseStructure.TIM_ClockDivision=0x0;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;//下面详细说明
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;//TIM_OutputState_Disable;
	TIM_OCInitStructure.TIM_Pulse=600;
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_Low;//如果是PWM1要为Low,PWM2则为High
	TIM_OC2Init(TIM1,&TIM_OCInitStructure);
	//TIM_InternalClockConfig(TIM2);

	//TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);

	//TIM_UpdateDisableConfig(TIM2,DISABLE);

}

//ADC_DMA初始化配置

void ADC_DMA_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;//注：ADC为12位模数转换器,只有ADCConvertedValue的低12位有效
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);//使能DMA时钟
	
	DMA_DeInit(DMA1_Channel1);//开启DMA1的第一通道
	
	DMA_InitStructure.DMA_PeripheralBaseAddr=(u32)&ADC1->DR;//DMA对应的外设基地址
	DMA_InitStructure.DMA_MemoryBaseAddr=(uint32_t)&ADCConvertedValue;//内存存储基地址
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;//DMA的转换模式为SRC模式,由外设搬移到内存
	DMA_InitStructure.DMA_BufferSize=CHANNEL_NUM;//DMA缓存大小,1个
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;//接收一次数据后,设备地址禁止后移
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable; //修改：使能 ；关闭接收一次数据后,目标内存地址后移
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;//定义外设数据宽度为16位
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;//DMA搬移数据尺寸,HalfWord就是为16位
	DMA_InitStructure.DMA_Mode=DMA_Mode_Circular;//循环转换模式
	DMA_InitStructure.DMA_Priority=DMA_Priority_High;//DMA优先级高
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;//M2M模式禁用
	DMA_Init(DMA1_Channel1,&DMA_InitStructure);

	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);//使能传输完成中断

}

//ADC初始化

void PulseSenosrInit(void)
{
	//当外部触发信号被选为ADC规则或注入转换时,只有它的上升沿可以启动转换
	ADC_InitTypeDef ADC_InitStructure;
	ADC_GPIO_Configuration();//IO口配置
	TIM1_ConfiguraTIon();//定时器配置
	ADC_DMA_Config();//ADC_DMA配置
	
	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;//独立的转换模式ADC_DUALMOD［3:0］=0000;
	ADC_InitStructure.ADC_ScanConvMode=ENABLE;//关闭扫描模式因为只有一个通道
	ADC_InitStructure.ADC_ContinuousConvMode=DISABLE;//关闭连续转换模式否则只要触发一次,后续的转换就会永不停歇(除非CONT清0),这样第一次以后的ADC,就不是由TIM2_CC2来触发了
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_T1_CC2;//软件转换模式
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;//对齐方式,ADC为12位中,右对齐方式ADC_ALIGN=0;
	ADC_InitStructure.ADC_NbrOfChannel=CHANNEL_NUM;//开启通道数,1个ADC_SQR1［23:20］=0000;
	//ADC_SQR1［23:20］设置通道数目的选择
	ADC_Init(ADC1,&ADC_InitStructure);
	//RCC_ADCCLKConfig(RCC_PCLK2_Div6);//配置时钟(12MHz),在RCC里面还应配置APB2=AHB时钟72MHz
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1,1, ADC_SampleTime_1Cycles5);
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_3,2, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5,2, ADC_SampleTime_1Cycles5);
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_7,4, ADC_SampleTime_1Cycles5);
	//ADC_SMPR2ADC_SMPR1设置每个通道的采样时间
	//ADC_SQR1［19:0］DC_SQR1［29:0］DC_SQR3［29:0］设置对应通道的转换顺序适用于多通道采样
	//ADC通道组,第3个通道采样顺序1,转换时间

	ADC_ExternalTrigConvCmd(ADC1,ENABLE);//设置外部触发模式使能(这个“外部“其实仅仅是相//对于ADC模块的外部,

	ADC_DMACmd(ADC1,ENABLE);

	ADC_Cmd(ADC1,ENABLE);//ADC命令,使能ADC_ADON=1

	ADC_ResetCalibration(ADC1);//重新校准

	while(ADC_GetResetCalibrationStatus(ADC1));//等待重新校准完成

	ADC_StartCalibration(ADC1);//开始校准ADC_RSTCAL=1;初始化校准寄存器

	while(ADC_GetCalibrationStatus(ADC1));//等待校准完成ADC_CAL=0;

	//ADC_SoftwareStartConvCmd(ADC1,ENABLE);//连续转换开始,ADC通过DMA方式不断的更新RAM区。

	//ADC_SWSTART=1开始规则转换切记软件触发也属于外部事件要设置ADC_EXTTRIG=1

	//////实际上还是在STM32内部)
	TIM_Cmd(TIM1,ENABLE);//最后面打开定时器使能
	TIM_CtrlPWMOutputs(TIM1,ENABLE); 
	DMA_Cmd(DMA1_Channel1,ENABLE);//使能DMA

}

//中断处理函数

void DMA1_Channel1_IRQHandler(void)
{	
	u8 i;
  if(DMA_GetITStatus(DMA1_IT_TC1)!=RESET)
	{
		if(ADC_INT_Flag == 0)
		{
			
			for(i =0;i<2;i++)
			{
				ams[i] = (double)ADCConvertedValue[i]*(3.3/4096);		
				sum[i] = sum[i] + ams[i];	
			}
							
			SAMU_N++;
			if(SAMU_N == ADC_SAMPLE_NUM) //30 times
			{
				ADC_INT_Flag = 1;	
				for(i = 0;i <2;i++)
				{
					current_data[i] = sum[i]/ADC_SAMPLE_NUM;
					sum[i] = 0;
				}				
				SAMU_N = 0;
			}
	//自己的中断处理代码但是记住程序不要太复杂最好不要超过中断时间
		}
		DMA_ClearITPendingBit(DMA1_IT_TC1);
	}
}



void adc_pros(void)
{

	if(ADC_INT_Flag == 1)
	{
		ADC_INT_Flag = 0;
		adc_temp = current_data[0]; //湿度
		battery = current_data[1];//电量				
		adc_temp = -20.622*adc_temp +99;	
		battery = 57.3 * battery -34.1;
	}
		
}

///*******************************************************
//*函数名称：getAdcValue
//*函数功能：得到ADC的值
//*输入：u8 axis 进行转换的ADC1的第几个通道
//*输出：当前电压值
//**********************************************************/
//double getAdcValue(u8 axis)  
//{  
//	u8 count,i,j;  
//	double value_store;
//	double sum=0;
//	double temp;
//	
//	/*排序*/
//	for(i=0;i<ADC_SAMPLE_NUM-1;i++)
//	{  
//		for(j=0;j<ADC_SAMPLE_NUM-i-1;j++)
//		{  
//			if(current_data[j][axis] > current_data[j+1][axis] )  
//				{  
//					temp = current_data[j][axis];  
//					current_data[j][axis] = current_data[j+1][axis];  
//					current_data[j+1][axis] = temp;    
//				}  
//		}  
//	}  
//  
//	for(count=FILING_TIME;count<ADC_SAMPLE_NUM-FILING_TIME;count++)
//	{
//		sum += current_data[count][axis];  		
//	}
//	
//	value_store =sum/(ADC_SAMPLE_NUM-2*FILING_TIME);

//	return value_store;  
//}

