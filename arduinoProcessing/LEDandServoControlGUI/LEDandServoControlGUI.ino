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
  randomSeed(A0); //To generate new randoms everytime arduino is reset
}

void loop() {
  static int v = 0;
  if(Serial.available()){
    char val = Serial.read(); //reads the character that came in
    
    switch(val){
      case '0'...'9':
        v = v*10 +val -'0';
        break;
      case 'a':
        myServo.write(v);
        Serial.println(v);
        v=0;
        break;
      case 'b':
        updateLights(v);
    }
    if(val == 666){goCrazy(20);} 
  }
  digitalWrite(redLED,redState);  
  digitalWrite(yellowLED,yellowState);  
  digitalWrite(greenLED,greenState);
  delay(10); 
}
//-----------------------------------------------------------------
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
void updateLights(int val){
  if(val==500){redState = LOW;}
  if(val==501){redState = HIGH;}
  if(val==502){yellowState = LOW;}
  if(val==503){yellowState = HIGH;}
  if(val==504){greenState = LOW;}
  if(val==505){greenState = HIGH;}
}
void goCrazy(int cycles){
  for(int i = 0; i < cycles; i++){
    int rand1 = random(10,500);
    int rand2 = random(10,400);
    digitalWrite(redLED,HIGH);  
    digitalWrite(yellowLED,HIGH);  
    digitalWrite(greenLED,HIGH);
    delay(rand1);
    digitalWrite(redLED,LOW);  
    digitalWrite(yellowLED,LOW);  
    digitalWrite(greenLED,LOW);  
    delay(rand2);   
  }
}
