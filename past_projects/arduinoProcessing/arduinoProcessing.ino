 char val; // Data received from the serial port READS AS A CHAR or STRING?
 int ledPin = 13; // Set the pin to digital I/O 13
 boolean ledState = LOW;  //to toggle our LED

void setup() {
  //initialize serial communications at a 9600 baud rate
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  //establishContact();  // send a byte to establish contact until receiver responds 
}
void loop(){
  Serial.println("Hello there");
  delay(200);
  /*
  // If data is available to read:
  if (Serial.available() > 0) { 
    val = Serial.read(); // read it and store it in val
    if(val == '1'){ //if we get a 1
       ledState = !ledState; //flip the ledState
       digitalWrite(ledPin, ledState); 
    }
    delay(100);
  }else{  
    //send back a hello world when nothing is being sent
    Serial.println("Hello, world!"); 
    delay(50);
    }
    */
}
void establishContact(){
  //while nothing is being sent:
  while (Serial.available() <= 0) {
    Serial.println("A");   // send a capital A -- lets us "start" the program at a specific time
    delay(300);
  }
}
