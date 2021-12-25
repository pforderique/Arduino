#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

byte at1[8] = {
  0b00111,
  0b01000,
  0b10011,
  0b10100,
  0b10100,
  0b10011,
  0b01000,
  0b00111
};

byte at2[8] = {
  0b10000,
  0b01000,
  0b00100,
  0b10100,
  0b10100,
  0b01000,
  0b00000,
  0b00000
};

void setup(){
    lcd.createChar(0, at1);
    lcd.createChar(1, at2);
    lcd.begin(16, 2);
}

void loop(){
  lcd.home();
  lcd.print("follow");
  lcd.setCursor(0,1);
  lcd.write((uint8_t)0);
  lcd.write((uint8_t)1);
  lcd.print("arduino.maker");
}
