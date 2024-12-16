#include <LiquidCrystal_I2C.h>
//#include <Wire.h>

LiquidCrystal_I2C lcd(0x3F,16,2);

void setup() {
  // put your setup code here, to run once:
lcd.init();
lcd.backlight();
lcd.print("hello world");
lcd.setCursor(2, 1);
lcd.print(char(223));
}

void loop() {
  // put your main code here, to run repeatedly:
lcd.setCursor(0,1);
lcd.print(millis()/1000);
}
