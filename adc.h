#ifndef __ADC_H
#define __ADC_H

#include "sys.h"

#define ADC_SAMPLE_NUM	30
//#define FILING_TIME    10
#define CHANNEL_NUM   2     //ת��ͨ����

void TIM1_ConfiguraTIon(void);
void ADC_DMA_Config(void);
void PulseSenosrInit(void); 
void NVIC_Init_Adc(void);  
void ADC_GPIO_Configuration(void);//ADC���ú���
void adc_pros(void);
double getAdcValue(u8 axis);
#endif
