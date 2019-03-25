/* This is the basic LCD display setup code.
 * @author Piero Orderique
 * 
 * LCD    Arduino
 * 1        Gnd
 * 2        5V
 * 3      Pot Mid
 * 4        12
 * 5        Gnd
 * 6        11
 * 11       5
 * 12       4
 * 13       3
 * 14       2
 * 15       5V   (w/220 ohm)
 * 16       Gnd
 */

 //Other functions not included as examples:
 //lcd.autoscroll() & lcd.noAutoscroll()
 //lcd.rightToLeft() prints in reverse order
 
#include <LiquidCrystal.h>
//Creates a new object LiquidCrystal(RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//create custom character use the space on the right side of the b's
//for example this makes all 1's visible on the 5x8 rect
byte customChar[8] = {
  0b11100,
  0b10000,
  0b11000,
  0b10000,
  0b10111,
  0b00101,
  0b00111,
  0b00101
};
//https://omerk.github.io/lcdchargen/ for easier character making

void setup() {
  lcd.createChar(0, customChar);
  lcd.begin(16, 2);
  //writes your character
  lcd.write((uint8_t)0);
}

void loop() {

}

void t1(){
  lcd.print(100, BIN);
  delay(500);
  lcd.clear();
  delay(500);
}
//same as t1, but doesnt erase memory
void T1(){
  lcd.display();
  delay(500);
  lcd.noDisplay();
  delay(500);
}

void t2(){
  //places the cursor in the upper left hand corner of the screen
  lcd.home();
  
  lcd.print("XXX");
}

//Simulates the blinking cursor
void t3(){
  //sets cursor pos (0-15 or 0-1) 
  //lcd.setCursor(column, row)
  lcd.setCursor(2, 1);
  
  lcd.cursor();
  delay(500);
  lcd.noCursor();
  delay(500);
}
//scorlls horizontally through the text
void t4(){
  delay(500);
  lcd.scrollDisplayLeft();
}
