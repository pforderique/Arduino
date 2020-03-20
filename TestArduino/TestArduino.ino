//#include <Blink.h>


/*
 * This is is a test project to make sure that it uploads correctly 
 * to GitHub and to see where it saves my code
 * 
 * UPDATE: NEW LIBRARY BLINK THAT I MADE
 */
 

//Blink led(13);
#include <Servo.h>;
Servo myServo;
const int led = 10;
const int servoPin = 6;

void setup() {
  pinMode(led,OUTPUT);
  myServo.attach(servoPin);
}

void loop() {
  //testLED(led);
  sweepServo();
}
//------------------------ TEST METHODS ------------------------------
void testLED(int pin){
  digitalWrite(pin,HIGH);
  delay(200);
  digitalWrite(pin,LOW) ;
  delay(200);
}
void sweepServo(){
  for(int i=0; i<180; i++){
    myServo.write(i);
    delay(10);
  }
  for(int i=180; i>0; i--){
    myServo.write(i);
    delay(10);    
  }
}
