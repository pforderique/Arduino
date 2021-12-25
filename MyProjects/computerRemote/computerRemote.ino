#include <IRremote.h>
#include "mappings.h"

const int LED_PIN = 13;
const int RECV_PIN = 3;
IRrecv irrecv(RECV_PIN);
decode_results results;

unsigned long start;
bool led_state = false;

void setup(){
  Serial.begin(115200);
  irrecv.enableIRIn();
  irrecv.blink13(true);
  
  pinMode(LED_PIN, OUTPUT);
  start = millis();
}

void loop(){ 
  runRemote();
}

void runRemote() {
  if (irrecv.decode(&results)){
    Serial.println(results.value);
    irrecv.resume();
  }
} 

void blinkLed(int REP, int DELAY){
  for(int i = 0; i < REP; i++){
    digitalWrite(LED_PIN,HIGH);
    delay(DELAY);
    digitalWrite(LED_PIN,LOW);
    delay(DELAY);    
  }
}
