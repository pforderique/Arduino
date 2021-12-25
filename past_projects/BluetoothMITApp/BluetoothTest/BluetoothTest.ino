//Use this one for the Video Tutorial and the app that was created
#include<Servo.h>
Servo s;
char data = 0;                //Variable for storing received data
void setup(){
  Serial.begin(9600);         //Sets the data rate in bits per second (baud) for serial data transmission
  pinMode(13, OUTPUT);        //Sets digital pin 13 as output pin
  s.attach(7);
}

void loop()
{
  if (Serial.available() > 0) // Send data only when you receive data:
  {
    data = Serial.read();      //Read the incoming data and store it into variable data
    Serial.print(data);        //Print Value inside data in Serial monitor
    Serial.print("\n");        //New line
  }

  if (data == '1') {          //Checks whether value of data is equal to 1 ---- corresponds to byte #49
    digitalWrite(13, HIGH);  //If value is 1 then LED turns ON
    s.write(180);
  } else if (data == '0') {     //Checks whether value of data is equal to 0
    digitalWrite(13, LOW);   //If value is 0 then LED turns OFF
    s.write(0);
  }

}
