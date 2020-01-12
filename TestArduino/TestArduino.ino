//#include <Blink.h>


/*
 * This is is a test project to make sure that it uploads correctly 
 * to GitHub and to see where it saves my code
 * 
 * UPDATE: NEW LIBRARY BLINK THAT I MADE
 */
 

//Blink led(13);
const int led = 13;

void setup() {
  pinMode(13,OUTPUT);
}

void loop() {
  //led.blink(500);
  digitalWrite(13,HIGH);
  delay(200);
  digitalWrite(13,LOW) ;
  delay(200);
}
