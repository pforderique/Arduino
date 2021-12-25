const int LED = 9;
void setup() {
  Serial.begin(9600);
  pinMode(LED,OUTPUT);
}

void loop() {
  int flexVal = analogRead(A1);
  Serial.println(flexVal );
  if(flexVal < 5){
    digitalWrite(LED,LOW);
  }else if(flexVal > 8){
    digitalWrite(LED,HIGH);
  }
  delay(10);
}
