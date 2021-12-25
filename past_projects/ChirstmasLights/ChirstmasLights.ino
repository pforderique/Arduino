#include <IRremote.h>

const int RECV_PIN = 3;
IRrecv irrecv(RECV_PIN);
decode_results results;
//unsigned long key_value = 0;

int state = 0;
const int relayPin = 2;

int res;

void setup(){
  Serial.begin(9600);
  irrecv.enableIRIn();
  irrecv.blink13(true);
  pinMode(relayPin,OUTPUT);
}

void loop(){ 
  if (irrecv.decode(&results)){
    switch(results.value){
      case 484657335:
        Serial.println("POWER_BUTTON");
        updateState();
        break;
      case 484671615:
        Serial.println("One button");
        blinkLed(10,500);
        break;
      case 484655295:
        Serial.println("Two button");
        blinkLed(10,250);
        break;
      case 484687935:
        Serial.println("Three button");
        blinkLed(10,100);
        break;
     case 484647135:
        Serial.println("Four button");
        blinkLed(15,500);
        break;
      case 484679775:
        Serial.println("Five button");
        blinkLed(15,250);
        break;
      case 484663455:
        Serial.println("Six button");
        blinkLed(15,100);
        break;
     case 484696095:
        Serial.println("Seven button");
        blinkLed(20,500);
        break;
      case 484643055:
        Serial.println("Eight button");
        blinkLed(20,250);
        break;
      case 484675695:
        Serial.println("Nine button");
        blinkLed(20,100);
        break;                
    }
    irrecv.resume();
  }
}

void blinkLed(int REP, int DELAY){
  for(int i = 0; i < REP; i++){
    digitalWrite(relayPin,HIGH);
    delay(DELAY);
    digitalWrite(relayPin,LOW);
    delay(DELAY);    
  }
}

void updateState(){
  state++;
  if(state%2 == 0){
    digitalWrite(relayPin, LOW);
    Serial.println("LIGHTS ON");
  }else if(state%2 == 1){
    digitalWrite(relayPin, HIGH);
    Serial.println("LIGHTS OFF");
  }
}
