//***************************************************************************
//  File........: PCF8814.h
//  Author(s)...: Chiper
//  Porting.....: Igorok107
//  URL(s)......: http://digitalchip.ru
//  Description.: Драйвер LCD-контроллера от Nokia1100 с графическими функциями
//  Data........: 07.11.13
//  Version.....: 2.2.0
//***************************************************************************
#include "PCF8814.h"
#include "PCF8814_font.h" // Подключаем шрифт (будет размещен в программной памяти)

// Видеобуфер. Работаем через буффер, так как из контроллера читать данные нельзя, а для
// графического режима нам нужно знать содержимое видеопамяти. (9 банков по 96 байт)
static uint8_t lcd_memory[LCD_X_RES][(LCD_Y_RES/8)+1];

// Тукущие координаты (указатели) в видеобуфере
// lcd_xcurr - в пикселах, lcd_ycurr- в банках (строках)
static uint8_t lcd_xcurr, lcd_ycurr;

// Порты к которым подключен дисплей в нумерации Arduino
volatile uint8_t LCD_SCLK, LCD_SDA, LCD_CS, LCD_RST;


PCF8814::PCF8814(uint8_t _LCD_SCLK, uint8_t _LCD_SDA, uint8_t _LCD_CS, uint8_t _LCD_RST)
{
	// Инициализируем пины на вывод для работы с LCD-контроллером
	pinMode(_LCD_RST,OUTPUT);
	LCD_RST		=	_LCD_RST;	
#ifdef SOFT_SPI
	pinMode(_LCD_SCLK,OUTPUT);
	pinMode(_LCD_SDA,OUTPUT);
	pinMode(_LCD_CS,OUTPUT);	
	LCD_SCLK	=	_LCD_SCLK;
	LCD_SDA		=	_LCD_SDA;
	LCD_CS		=	_LCD_CS;	
#else // Инициализация аппаратного SPI
	pinMode(SCK, OUTPUT);
	pinMode(MOSI, OUTPUT);	
	pinMode(SS, OUTPUT);
	SPCR = 0x10; //Определяем как MASTER.
	SPCR = (SPCR & ~0x03) | (SPI_CLOCK_DIV & 0x03); //Делитель частоты
	SPSR = (SPSR & ~1) | ((SPI_CLOCK_DIV >> 2) & 1);
#endif
	CS_LCD_SET;
}

uint8_t PCF8814::SPI_write(uint8_t cData) {
		SPDR = cData;
        while(!(SPSR & _BV(SPIF)));
        return SPDR;    
}
//******************************************************************************
// Передача байта (команды или данных) на LCD-контроллер
//  mode: CMD_LCD_MODE - передаем команду (0)
//		  DATA_LCD_MODE - передаем данные (1)
//  c: значение передаваемого байта
void PCF8814::SendByte(char mode,unsigned char c)
{
	#ifndef SOFT_SPI
		SPCR &= ~_BV(SPIE);
	#endif
	CS_LCD_RESET;
	if (mode)
	{
		lcd_memory[lcd_xcurr][lcd_ycurr] = c;
		lcd_xcurr++;
		if (lcd_xcurr>95)
		{
			lcd_xcurr = 0;
			lcd_ycurr++;
		}
		if (lcd_ycurr>8) lcd_ycurr = 0;
		SDA_LCD_SET;
	}
	else SDA_LCD_RESET;
	SCLK_LCD_SET;
	SCLK_LCD_RESET;
	#ifdef SOFT_SPI
		uint8_t i;
		for(i=0;i<8;i++)
		{
			delayMicroseconds(LCD_MIN_DELAY/2);
			SCLK_LCD_RESET;
			if(c & 0x80) SDA_LCD_SET;
			else	     SDA_LCD_RESET;
			delayMicroseconds(LCD_MIN_DELAY/2);
			SCLK_LCD_SET;
			c <<= 1;
		}
		SCLK_LCD_RESET;
	#else
		SPCR |= _BV(SPE);
		SPI_write(c);
		SPCR &= ~_BV(SPE);	
	#endif
	CS_LCD_SET;
	SDA_LCD_RESET;
}

