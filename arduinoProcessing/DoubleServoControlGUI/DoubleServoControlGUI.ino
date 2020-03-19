/*
 * Double servo control using Processing GUI
 * @author Piero Orderique
 * 
 */
#include <Servo.h>;
const int redLED = 10;
const int yellowLED = 6;
const int greenLED = 8;
boolean redState = LOW;
boolean yellowState = LOW;
boolean greenState = LOW;
Servo panServo;
Servo tiltServo;
int panPin = 3;
int tiltPin = 6;

void setup() {
  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  panServo.attach(panPin);
  tiltServo.attach(tiltPin);
  Serial.begin(9600);
}

void loop() {
  if(Serial.available()>0){
    int val = Serial.parseInt();
    //wanna add more functionality? use other mappings and set other numbers to do other things!
    if(val>=0 && val<=180){
      panServo.write(val);
      delay(20);
    }
    if(val>=200 && val<=300){
      int pos = map(val,200,300,0,180);
      tiltServo.write(pos);
      delay(20);
    }
  }
}
//-----------------------------------------------------------------
