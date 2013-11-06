#include <PCF8814.h>

PCF8814 Lcd(7,8,9,6); // LCD set SCLK: 7 pin, SDA: 8 pin, CS: 10 pin, RESET: 6 pin

char Text[] PROGMEM = "GRAPH DEMO INIT";

void setup() { 
  Lcd.Init();
  delay(100);
}

void loop() {
  Lcd.GotoXY(1,4);
  Lcd.PrintF(Text); // Выводим строку из программной памяти

  delay(4000);

  Lcd.GotoXY(0,0);

  Lcd.PrintF(PSTR(" GRAPH MODE DEMO")); // Другой способ задания строк в программной памяти
  Lcd.PrintF(PSTR("----------------"));
  Lcd.PrintF(PSTR(" DigitalChip.ru "));
  Lcd.PrintF(PSTR("    present     "));
  Lcd.PrintF(PSTR(" NOKIA 1100 LCD "));
  Lcd.PrintF(PSTR("  demonstration "));
  Lcd.PrintF(PSTR("----------------"));

  delay(8000);
  //
  Lcd.GotoXY(2,7);
  Lcd.PrintF(PSTR("Clear screen"));
  delay(4000);
  Lcd.Clear();
  //
  Lcd.GotoXY(1,7);
  Lcd.PrintF(PSTR("Pixel function"));

  byte i,j;
  for (i = 0; i<96; i++)
  {
    for (j = 0; j<57; j++)
    {
      Lcd.Pixel(i,j,PIXEL_ON);
      j++;
    }
    i++;
  }
  delay(4000);
  //
  Lcd.Clear();
  Lcd.GotoXY(0,7);
  Lcd.PrintF(PSTR("Line function   "));

  Lcd.Line (3,4,90,41, PIXEL_ON);
  Lcd.Line (95,0,30,55, PIXEL_ON);
  Lcd.Line (20,20,70,60, PIXEL_ON);

  delay(4000);

  //
  Lcd.Clear();
  Lcd.GotoXY(0,7);
  Lcd.PrintF(PSTR("Circle function "));

  Lcd.Circle(50,30,20, FILL_OFF, PIXEL_ON);
  delay(4000);

  //
  Lcd.GotoXY(0,7);
  Lcd.PrintF(PSTR("Fill circle     "));

  Lcd.Circle(40,25,15, FILL_ON, PIXEL_ON);
  delay(4000);
  //
  Lcd.GotoXY(0,7);
  Lcd.PrintF(PSTR("Fill rectangle  "));

  Lcd.Rect (10,10,55,38,FILL_ON, PIXEL_ON);
  delay(4000);

  //
  Lcd.GotoXY(0,7);
  Lcd.PrintF(PSTR("Rectangle invers"));

  Lcd.Rect (20,20,45,34,FILL_OFF, PIXEL_INV);
  delay(4000);

  //
  Lcd.GotoXY(0,7);
  Lcd.PrintF(PSTR("Pixel inversion "));

  for (i = 0; i<96; i++)
  {
    for (j = 0; j<57; j++)
    {
      Lcd.Pixel(i,j,PIXEL_INV);
      j++;
    }
    i++; 
    i++;
  }
  delay(4000);
  //

  Lcd.Clear();

  Lcd.PrintF(PSTR(" GRAPH MODE DEMO")); 
  Lcd.PrintF(PSTR("----------------"));
  Lcd.PrintF(PSTR("   End of demo  "));
  Lcd.PrintF(PSTR("                "));
  Lcd.PrintF(PSTR("   see more on  "));
  Lcd.PrintF(PSTR(" digitalchip.ru "));
  Lcd.PrintF(PSTR("----------------"));

  delay(8000);

  Lcd.Clear();
}



