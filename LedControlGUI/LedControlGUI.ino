/*
 * Attempting to connect read from Processing GUI
 * @author Piero Orderique
 * 
 * 
 */
const int redLED = 10;
const int yellowLED = 6;
const int greenLED = 8;

void setup() {
  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  Serial.begin(9600);
}

void loop() {

  if(Serial.available()){
    char val = Serial.read(); //reads the character that came in
    if(val == 'r'){ //if r received 
     digitalWrite(redLED, HIGH); 
    }
    if(val == 'y'){ //if y received 
     digitalWrite(yellowLED, HIGH); 
    } 
    if(val == 'g'){ //if g received 
     digitalWrite(greenLED, HIGH); 
    } 
    if(val == 'f'){ //if f received 
      digitalWrite(redLED,LOW);  
      digitalWrite(yellowLED,LOW);  
      digitalWrite(greenLED,LOW);  
    } 
  }
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
