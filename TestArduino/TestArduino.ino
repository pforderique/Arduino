#include <Blink.h>


/*
 * This is is a test project to make sure that it uploads correctly 
 * to GitHub and to see where it saves my code
 * 
 * UPDATE: NEW LIBRARY BLINK THAT I MADE
 */
 

Blink led(13);

void setup() {
  pinMode(5,OUTPUT);
}

void loop() {
  //led.blink(500);
  analogWrite(5,255);
  delay(50);
  analogWrite(5,0) ;
  delay(50);
}
