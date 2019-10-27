/*********ͷ�ļ�*******************************************/
#include "adc.h"
#include "oled_iic.h"

/*************ȫ�ֱ���************************/
u16 ADCConvertedValue[CHANNEL_NUM];  //��ADC�ɵ���һ������
double ams[CHANNEL_NUM];
double current_data[CHANNEL_NUM];  //�ܹ�4��ͨ�� ��10��
u8 ADC_INT_Flag = 1;
double sum[CHANNEL_NUM];
u8 SAMU_N = 0;
u8 time = 0;
float adc_temp;
float battery;
/***********************************************************/



//��ʱ����ʼ��
//�ж�����
void NVIC_Init_Adc(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel=DMA1_Channel1_IRQn;
 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3; //���ȼ���
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


void ADC_GPIO_Configuration(void)//ADC���ú���
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1|RCC_APB2Periph_GPIOA,ENABLE);//ʹ��ADC��GPIOBʱ��
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1 |GPIO_Pin_5;//�ܽ�0
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;//ģ������ģʽ
	GPIO_Init(GPIOA,&GPIO_InitStructure);//GPIO��
}

void TIM1_ConfiguraTIon(void)
{

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	TIM_TimeBaseStructure.TIM_Period=2249;//����3200HZ,312.5usһ��TIM2�Ƚϵ�����
	TIM_TimeBaseStructure.TIM_Prescaler=9;//ϵͳ��Ƶ72M,�����Ƶ9,�൱��7.2M�Ķ�ʱ��2ʱ��
	TIM_TimeBaseStructure.TIM_ClockDivision=0x0;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;//������ϸ˵��
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;//TIM_OutputState_Disable;
	TIM_OCInitStructure.TIM_Pulse=600;
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_Low;//�����PWM1ҪΪLow,PWM2��ΪHigh
	TIM_OC2Init(TIM1,&TIM_OCInitStructure);
	//TIM_InternalClockConfig(TIM2);

	//TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);

	//TIM_UpdateDisableConfig(TIM2,DISABLE);

}

//ADC_DMA��ʼ������

void ADC_DMA_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;//ע��ADCΪ12λģ��ת����,ֻ��ADCConvertedValue�ĵ�12λ��Ч
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);//ʹ��DMAʱ��
	
	DMA_DeInit(DMA1_Channel1);//����DMA1�ĵ�һͨ��
	
	DMA_InitStructure.DMA_PeripheralBaseAddr=(u32)&ADC1->DR;//DMA��Ӧ���������ַ
	DMA_InitStructure.DMA_MemoryBaseAddr=(uint32_t)&ADCConvertedValue;//�ڴ�洢����ַ
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;//DMA��ת��ģʽΪSRCģʽ,��������Ƶ��ڴ�
	DMA_InitStructure.DMA_BufferSize=CHANNEL_NUM;//DMA�����С,1��
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;//����һ�����ݺ�,�豸��ַ��ֹ����
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable; //�޸ģ�ʹ�� ���رս���һ�����ݺ�,Ŀ���ڴ��ַ����
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;//�����������ݿ��Ϊ16λ
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;//DMA�������ݳߴ�,HalfWord����Ϊ16λ
	DMA_InitStructure.DMA_Mode=DMA_Mode_Circular;//ѭ��ת��ģʽ
	DMA_InitStructure.DMA_Priority=DMA_Priority_High;//DMA���ȼ���
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;//M2Mģʽ����
	DMA_Init(DMA1_Channel1,&DMA_InitStructure);

	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);//ʹ�ܴ�������ж�

}

//ADC��ʼ��

