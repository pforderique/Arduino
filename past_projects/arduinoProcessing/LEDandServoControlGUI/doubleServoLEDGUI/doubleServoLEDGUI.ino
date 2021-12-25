/*
 * Double servo control using Processing GUI
 * @author Piero Orderique
 * 
 */
#include <Servo.h>;
const int redLED = 11;
const int yellowLED = 9;
const int greenLED = 10;
boolean redState = LOW;
boolean yellowState = LOW;
boolean greenState = LOW;
Servo panServo;
Servo tiltServo;
const int panPin = 3;
const int tiltPin = 6;


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
    //LED control
    if(val == 2){redState = LOW;digitalWrite(redLED, redState);}
    if(val == 3){redState = HIGH;digitalWrite(redLED, redState);}
    if(val == 4){yellowState = LOW;digitalWrite(yellowLED, yellowState);}
    if(val == 5){yellowState = HIGH;digitalWrite(yellowLED, yellowState);}
    if(val == 6){greenState = LOW;digitalWrite(greenLED, greenState);}
    if(val == 7){greenState = HIGH;digitalWrite(greenLED, greenState);}
    //pan servo control
    if(val>=2001 && val<=3000){
      int pos = map(val,2001,3000,0,180);
      panServo.write(pos);
      delay(20);
    }
    //tilt servo control
    if(val>=3001 && val<=4000){
      int pos = map(val,3001,4000,0,180);
      tiltServo.write(pos);
      delay(20);
    }
  }
}
//-----------------------------------------------------------------
