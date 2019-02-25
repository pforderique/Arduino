/*
 * This is is a test project to make sure that it uploads correctly 
 * to GitHub and to see where it saves my code
 */
const int led = 13;
void setup() {
  pinMode(led,OUTPUT);
}

void loop() {
  blinkLed(led,500);
}

void blinkLed(int ledPin, int Delay){
  digitalWrite(ledPin,HIGH);
  delay(Delay);
  digitalWrite(ledPin,LOW);
  delay(Delay);
}
