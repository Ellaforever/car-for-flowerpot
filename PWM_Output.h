#ifndef __PWM_OUTPUT_H__
#define __PWM_OUTPUT_H__
#include<stm32f10x.h>
void TIM3_PWM_Init(u32 Timeing_Period);

void TIM3_PWMoutPA6_Config(u16 CCR1_Val);
void TIM3_PWMoutPA7_Config(u16 CCR2_Val);
void TIM3_PWMoutPB0_Config(u16 CCR3_Val);
void TIM3_PWMoutPB1_Config(u16 CCR4_Val);
void TIM3_PWMoutPA9_Config(u16 CCR1_Val);
void TIM3_PWMoutPA10_Config(u16 CCR2_Val);
void TIM3_PWMoutPA11_Config(u16 CCR3_Val);
void TIM3_PWMoutPA12_Config(u16 CCR4_Val);

#endif
