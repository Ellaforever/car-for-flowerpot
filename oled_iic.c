#include "oled_iic.h"
#include "CodeDat.h"
#include "delay.h"

/*********************OLED驱动程序用的延时程序************************************/
void delay(u16 z)
{
	u16 x,y;
	for(x=z;x>0;x--)
	for(y=110;y>0;y--);
}

/******************************************************************************
****函数功能：IIC产生起始信号函数
****输入参数：无
****输出参数：无
******************************************************************************/
void GPIO_IIC_Start(void)
{
	IIC_SCL = 1;
	IIC_SDA = 1;
	delay_us(4);
	IIC_SDA = 0;
	delay_us(4);
	IIC_SCL = 0;
}

/******************************************************************************
****函数功能：IIC产生停止信号
****输入参数：无
****输出参数：无
******************************************************************************/
void GPIO_IIC_Stop(void)
{
	IIC_SCL = 0;
	IIC_SDA = 0;
	delay_us(4);
	IIC_SCL = 1;
	IIC_SDA = 1;
	delay_us(4);
}

/******************************************************************************
****函数功能：IIC发送一个字节
****输入参数：一个字节
****输出参数：无
******************************************************************************/
void GPIO_IIC_SendOneByte(u8 txd)
{                        
    u8 t;       
    IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			IIC_SDA=1;
		else
			IIC_SDA=0;
		txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		delay_us(2);
    }	 
		IIC_SDA=1; 
    IIC_SCL=1;    
    IIC_SCL=0;	  
		
} 
//{
//	u8 i;
//	for(i = 0; i < 8; i++)
//	{
//		if(IIC_Byte & 0x80)
//		{
//			IIC_SDA = 1;
//		}
//		else 
//			IIC_SDA = 0;
//		IIC_SCL = 1;
//		IIC_SCL = 0;
//		IIC_Byte <<= 1;
//	}
//	IIC_SDA = 1;
//	IIC_SCL = 1;
//	IIC_SCL = 0;
//}

/******************************************************************************
****函数功能：OLED写数据
****输入参数：写的数据
****输出参数：无
******************************************************************************/
void OLED_WriteDat(u8 IIC_Data)
{
	GPIO_IIC_Start();
	GPIO_IIC_SendOneByte(0x78);
	GPIO_IIC_SendOneByte(0x40);
	GPIO_IIC_SendOneByte(IIC_Data);
	GPIO_IIC_Stop();
}

/******************************************************************************
****函数功能：OLED写命令
****输入参数：写的命令
****输出参数：无
******************************************************************************/
void OLED_WriteCmd(u8 IIC_Command)
{
    GPIO_IIC_Start();
    GPIO_IIC_SendOneByte(0x78);           
    GPIO_IIC_SendOneByte(0x00);			
    GPIO_IIC_SendOneByte(IIC_Command);
    GPIO_IIC_Stop();
}

/******************************************************************************
****函数功能：OLED设置坐标函数
****输入参数：坐标x，y
****输出参数：无
******************************************************************************/
void OLED_SetPos(u8 x, u8 y) 
{ 
    OLED_WriteCmd(0xb0 + y);
    OLED_WriteCmd(((x & 0xf0) >> 4) | 0x10);
    OLED_WriteCmd((x & 0x0f) | 0x01);
} 

/******************************************************************************
****函数功能：OLED全屏填充
****输入参数：数据指令
****输出参数：无
******************************************************************************/
void OLED_Fill(u8 FillData) 
{
	u8 y,x;
	for(y = 0; y < 8; y++)
	{
		OLED_WriteCmd(0xb0 + y);
		OLED_WriteCmd(0x01);
		OLED_WriteCmd(0x10);
		for(x = 0;x < X_WIDTH; x++)
			OLED_WriteDat(FillData);
	}
}


/******************************************************************************
****函数功能：OLED清屏函数
****输入参数：无
****输出参数：无
******************************************************************************/
void OLED_CS(void)
{
	u8 y,x;
	for(y = 0; y < 8; y++)
	{
		OLED_WriteCmd(0xb0 + y);
		OLED_WriteCmd(0x01);
		OLED_WriteCmd(0x10);
		for(x = 0; x < X_WIDTH; x++)
			OLED_WriteDat(0);
	}
}

