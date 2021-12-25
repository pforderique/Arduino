//Hand Repulsor Code
//started 12/20/18 at 11:05pm

/*
 * Includes Servo to open the arm
 * 
 * TODO:
 * Build hand with LEDs
 * install a laser module
 * get it to work with nano
 * Bluetooth control using app
 * 
 */

//Servo that opens the arm
#include<Servo.h>
Servo Arm;  //creates the object
const int armPin = 9; //sets the pin number
boolean isClosed = false; //initializes state
const int t = 20; //delay for opening and closing
 
void setup() {
  Serial.begin(9600); //begins communication
  
//Arm Servo
  Arm.attach(open1Pin); //hooks up the servo
  int currentPos = Arm.read();  //gets the current angle
  if( currentPos < 22){ //if it's closed, change to true
    isClosed = true;
  }else{ //it it's not closed, then close it, starting at the
    closeArm(currentPos,20);  //current position
  }
}

void loop() {
  openArm(20,115);
  delay(5000);
  closeArm(115,20);
  delay(5000);
}

void openArm(int startPos, int endPos){
    for(int i = startPos; i<=endPos; i++){
     delay(t);
     open1.write(i);
     Serial.println(Arm.read());
  }
}

void closeArm(int startPos, int endPos){
    for(int i = startPos; i>=endPos; i--){
     delay(t);
     open1.write(i);
     Serial.println(Arm.read());
  }  
}
