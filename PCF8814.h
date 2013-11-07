//***************************************************************************
//  File........: PCF8814.h
//  Author(s)...: Chiper
//  Porting.....: Igorok107
//  URL(s)......: http://digitalchip.ru
//  Description.: Драйвер LCD-контроллера от Nokia1100 с графическими функциями
//  Data........: 07.11.13
//  Version.....: 2.2.0
//***************************************************************************
#ifndef PCF8814_H
#define PCF8814_H

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
	#include "pins_arduino.h"
#endif
//******************************************************************************
//******************************************************************************
// Применять полный набор символов.
#define FULL_CHARSET

//******************************************************************************
//#define SOFT_SPI // Включение програмного SPI, если LCD не справляется со скоростью
//#define LCD_MIN_DELAY	50 // *****!!!!! Минимальная задержка, при которой работает LCD-контроллер
// *****!!!!! Подбирается экспериментально под конкретный контроллер

// Макросы, определения, служебные переменные
#define RST_LCD_SET     digitalWrite(LCD_RST,HIGH)
#define RST_LCD_RESET   digitalWrite(LCD_RST,LOW)
#ifdef SOFT_SPI
	#define SCLK_LCD_SET    digitalWrite(LCD_SCLK,HIGH)
	#define SDA_LCD_SET     digitalWrite(LCD_SDA,HIGH)
	#define CS_LCD_SET      digitalWrite(LCD_CS,HIGH)
	#define SCLK_LCD_RESET  digitalWrite(LCD_SCLK,LOW)
	#define SDA_LCD_RESET   digitalWrite(LCD_SDA,LOW)
	#define CS_LCD_RESET    digitalWrite(LCD_CS,LOW)
#else
	#define SPI_CLOCK_DIV	0x01 // Делитель SPI, определяет скорость. Значения [0-6] = Fosc/[4,16,64,128,2,8,32]
	#define SCLK_LCD_SET	PORTB |= _BV(PB5)
	#define SDA_LCD_SET 	PORTB |= _BV(PB3)
	#define CS_LCD_SET		PORTB |= _BV(PB2)
	#define SCLK_LCD_RESET	PORTB &= ~_BV(PB5)
	#define SDA_LCD_RESET	PORTB &= ~_BV(PB3)
	#define CS_LCD_RESET	PORTB &= ~_BV(PB2)
#endif

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
		PCF8814(uint8_t _LCD_SCLK = 13, uint8_t _LCD_SDA = 11, uint8_t _LCD_CS = 10, uint8_t _LCD_RST = 9);
		void Init(void);
		void Clear(void);
		void Mirror(uint8_t x, uint8_t y);
		void Contrast(uint8_t c = 0x0D);
		void SendByte(char mode,unsigned char c);
		void Putc(unsigned char c);
		void PutcWide(unsigned char c);
		void Print(const char * message);
		void PrintF(char * message);
		void PrintWide(char * message);
		void GotoXY(uint8_t x = 0,uint8_t y = 0);
		void Inverse(uint8_t mode);
		void Pixel (uint8_t x,uint8_t y, uint8_t pixel_mode);
		void Line  (uint8_t x1,uint8_t y1, uint8_t x2,uint8_t y2, uint8_t pixel_mode);
		void Circle(uint8_t x, uint8_t y, uint8_t radius, uint8_t fill, uint8_t pixel_mode);
		void Rect  (uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t fill, uint8_t pixel_mode);
		void Pict  (uint8_t x, uint8_t y, uint8_t * picture);
	private:
		void GotoXY_pix(uint8_t x,uint8_t y);
		uint8_t SPI_write(uint8_t cData);
};
#endif /* PCF8814_H */