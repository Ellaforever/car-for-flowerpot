#ifndef __OLED_IIC_H
#define __OLED_IIC_H

#include "sys.h"

/**********************************************************************************
****宏定义
**********************************************************************************/
#define IIC_SCL PAout(4)// 
#define IIC_SDA PAout(3)// 

#define high 1
#define low 0

#define	Brightness	0xCF 
#define X_WIDTH 	128
#define Y_WIDTH 	64


/**********************************************************************************
****函数声明
**********************************************************************************/
void delay(u16 z);
void GPIO_IIC_Start(void);
void GPIO_IIC_Stop(void);
void GPIO_IIC_SendOneByte(u8 txd);
void OLED_WriteDat(u8 IIC_Data);
void OLED_WriteCmd(u8 IIC_Command);
void OLED_SetPos(u8 x, u8 y);
void OLED_Fill(u8 FillData);
void OLED_CS(void);
void OLED_Init(void);
void OLED_P6x8Str(u8 x, u8 y, u8 ch[]);
void OLED_P8x16Str(u8 x,u8 y,u8 ch[]);
void OLED_P16x16Ch(u8 x,u8 y,u8 N);
void OLED_DrawBMP(u8 x0,u8 y0,u8 x1,u8 y1,const u8 BMP[]);
#endif