//******************************************************************************
// Инициализация контроллера
void PCF8814::Init(void)
{
	SCLK_LCD_RESET;
	SDA_LCD_RESET;
	CS_LCD_RESET;
	RST_LCD_RESET;
	delay(10);            // выжидем не менее 5мс для установки генератора(менее 5 мс может неработать)
	RST_LCD_SET;
	CS_LCD_SET;

	SendByte(CMD_LCD_MODE,0xE2); // *** SOFTWARE RESET
	SendByte(CMD_LCD_MODE,0x3A); // *** Use internal oscillator
	SendByte(CMD_LCD_MODE,0xEF); // *** FRAME FREQUENCY:
	SendByte(CMD_LCD_MODE,0x04); // *** 80Hz
	SendByte(CMD_LCD_MODE,0xD0); // *** 1:65 divider
	SendByte(CMD_LCD_MODE,0x20); // Запись в регистр Vop
	SendByte(CMD_LCD_MODE,0x85); // Определяет контрастность
	SendByte(CMD_LCD_MODE,0xA4); // all on/normal display
	SendByte(CMD_LCD_MODE,0x2F); // Power control set(charge pump on/off)
	SendByte(CMD_LCD_MODE,0x40); // set start row address = 0
	SendByte(CMD_LCD_MODE,0xB0); // установить Y-адрес = 0
	SendByte(CMD_LCD_MODE,0x10); // установить X-адрес, старшие 3 бита
	SendByte(CMD_LCD_MODE,0x00);  // установить X-адрес, младшие 4 бита

	//SendByte(CMD_LCD_MODE,0xC8); // mirror Y axis (about X axis)
	SendByte(CMD_LCD_MODE,0xA1); // Инвертировать экран по горизонтали

	SendByte(CMD_LCD_MODE,0xAC); // set initial row (R0) of the display
	SendByte(CMD_LCD_MODE,0x07);
	SendByte(CMD_LCD_MODE,0xAF); // экран вкл/выкл

	Clear(); // clear LCD
}

//******************************************************************************
// Очистка экрана
void PCF8814::Clear(void)
{
	SendByte(CMD_LCD_MODE,0x40); // Y = 0
	SendByte(CMD_LCD_MODE,0xB0);
	SendByte(CMD_LCD_MODE,0x10); // X = 0
	SendByte(CMD_LCD_MODE,0x00);

	lcd_xcurr=0;
	lcd_ycurr=0;		  // Устанавливаем в 0 текущие координаты в видеобуфере

	SendByte(CMD_LCD_MODE,0xAE); // disable display;
	unsigned int i;
	for(i=0;i<864;i++) SendByte(DATA_LCD_MODE,0x00);
	SendByte(CMD_LCD_MODE,0xAF); // enable display;
}

//******************************************************************************
// Зеркалирование LCD-экрана по оси x и y соответственно.
//  ON: Отразить
//  OFF: Не отражатьж
void PCF8814::Mirror(uint8_t x, uint8_t y)
{
	SendByte(CMD_LCD_MODE,0xA0 | x);
	SendByte(CMD_LCD_MODE,0xC0 | y<<3);
}

//******************************************************************************
// Контрасиность LCD-экрана.
//  с: принимает значения от 0 до 31.
void PCF8814::Contrast(uint8_t c)
{
	if (c >= 0x20) c = 0x1F;
	SendByte(CMD_LCD_MODE,0x20);
	SendByte(CMD_LCD_MODE,0x80+c); // Определяет контрастность [0x80-0x9F]
}

//******************************************************************************
// Вывод символа на LCD-экран в текущее место
//  c: код символа
void PCF8814::Putc(unsigned char c)
{
	if (c < 208){
		uint8_t i;
		for ( i = 0; i < 5; i++ )
		SendByte(DATA_LCD_MODE,pgm_read_byte(&(lcd_Font[c-32][i])));

		SendByte(DATA_LCD_MODE,0x00); // Зазор между символами по горизонтали в 1 пиксель
	}
}

