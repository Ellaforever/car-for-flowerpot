#include "myiic_1.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//IIC���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/9
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
/* 
//��ʼ��IIC
void IIC_1_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	//ʹ��GPIOBʱ��
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_4|GPIO_Pin_5); 	//PB6,PB7 �����
}
*/
void IIC_1_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	//ʹ��GPIOBʱ��
	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);  //???SWJ??,??PA15,PB4
  //DBGMCU->CR &= 0xFFFFFFDF;   //??????,??PB3   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_10|GPIO_Pin_11); 	//PB6,PB7 �����
}

//����IIC��ʼ�ź�
void IIC_1_Start(void)
{
	SDA_1_OUT();     //sda�����
	IIC_SDA_1=1;	  	  
	IIC_SCL_1=1;
	delay_us(4);
 	IIC_SDA_1=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL_1=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void IIC_1_Stop(void)
{
	SDA_1_OUT();//sda�����
	IIC_SCL_1=0;
	IIC_SDA_1=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL_1=1; 
	IIC_SDA_1=1;//����I2C���߽����ź�
	delay_us(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��0������Ӧ��ʧ��
//        1������Ӧ��ɹ�
u8 IIC_1_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_1_IN();      //SDA����Ϊ����  
	IIC_SDA_1=1;delay_us(1);	   
	IIC_SCL_1=1;delay_us(1);	 
	while(READ_SDA_1)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_1_Stop();
			return 0;
		}
	}
	IIC_SCL_1=0;//ʱ�����0 	   
	return 1;  
} 
//����ACKӦ��
void IIC_1_Ack(void)
{
	IIC_SCL_1=0;
	SDA_1_OUT();
	IIC_SDA_1=0;
	delay_us(2);
	IIC_SCL_1=1;
	delay_us(2);
	IIC_SCL_1=0;
}
//������ACKӦ��		    
void IIC_1_NAck(void)
{
	IIC_SCL_1=0;
	SDA_1_OUT();
	IIC_SDA_1=1;
	delay_us(2);
	IIC_SCL_1=1;
	delay_us(2);
	IIC_SCL_1=0;
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_1_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_1_OUT(); 	    
    IIC_SCL_1=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			IIC_SDA_1=1;
		else
			IIC_SDA_1=0;
		txd<<=1; 	  
		delay_us(2);   //��TEA5767��������ʱ���Ǳ����
		IIC_SCL_1=1;
		delay_us(2); 
		IIC_SCL_1=0;	
		delay_us(2);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC_1_Read_Byte(unsigned char ack)
{
//	unsigned char i,receive=0;
//	SDA_IN();//SDA����Ϊ����
//    for(i=0;i<8;i++ )
//	{
//        IIC_SCL=0; 
//        delay_us(2);
//		IIC_SCL=1;
//        receive<<=1;
//        if(READ_SDA)receive++;   
//		delay_us(1); 
//    }					 
//    if (!ack)
//        IIC_NAck();//����nACK
//    else
//        IIC_Ack(); //����ACK   
//    return receive;
	unsigned char i,receive=0;
	SDA_1_IN();//
	IIC_SDA_1 = 1;
	delay_us(4);
	for(i=0;i<8;i++ )
	{
		receive<<=1;
		IIC_SCL_1=0; 
		delay_us(4);
	  IIC_SCL_1=1;
		delay_us(4);
		if(READ_SDA_1)
			receive |= 0x01;   
	  delay_us(4); //1
	}	
  IIC_SCL_1 = 0;	
	return receive;

}










