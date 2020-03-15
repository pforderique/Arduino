/*
 * Servo and LED control using COM5 and Processing GUI
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
Servo myServo;

void setup() {
  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  myServo.attach(3);
  Serial.begin(9600);
}

void loop() { 
  for(int i = 1; i <=180; i++){myServo.write(i);delay(10);}
  for(int i = 179; i >=0; i--){myServo.write(i);delay(10);}
  if(Serial.available()){
    char val = Serial.read(); //reads the character that came in
    if(val == 'r'){ //if r received 
     redState = !redState;
    }
    if(val == 'y'){ //if y received 
     yellowState = !yellowState; 
    } 
    if(val == 'g'){ //if g received 
      greenState = !greenState; 
    } 
    if(val == 'f'){ //if f received 
      redState = !redState;
      yellowState = !yellowState;
      greenState = !greenState;
    } 
  }
  digitalWrite(redLED,redState);  
  digitalWrite(yellowLED,yellowState);  
  digitalWrite(greenLED,greenState); 
}

void blinkAll(){
  digitalWrite(redLED,HIGH);  
  digitalWrite(yellowLED,HIGH);  
  digitalWrite(greenLED,HIGH);
  delay(500);
  digitalWrite(redLED,LOW);  
  digitalWrite(yellowLED,LOW);  
  digitalWrite(greenLED,LOW);  
  delay(500);
}
