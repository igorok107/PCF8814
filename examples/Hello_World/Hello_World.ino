#include <PCF8814.h> 

PCF8814 Lcd(13,11,10,6); // LCD sets SPI SCLK: 7 pin, SDA: 8 pin, CS: 9 pin. RESET: 6 pin

void setup() { 
  Lcd.Init();
  Lcd.GotoXY(6,2);
  Lcd.Print("Hello");
  Lcd.GotoXY(4,4);
  Lcd.PrintWide("WORLD");
  Lcd.Rect(0,0,95,67,FILL_OFF,PIXEL_ON); 
  Lcd.Circle(48,34,32,FILL_OFF,PIXEL_ON);
}

void loop() {
}

