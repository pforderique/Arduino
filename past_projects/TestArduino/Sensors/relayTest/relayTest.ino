/*
 * Relay works exactly like an led 
 * 
 * @author Piero Orderique
 */

const int blueLed = 12;
const int redLed = 11;
const int yellowLed = 10;
const int greenLed = 9;
const int relay = 2;

const int X_pin = 0; // analog pin connected to X output
const int Y_pin = 1; // analog pin connected to Y output

 
void setup() {
  pinMode(relay, OUTPUT);
  pinMode(blueLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(greenLed, OUTPUT);

  Serial.begin(115200);
}

void loop() {
  Serial.print("X-axis: ");
  Serial.print(analogRead(X_pin));
  Serial.print("\n");
  Serial.print("Y-axis: ");
  Serial.println(analogRead(Y_pin));
  Serial.print("\n\n");
  //runBoth();
  runRelay();
  //runLights();
}

void runBoth(){
  if(analogRead(Y_pin) > 750){
    digitalWrite(blueLed, LOW);
    digitalWrite(yellowLed,HIGH);
    delay(50);
    digitalWrite(yellowLed,LOW) ;
    delay(50);
  }else if(analogRead(Y_pin) < 250){
    digitalWrite(blueLed, LOW);
    digitalWrite(relay,HIGH);
    delay(5);
    digitalWrite(relay,LOW) ;
    delay(5);     
  }else if(analogRead(X_pin) < 250){
    digitalWrite(blueLed, LOW);
    digitalWrite(redLed,HIGH);
    delay(50);
    digitalWrite(redLed,LOW) ;
    delay(50);  
   }else if(analogRead(X_pin) > 750){
    digitalWrite(blueLed, LOW);
    digitalWrite(greenLed,HIGH);
    delay(50);
    digitalWrite(greenLed,LOW) ;
    delay(50); 
   }else{
    digitalWrite(blueLed, HIGH);
    delay(100);
  }
}

void runRelay(){
  if(analogRead(Y_pin) > 750){
    digitalWrite(blueLed, LOW);
    digitalWrite(relay,HIGH);
    delay(50);
    digitalWrite(relay,LOW) ;
    delay(50);
  }else if(analogRead(Y_pin) < 250){
    digitalWrite(blueLed, LOW);
    digitalWrite(relay,HIGH);
    delay(5);
    digitalWrite(relay,LOW) ;
    delay(5);     
  }else if(analogRead(X_pin) < 250){
    digitalWrite(blueLed, LOW);
    digitalWrite(relay,HIGH);
    delay(100);
    digitalWrite(relay,LOW) ;
    delay(100);  
   }else if(analogRead(X_pin) > 750){
    digitalWrite(blueLed, LOW);
    digitalWrite(relay,HIGH);
    delay(15);
    digitalWrite(relay,LOW) ;
    delay(15); 
   }else{
    digitalWrite(blueLed, HIGH);
    delay(100);
   }  
}

void runLights(){
  if(analogRead(Y_pin) > 750){
    digitalWrite(yellowLed,HIGH);
    delay(5);
    digitalWrite(yellowLed,LOW) ;
    delay(5);
  }else if(analogRead(Y_pin) < 250){
    digitalWrite(blueLed,HIGH);
    delay(5);
    digitalWrite(blueLed,LOW) ;
    delay(5);     
  }else if(analogRead(X_pin) < 250){
    digitalWrite(redLed,HIGH);
    delay(5);
    digitalWrite(redLed,LOW) ;
    delay(5);  
   }else if(analogRead(X_pin) > 750){
    digitalWrite(greenLed,HIGH);
    delay(5);
    digitalWrite(greenLed,LOW) ;
    delay(5); 
   }
   delay(50);
}
