const int LED = 3;
void setup() {
  Serial.begin(9600);
  pinMode(LED,OUTPUT);
}

void loop() {
  int flexVal = analogRead(A1);
  Serial.println(flexVal );
  if(flexVal < 142){
    digitalWrite(LED,HIGH);
  }else{
    digitalWrite(LED,LOW);
  }
  delay(500);
}
