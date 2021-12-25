const int buzzerPin = 7;
const int photoPin = A5;
void setup()
{
    Serial.begin(9600);  //Begin serial communcation
    pinMode(buzzerPin,OUTPUT);
    pinMode(photoPin,INPUT);
}

void loop()
{
  int val = analogRead(photoPin);
    Serial.println(val); //Write the value of the photoresistor to the serial monitor.
     delay(10); //short delay for faster response to light.
     
  if(val<275){
      digitalWrite(buzzerPin,HIGH);
      delay(5000);
      digitalWrite(buzzerPin,LOW);
  }
}