/******************************************************************************
****函数功能：OLED初始化函数
****输入参数：无
****输出参数：无
******************************************************************************/
void OLED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使能或者失能APB2外设时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//最高输出速率50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化外设GPIOB.10寄存器
	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化外设GPIOB.11寄存器
	
	IIC_SDA = 1;
	IIC_SCL = 1;
	
  
	delay(500);//初始化之前的延时很重要！
	OLED_WriteCmd(0xae);//--turn off oled panel
	OLED_WriteCmd(0x00);//---set low column address
	OLED_WriteCmd(0x10);//---set high column address
	OLED_WriteCmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WriteCmd(0x81);//--set contrast control register
	OLED_WriteCmd(Brightness); // Set SEG Output Current Brightness
	OLED_WriteCmd(0xa1);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	OLED_WriteCmd(0xc8);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	OLED_WriteCmd(0xa6);//--set normal display
	OLED_WriteCmd(0xa8);//--set multiplex ratio(1 to 64)
	OLED_WriteCmd(0x3f);//--1/64 duty
	OLED_WriteCmd(0xd3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WriteCmd(0x00);//-not offset
	OLED_WriteCmd(0xd5);//--set display clock divide ratio/oscillator frequency
	OLED_WriteCmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WriteCmd(0xd9);//--set pre-charge period
	OLED_WriteCmd(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WriteCmd(0xda);//--set com pins hardware configuration
	OLED_WriteCmd(0x12);
	OLED_WriteCmd(0xdb);//--set vcomh
	OLED_WriteCmd(0x40);//Set VCOM Deselect Level
	OLED_WriteCmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WriteCmd(0x02);//
	OLED_WriteCmd(0x8d);//--set Charge Pump enable/disable
	OLED_WriteCmd(0x14);//--set(0x10) disable
	OLED_WriteCmd(0xa4);// Disable Entire Display On (0xa4/0xa5)
	OLED_WriteCmd(0xa6);// Disable Inverse Display On (0xa6/a7) 
	OLED_WriteCmd(0xaf);//--turn on oled panel
	OLED_Fill(0x00); //初始清屏
	OLED_SetPos(0,0);
} 


/******************************************************************************
****函数功能：OLED显示6*8一组标准ASCII字符串
****输入参数：x、y起始坐标，需要显示的字符
****输出参数：无
******************************************************************************/
void OLED_P6x8Str(u8 x, u8 y, u8 ch[])
{
	u8 c = 0, i = 0, j = 0;
	while (ch[j] != '\0')
    {
			c = ch[j] - 32;
			if(x > 126)
			{
				x = 0;
				y++;
			}
			OLED_SetPos(x, y);
			for(i = 0; i < 6; i++)
	    OLED_WriteDat(F6x8[c][i]);
			x += 6;
			j++;
    }
}

/******************************************************************************
****函数功能：OLED显示8*16一组标准ASCII字符串
****输入参数：x、y起始坐标，需要显示的字符
****输出参数：无
******************************************************************************/
void OLED_P8x16Str(u8 x,u8 y,u8 ch[])
{
    u8 c = 0, i = 0, j = 0;
    while (ch[j] != '\0')
    {
			c = ch[j] - 32;
			if(x > 120)
			{
				x = 0;
        y++;
       }
			OLED_SetPos(x, y);
			for(i = 0; i < 8; i++)
				OLED_WriteDat(F8X16[c * 16 + i]);
			OLED_SetPos(x, y + 1);
			for(i = 0; i < 8; i++)
				OLED_WriteDat(F8X16[c * 16 + i + 8]);
			x += 8;
			j++;
    }
}


/******************************************************************************
****函数功能：OLED显示中文函数
****输入参数：x、y坐标，汉字
****输出参数：无
******************************************************************************/
void OLED_P16x16Ch(u8 x,u8 y,u8 N)
{
	u8 wm = 0;
	u8 adder = 32 * N;
	OLED_SetPos(x , y);
	for(wm = 0; wm < 16; wm++)
	{
		OLED_WriteDat(F16x16[adder]);
		adder += 1;
	}
	OLED_SetPos(x, y + 1);
	for(wm = 0; wm < 16; wm++)
	{
		OLED_WriteDat(F16x16[adder]);
		adder += 1;
	} 	  	
}

/******************************************************************************
****函数功能：OLED示BMP图片128×64BMP位图
****输入参数：x0、y0起始坐标，x1、y2结束坐标，BMP数据
****输出参数：无
******************************************************************************/
void OLED_DrawBMP(u8 x0,u8 y0,u8 x1,u8 y1,const u8 BMP[])
{
	u16 j = 0;
	u8 x, y;

	if(y1 % 8 == 0) 
		y = y1 / 8;      
	else 
		y = y1 / 8 + 1;
	for(y = y0; y < y1; y++)
	{
		OLED_SetPos(x0, y);
		for(x = x0; x < x1; x++)
		{      
	    OLED_WriteDat(BMP[j++]);
		}
	}
}