//******************************************************************************
// Вывод широкого символа на LCD-экран в текущее место
//  c: код символа
void PCF8814::PutcWide(unsigned char c)
{
	if (c < 208){ 	// Урезаем первый байт в кодировке UTF8

		uint8_t i;
		for ( i = 0; i < 5; i++ )
		{
			unsigned char glyph = pgm_read_byte(&(lcd_Font[c-32][i]));
			SendByte(DATA_LCD_MODE,glyph);
			SendByte(DATA_LCD_MODE,glyph);
		}

		SendByte(DATA_LCD_MODE,0x00); // Зазор между символами по горизонтали в 1 пиксель
		//	SendByte(DATA_LCD_MODE,0x00); // Можно сделать две линии
	}
}

//******************************************************************************
// Вывод строки символов на LCD-экран в текущее место. Если строка выходит
// за экран в текущей строке, то остаток переносится на следующую строку.
//  message: указатель на строку символов. 0x00 - признак конца строки.
void PCF8814::Print(const char * message)
{
	while (*message) Putc(*message++); // Конец строки обозначен нулем
}

//******************************************************************************
// Вывод строки символов двойной ширины на LCD-экран в текущее место
// из оперативной памяти. Если строка выходит за экран в текущей строке, то остаток
// переносится на следующую строку.
//  message: указатель на строку символов в оперативной памяти. 0x00 - признак конца строки.
void PCF8814::PrintWide(char * message)
{
	while (*message) PutcWide(*message++);  // Конец строки обозначен нулем
}

//******************************************************************************
// Вывод строки символов на LCD-экран NOKIA 1100 в текущее место из программной памяти.
// Если строка выходит за экран в текущей строке, то остаток переносится на следующую строку.
//  message: указатель на строку символов в программной памяти. 0x00 - признак конца строки.
void PCF8814::PrintF(char * message)
{
	uint8_t data;
	while (data=pgm_read_byte(message), data)
	{
		Putc(data);
		message++;
	}
}

//******************************************************************************
// Устанавливает курсор в необходимое положение. Отсчет начинается в верхнем
// левом углу. По горизонтали 16 знакомест, по вертикали - 8
//  x: 0..15
//  y: 0..7
void PCF8814::GotoXY(uint8_t x,uint8_t y)
{
	x=x*6;	// Переходим от координаты в знакоместах к координатам в пикселях

	lcd_xcurr=x;
	lcd_ycurr=y;

	SendByte(CMD_LCD_MODE,(0xB0|(y&0x0F)));      // установка адреса по Y: 0100 yyyy
	SendByte(CMD_LCD_MODE,(0x00|(x&0x0F)));      // установка адреса по X: 0000 xxxx - биты (x3 x2 x1 x0)
	SendByte(CMD_LCD_MODE,(0x10|((x>>4)&0x07))); // установка адреса по X: 0010 0xxx - биты (x6 x5 x4)

}

//******************************************************************************
// Устанавливает курсор в пикселях. Отсчет начинается в верхнем
// левом углу. По горизонтали 96 пикселей, по вертикали - 65
//  x: 0..95
//  y: 0..64
void PCF8814::GotoXY_pix(uint8_t x,uint8_t y)
{
	lcd_xcurr=x;
	lcd_ycurr=y/8;

	SendByte(CMD_LCD_MODE,(0xB0|(lcd_ycurr&0x0F)));      // установка адреса по Y: 0100 yyyy
	SendByte(CMD_LCD_MODE,(0x00|(x&0x0F)));      // установка адреса по X: 0000 xxxx - биты (x3 x2 x1 x0)
	SendByte(CMD_LCD_MODE,(0x10|((x>>4)&0x07))); // установка адреса по X: 0010 0xxx - биты (x6 x5 x4)
}

