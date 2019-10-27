#include "oled_iic.h"
#include "CodeDat.h"
#include "delay.h"

/*********************OLED���������õ���ʱ����************************************/
void delay(u16 z)
{
	u16 x,y;
	for(x=z;x>0;x--)
	for(y=110;y>0;y--);
}

/******************************************************************************
****�������ܣ�IIC������ʼ�źź���
****�����������
****�����������
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
****�������ܣ�IIC����ֹͣ�ź�
****�����������
****�����������
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
****�������ܣ�IIC����һ���ֽ�
****���������һ���ֽ�
****�����������
******************************************************************************/
void GPIO_IIC_SendOneByte(u8 txd)
{                        
    u8 t;       
    IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			IIC_SDA=1;
		else
			IIC_SDA=0;
		txd<<=1; 	  
		delay_us(2);   //��TEA5767��������ʱ���Ǳ����
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
****�������ܣ�OLEDд����
****���������д������
****�����������
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
****�������ܣ�OLEDд����
****���������д������
****�����������
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
****�������ܣ�OLED�������꺯��
****�������������x��y
****�����������
******************************************************************************/
void OLED_SetPos(u8 x, u8 y) 
{ 
    OLED_WriteCmd(0xb0 + y);
    OLED_WriteCmd(((x & 0xf0) >> 4) | 0x10);
    OLED_WriteCmd((x & 0x0f) | 0x01);
} 

/******************************************************************************
****�������ܣ�OLEDȫ�����
****�������������ָ��
****�����������
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
****�������ܣ�OLED��������
****�����������
****�����������
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
****�������ܣ�OLED��ʼ������
****�����������
****�����������
******************************************************************************/
void OLED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//ʹ�ܻ���ʧ��APB2����ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//����������50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//�������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ������GPIOB.10�Ĵ���
	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ������GPIOB.11�Ĵ���
	
	IIC_SDA = 1;
	IIC_SCL = 1;
	
  
	delay(500);//��ʼ��֮ǰ����ʱ����Ҫ��
	OLED_WriteCmd(0xae);//--turn off oled panel
	OLED_WriteCmd(0x00);//---set low column address
	OLED_WriteCmd(0x10);//---set high column address
	OLED_WriteCmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WriteCmd(0x81);//--set contrast control register
	OLED_WriteCmd(Brightness); // Set SEG Output Current Brightness
	OLED_WriteCmd(0xa1);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
	OLED_WriteCmd(0xc8);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
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
	OLED_Fill(0x00); //��ʼ����
	OLED_SetPos(0,0);
} 


/******************************************************************************
****�������ܣ�OLED��ʾ6*8һ���׼ASCII�ַ���
****���������x��y��ʼ���꣬��Ҫ��ʾ���ַ�
****�����������
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
****�������ܣ�OLED��ʾ8*16һ���׼ASCII�ַ���
****���������x��y��ʼ���꣬��Ҫ��ʾ���ַ�
****�����������
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
****�������ܣ�OLED��ʾ���ĺ���
****���������x��y���꣬����
****�����������
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
****�������ܣ�OLEDʾBMPͼƬ128��64BMPλͼ
****���������x0��y0��ʼ���꣬x1��y2�������꣬BMP����
****�����������
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


