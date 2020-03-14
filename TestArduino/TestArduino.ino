//#include <Blink.h>


/*
 * This is is a test project to make sure that it uploads correctly 
 * to GitHub and to see where it saves my code
 * 
 * UPDATE: NEW LIBRARY BLINK THAT I MADE
 */
 

//Blink led(13);
const int led = 10;

void setup() {
  pinMode(led,OUTPUT);
}

void loop() {
  //led.blink(500);
  digitalWrite(led,HIGH);
  delay(200);
  digitalWrite(led,LOW) ;
  delay(200);
}
