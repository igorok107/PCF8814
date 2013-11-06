//***************************************************************************
//  File........: PCF8814.h
//  Author(s)...: Chiper
//  Porting.....: Igorok107
//  URL(s)......: http://digitalchip.ru
//  Description.: Драйвер LCD-контроллера от Nokia1100 с графическими функциями
//  Data........: 02.11.13
//  Version.....: 2.1.0
//***************************************************************************
#ifndef PCF8814_H
#define PCF8814_H

#include "Arduino.h"
//******************************************************************************
//******************************************************************************
// Применять полный набор символов. 
#define FULL_CHARSET 

// *****!!!!! Минимальная задержка, при которой работает LCD-контроллер
#define LCD_MIN_DELAY	4
// *****!!!!! Подбирается экспериментально под конкретный контроллер

// Макросы, определения, служебные переменные
#define SCLK_LCD_SET    digitalWrite(LCD_SCLK,HIGH)
#define SDA_LCD_SET     digitalWrite(LCD_SDA,HIGH)
#define CS_LCD_SET      digitalWrite(LCD_CS,HIGH)
#define RST_LCD_SET     digitalWrite(LCD_RST,HIGH)
#define SCLK_LCD_RESET  digitalWrite(LCD_SCLK,LOW)
#define SDA_LCD_RESET   digitalWrite(LCD_SDA,LOW)
#define CS_LCD_RESET    digitalWrite(LCD_CS,LOW)
#define RST_LCD_RESET   digitalWrite(LCD_RST,LOW)

// Макросы для работы с битами
#define ClearBit(reg, bit)       reg &= (~(1<<(bit)))
#define SetBit(reg, bit)         reg |= (1<<(bit))	
#define InvBit(reg, bit)         reg ^= 1<<bit	

#define CMD_LCD_MODE	0
#define DATA_LCD_MODE	1

#define PIXEL_ON	0
#define PIXEL_OFF	1
#define PIXEL_INV 	2

#define FILL_OFF	0
#define FILL_ON		1

#define INV_MODE_ON	0
#define INV_MODE_OFF	1

#define ON	1
#define OFF	0

// Разрешение дисплея в пикселях
#define LCD_X_RES	96		// разрешение по горизонтали
#define LCD_Y_RES	68		// разрешение по вертикали

//******************************************************************************
class PCF8814 {
	public:
		PCF8814(byte _LCD_SCLK, byte _LCD_SDA, byte _LCD_CS, byte _LCD_RST);
		void Init(void);
		void Clear(void);
		void Mirror(byte x, byte y);
		void Contrast(byte c = 0x0D);
		void SendByte(char mode,unsigned char c);
		void Putc(unsigned char c);
		void PutcWide(unsigned char c);
		void Print(const char * message);
		void PrintF(char * message);
		void PrintWide(char * message);
		void GotoXY(byte x = 0,byte y = 0);
		void Inverse(byte mode);
		void Pixel (byte x,byte y, byte pixel_mode);
		void Line  (byte x1,byte y1, byte x2,byte y2, byte pixel_mode);
		void Circle(byte x, byte y, byte radius, byte fill, byte pixel_mode);
		void Rect  (byte x1, byte y1, byte x2, byte y2, byte fill, byte pixel_mode);
		void Pict  (byte x, byte y, byte * picture);
	private:
		void GotoXY_pix(byte x,byte y);
};
#endif /* PCF8814_H */