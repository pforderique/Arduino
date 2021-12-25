/*
 * USE THIS CODE TO FIGURE OUT WHAT VALUES THE CONTROLLER
 * GIVES!!! 
 * 
 * only thing you need is to wire receiver up to pin 7
 */


#include <IRremote.h>

const int RECV_PIN = 7;
IRrecv irrecv(RECV_PIN);
decode_results results;

void setup(){
  Serial.begin(9600);
  irrecv.enableIRIn();
  irrecv.blink13(true);
}

void loop(){
  if (irrecv.decode(&results)){
    Serial.println(results.value, DEC);
    irrecv.resume();
  }
}
