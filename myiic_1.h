#ifndef __MYIIC_1_H
#define __MYIIC_1_H
#include "sys.h"
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
//IO��������
 
#define SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)8<<28;}
#define SDA_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)3<<28;}

//IO��������	 
#define IIC_SCL_1    PBout(4) //SCL
#define IIC_SDA_1   PBout(5) //SDA	 
#define READ_SDA_1   PBin(5)  //����SDA 
*/
//IO��������
 
#define SDA_1_IN()  {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=(u32)8<<12;}
#define SDA_1_OUT() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=(u32)3<<12;}

//IO��������	 
#define IIC_SCL_1    PBout(10) //SCL
#define IIC_SDA_1    PBout(11) //SDA	 
#define READ_SDA_1   PBin(11)  //����SDA


//IIC���в�������
void IIC_1_Init(void);                //��ʼ��IIC��IO��				 
void IIC_1_Start(void);				//����IIC��ʼ�ź�
void IIC_1_Stop(void);	  			//����IICֹͣ�ź�
void IIC_1_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC_1_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_1_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_1_Ack(void);					//IIC����ACK�ź�
void IIC_1_NAck(void);				//IIC������ACK�ź�

void IIC_1_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_1_Read_One_Byte(u8 daddr,u8 addr);	  
#endif
















