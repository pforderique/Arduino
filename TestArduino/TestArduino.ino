#include <Blink.h>


/*
 * This is is a test project to make sure that it uploads correctly 
 * to GitHub and to see where it saves my code
 * 
 * UPDATE: NEW LIBRARY BLINK THAT I MADE
 */
 

//Blink led(13);
const int led = 13;

void setup() {
  pinMode(led,OUTPUT);
}

void loop() {
  //led.blink(500);
  analogWrite(led,255);
  delay(500);
  analogWrite(led,0) ;
  delay(500);
}
