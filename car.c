#include<stm32f10x.h>
#include"car.h"
#include"delay.h"
#define IN0 GPIO_Pin_8
#define IN1 GPIO_Pin_9
#define IN2 GPIO_Pin_10
#define IN3 GPIO_Pin_11
#define IN4 GPIO_Pin_12


void Car_GPIO_Config()
{
GPIO_InitTypeDef GPIO_InitStructure;
RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
GPIO_InitStructure.GPIO_Pin = IN0|IN1|IN2|IN3|IN4;//ֱ�ӽ����е�GPIO�����ó�ͬ������
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;//GPIO��ƽ�任������
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//����GPIO�Ĺ���ģʽ
GPIO_Init(GPIOA, &GPIO_InitStructure);//����GPIO_Init�����ϲ�����������	
}

void Go()
{
GPIO_SetBits(GPIOA,IN1);
GPIO_ResetBits(GPIOA,IN2);
GPIO_SetBits(GPIOA,IN3);
GPIO_ResetBits(GPIOA,IN4);
}
void Left()
{
GPIO_ResetBits(GPIOA,IN1);
GPIO_SetBits(GPIOA,IN2);
GPIO_SetBits(GPIOA,IN3);
GPIO_ResetBits(GPIOA,IN4);

}
void Right()
{
GPIO_ResetBits(GPIOA,IN1);
GPIO_ResetBits(GPIOA,IN2);
GPIO_SetBits(GPIOA,IN3);
GPIO_SetBits(GPIOA,IN4);
}
void Spin()
{
//GPIO_SetBits(GPIOA,IN1);
//GPIO_SetBits(GPIOA,IN2);
//GPIO_ResetBits(GPIOB,GPIO_Pin_0);
//GPIO_ResetBits(GPIOB,GPIO_Pin_1);
	GPIO_SetBits(GPIOA,IN1);
	GPIO_SetBits(GPIOA,IN2);
	GPIO_ResetBits(GPIOA,IN3);
	GPIO_ResetBits(GPIOA,IN4);
	
}
void Stop()
{
 GPIO_ResetBits(GPIOA,IN1);
 GPIO_ResetBits(GPIOA,IN2);
 GPIO_ResetBits(GPIOA,IN3);
 GPIO_ResetBits(GPIOA,IN4);
}
void Back()
{/*
 GPIO_ResetBits(GPIOA,IN1);
 GPIO_ResetBits(GPIOA,IN2);
 GPIO_ResetBits(GPIOA,IN3);
 GPIO_ResetBits(GPIOA,IN4);*/
 GPIO_ResetBits(GPIOA,IN1);
 GPIO_ResetBits(GPIOA,IN2);
 GPIO_ResetBits(GPIOA,IN3);
 GPIO_ResetBits(GPIOA,IN4);
}
void Default()
{/*
 GPIO_ResetBits(GPIOA,IN1);
 GPIO_ResetBits(GPIOA,IN2);
 GPIO_ResetBits(GPIOA,IN3);
 GPIO_ResetBits(GPIOA,IN4);*/
 GPIO_SetBits(GPIOA,IN1);
 GPIO_SetBits(GPIOA,IN2);
 GPIO_SetBits(GPIOA,IN3);
 GPIO_SetBits(GPIOA,IN4);
}