void PulseSenosrInit(void)
{
	//���ⲿ�����źű�ѡΪADC�����ע��ת��ʱ,ֻ�����������ؿ�������ת��
	ADC_InitTypeDef ADC_InitStructure;
	ADC_GPIO_Configuration();//IO������
	TIM1_ConfiguraTIon();//��ʱ������
	ADC_DMA_Config();//ADC_DMA����
	
	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;//������ת��ģʽADC_DUALMOD��3:0��=0000;
	ADC_InitStructure.ADC_ScanConvMode=ENABLE;//�ر�ɨ��ģʽ��Ϊֻ��һ��ͨ��
	ADC_InitStructure.ADC_ContinuousConvMode=DISABLE;//�ر�����ת��ģʽ����ֻҪ����һ��,������ת���ͻ�����ͣЪ(����CONT��0),������һ���Ժ��ADC,�Ͳ�����TIM2_CC2��������
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_T1_CC2;//���ת��ģʽ
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;//���뷽ʽ,ADCΪ12λ��,�Ҷ��뷽ʽADC_ALIGN=0;
	ADC_InitStructure.ADC_NbrOfChannel=CHANNEL_NUM;//����ͨ����,1��ADC_SQR1��23:20��=0000;
	//ADC_SQR1��23:20������ͨ����Ŀ��ѡ��
	ADC_Init(ADC1,&ADC_InitStructure);
	//RCC_ADCCLKConfig(RCC_PCLK2_Div6);//����ʱ��(12MHz),��RCC���滹Ӧ����APB2=AHBʱ��72MHz
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1,1, ADC_SampleTime_1Cycles5);
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_3,2, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5,2, ADC_SampleTime_1Cycles5);
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_7,4, ADC_SampleTime_1Cycles5);
	//ADC_SMPR2ADC_SMPR1����ÿ��ͨ���Ĳ���ʱ��
	//ADC_SQR1��19:0�ݡADC_SQR1��29:0�ݡADC_SQR3��29:0�����ö�Ӧͨ����ת��˳�������ڶ�ͨ������
	//ADCͨ����,��3��ͨ������˳��1,ת��ʱ��

	ADC_ExternalTrigConvCmd(ADC1,ENABLE);//�����ⲿ����ģʽʹ��(������ⲿ����ʵ��������//����ADCģ����ⲿ,

	ADC_DMACmd(ADC1,ENABLE);

	ADC_Cmd(ADC1,ENABLE);//ADC����,ʹ��ADC_ADON=1

	ADC_ResetCalibration(ADC1);//����У׼

	while(ADC_GetResetCalibrationStatus(ADC1));//�ȴ�����У׼���

	ADC_StartCalibration(ADC1);//��ʼУ׼ADC_RSTCAL=1;��ʼ��У׼�Ĵ���

	while(ADC_GetCalibrationStatus(ADC1));//�ȴ�У׼���ADC_CAL=0;

	//ADC_SoftwareStartConvCmd(ADC1,ENABLE);//����ת����ʼ,ADCͨ��DMA��ʽ���ϵĸ���RAM����

	//ADC_SWSTART=1��ʼ����ת���м��������Ҳ�����ⲿ�¼�Ҫ����ADC_EXTTRIG=1

	//////ʵ���ϻ�����STM32�ڲ�)
	TIM_Cmd(TIM1,ENABLE);//�����򿪶�ʱ��ʹ��
	TIM_CtrlPWMOutputs(TIM1,ENABLE); 
	DMA_Cmd(DMA1_Channel1,ENABLE);//ʹ��DMA

}

//�жϴ�����

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
	//�Լ����жϴ�����뵫�Ǽ�ס����Ҫ̫������ò�Ҫ�����ж�ʱ��
		}
		DMA_ClearITPendingBit(DMA1_IT_TC1);
	}
}



void adc_pros(void)
{

	if(ADC_INT_Flag == 1)
	{
		ADC_INT_Flag = 0;
		adc_temp = current_data[0]; //ʪ��
		battery = current_data[1];//����				
		adc_temp = -20.622*adc_temp +99;	
		battery = 57.3 * battery -34.1;
	}
		
}

///*******************************************************
//*�������ƣ�getAdcValue
//*�������ܣ��õ�ADC��ֵ
//*���룺u8 axis ����ת����ADC1�ĵڼ���ͨ��
//*�������ǰ��ѹֵ
//**********************************************************/
//double getAdcValue(u8 axis)  
//{  
//	u8 count,i,j;  
//	double value_store;
//	double sum=0;
//	double temp;
//	
//	/*����*/
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

