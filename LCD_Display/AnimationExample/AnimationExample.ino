#include <LiquidCrystal.h>

// initialize the library
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

byte heart[8] = { // 1
0b00000,
0b00000,
0b01010,
0b11111,
0b11111,
0b01110,
0b00100,
0b00000
};

byte smiley[8] = { // 2
0b00000,
0b00000,
0b01010,
0b00000,
0b00000,
0b10001,
0b01110,
0b00000
};

byte frownie[8] = { // 3
0b00000,
0b00000,
0b01010,
0b00000,
0b00000,
0b00000,
0b01110,
0b10001
};

byte armsDown[8] = { // 4
0b00100,
0b01010,
0b00100,
0b00100,
0b01110,
0b10101,
0b00100,
0b01010
};

byte armsUp[8] = { // 5
0b00100,
0b01010,
0b00100,
0b10101,
0b01110,
0b00100,
0b00100,
0b01010
};

void setup()
{
lcd.createChar(1, heart);
lcd.createChar(2, smiley);
lcd.createChar(3, frownie);
lcd.createChar(4, armsDown);
lcd.createChar(5, armsUp);
lcd.begin(16, 2);
lcd.setCursor(3, 0);
lcd.write(1);
lcd.print("Arduino");
lcd.write(2);
lcd.setCursor(4, 1);
lcd.print("Animation");
delay(5000);
}

void loop()
{
delay(500);
lcd.clear();
lcd.setCursor(4, 1);
lcd.write(5);
lcd.setCursor(13, 0);
lcd.write(3);
delay(500);
lcd.clear();
lcd.setCursor(4, 1);
lcd.write(4);
lcd.setCursor(13, 0);
lcd.write(2);
}