//******************************************************************************
// Вывод точки на LCD-экран
//  x: 0..95  координата по горизонтали (отсчет от верхнего левого угла)
//	y: 0..64  координата по вертикали
//	pixel_mode: PIXEL_ON  - для включения пикскела
//				PIXEL_OFF - для выключения пиксела
//				PIXEL_INV - для инверсии пиксела
void PCF8814::Pixel(uint8_t x,uint8_t y, uint8_t pixel_mode)
{
	uint8_t temp;

	GotoXY_pix(x,y);
	temp=lcd_memory[lcd_xcurr][lcd_ycurr];

	switch(pixel_mode)
	{
		case PIXEL_ON:
		SetBit(temp, y%8);			// Включаем пиксел
		break;
		case PIXEL_OFF:
		ClearBit(temp, y%8);		// Выключаем пиксел
		break;
		case PIXEL_INV:
		InvBit(temp, y%8);			// Инвертируем пиксел
		break;
	}

	lcd_memory[lcd_xcurr][lcd_ycurr] = temp; // Передаем байт в видеобуфер
	SendByte(DATA_LCD_MODE,temp); // Передаем байт в контроллер
}

//******************************************************************************
// Вывод линии на LCD-экран
//  x1, x2: 0..95  координата по горизонтали (отсчет от верхнего левого угла)
//	y1, y2: 0..64  координата по вертикали
//	pixel_mode: PIXEL_ON  - для включения пикскела
//				PIXEL_OFF - для выключения пиксела
//				PIXEL_INV - для инверсии пиксела
void PCF8814::Line (uint8_t x1,uint8_t y1, uint8_t x2,uint8_t y2, uint8_t pixel_mode)
{
	if (x1 > LCD_X_RES-1) x1 = LCD_X_RES-1 ;
	if (x2 > LCD_X_RES-1) x2 = LCD_X_RES-1 ;
	if (y1 > LCD_Y_RES-1) y1 = LCD_Y_RES-1 ;
	if (y2 > LCD_Y_RES-1) y2 = LCD_Y_RES-1 ;
	
	int dy, dx;
	signed char addx = 1, addy = 1;
	signed int 	P, diff;

	uint8_t i = 0;

	dx = abs((signed char)(x2 - x1));
	dy = abs((signed char)(y2 - y1));

	if(x1 > x2)	addx = -1;
	if(y1 > y2)	addy = -1;

	if(dx >= dy)
	{
		dy *= 2;
		P = dy - dx;

		diff = P - dx;

		for(; i<=dx; ++i)
		{
			Pixel(x1, y1, pixel_mode);

			if(P < 0)
			{
				P  += dy;
				x1 += addx;
			}
			else
			{
				P  += diff;
				x1 += addx;
				y1 += addy;
			}
		}
	}
	else
	{
		dx *= 2;
		P = dx - dy;
		diff = P - dy;

		for(; i<=dy; ++i)
		{
			Pixel(x1, y1, pixel_mode);

			if(P < 0)
			{
				P  += dx;
				y1 += addy;
			}
			else
			{
				P  += diff;
				x1 += addx;
				y1 += addy;
			}
		}
	}
}



//******************************************************************************
// Вывод окружности на LCD-экран
//  x: 0..95  координаты центра окружности (отсчет от верхнего левого угла)
//	y: 0..64  координата по вертикали
//  radius:   радиус окружности
//  fill:		FILL_OFF  - без заливки окружности
//				FILL_ON	  - с заливкой
//	pixel_mode: PIXEL_ON  - для включения пикскела
//				PIXEL_OFF - для выключения пиксела
//				PIXEL_INV - для инверсии пиксела

void PCF8814::Circle(uint8_t x, uint8_t y, uint8_t radius, uint8_t fill, uint8_t pixel_mode)
{
	signed char  a, b, P;

	a = 0;
	b = radius;
	P = 1 - radius;

	do
	{
		if(fill)
		{
			Line(x-a, y+b, x+a, y+b, pixel_mode);
			Line(x-a, y-b, x+a, y-b, pixel_mode);
			Line(x-b, y+a, x+b, y+a, pixel_mode);
			Line(x-b, y-a, x+b, y-a, pixel_mode);
		}
		else
		{
			Pixel(a+x, b+y, pixel_mode);
			Pixel(b+x, a+y, pixel_mode);
			Pixel(x-a, b+y, pixel_mode);
			Pixel(x-b, a+y, pixel_mode);
			Pixel(b+x, y-a, pixel_mode);
			Pixel(a+x, y-b, pixel_mode);
			Pixel(x-a, y-b, pixel_mode);
			Pixel(x-b, y-a, pixel_mode);
		}

		if(P < 0) P += 3 + 2 * a++;
		else P += 5 + 2 * (a++ - b--);
	}
	while(a <= b);
}



