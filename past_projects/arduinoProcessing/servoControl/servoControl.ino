/*
 * Double servo control using Processing GUI
 * @author Piero Orderique
 * 
 */
#include <Servo.h>;
Servo panServo;
int panPin = 3;

void setup() {
  panServo.attach(panPin);
  Serial.begin(9600);
}

void loop() {
  if(Serial.available()){
    int val = Serial.parseInt(); 
    Serial.println(val);
    panServo.write(val);
  }
}
//-----------------------------------------------------------------