//******************************************************************************
// Вывод прямоугольника на LCD-экран
//  x1, x2: 0..95  координата по горизонтали (отсчет от верхнего левого угла)
//	y1, y2: 0..64  координата по вертикали
//	pixel_mode: PIXEL_ON  - для включения пикскела
//				PIXEL_OFF - для выключения пиксела
//				PIXEL_INV - для инверсии пиксела
void PCF8814::Rect (uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t fill, uint8_t pixel_mode)
{
	if(fill)
	{			// С заливкой
		uint8_t  i, xmin, xmax, ymin, ymax;

		if(x1 < x2) {
			xmin = x1;
			xmax = x2;
		}	// Определяем минимальную и максимальную координату по X
		else {
			xmin = x2;
			xmax = x1;
		}

		if(y1 < y2) {
			ymin = y1;
			ymax = y2;
		}	// Определяем минимальную и максимальную координату по Y
		else {
			ymin = y2;
			ymax = y1;
		}

		for(; xmin <= xmax; ++xmin)
		{
			for(i=ymin; i<=ymax; ++i) Pixel(xmin, i, pixel_mode);
		}
	}
	else		// Без заливки
	{
		Line(x1, y1, x2, y1, pixel_mode);		// Рисуем стороны прямоуголиника
		Line(x1, y2, x2, y2, pixel_mode);
		Line(x1, y1+1, x1, y2-1, pixel_mode);
		Line(x2, y1+1, x2, y2-1, pixel_mode);
	}
}

//******************************************************************************
// Вывод картинки на LCD-экран
//  x: 0..95  координата верхнего левого угла по горизонтали (отсчет от верхнего левого угла экрана)
//	y: 0..64  координата верхнего левого угла по вертикали
//  picture: указатель на массив с монохромной картинкой в программной памяти, первые 2 байта указывают соответственно
//			 размер картинки по горизонтали и вертикали
void PCF8814::Pict  (uint8_t x, uint8_t y, uint8_t * picture)
{
	uint8_t pict_width = pgm_read_byte(&picture[0]);  // ширина спрайта в пикселах
	uint8_t pict_height = pgm_read_byte(&picture[1]); // высота спрайта в пикселах
	uint8_t pict_height_bank=pict_height / 8+((pict_height%8)>0?1:0); // высота спрайта в банках
	uint8_t y_pos_in_bank = y/8 + ((y%8)>0?1:0);		// позиция по y в банках (строках по 8 пикс.)

	int adr = 2; // индекс текущего байта в массиве с картинкой
	uint8_t i;
	for (i=0; i< pict_height_bank; i++)
	{ // проход построчно (по банкам)

		if (i<((LCD_Y_RES/8)+1)) // не выводить картинку за пределами экрана
		{
			//позиционирование на новую строку
			lcd_xcurr=x;
			lcd_ycurr=y_pos_in_bank + i;

			SendByte(CMD_LCD_MODE,(0xB0|((y_pos_in_bank+i)&0x0F))); // установка адреса по Y: 0100 yyyy
			SendByte(CMD_LCD_MODE,(0x00|(x&0x0F)));      // установка адреса по X: 0000 xxxx - биты (x3 x2 x1 x0)
			SendByte(CMD_LCD_MODE,(0x10|((x>>4)&0x07))); // установка адреса по X: 0010 0xxx - биты (x6 x5 x4)

			//вывод строки
			uint8_t j;
			for ( j = 0; j < pict_width; j++ )
			{
				if ((x+j) < LCD_X_RES) SendByte(DATA_LCD_MODE,pgm_read_byte(&picture[adr])); // не выводить картинку за пределами экрана
				adr++;
			}
		}
	}
}
