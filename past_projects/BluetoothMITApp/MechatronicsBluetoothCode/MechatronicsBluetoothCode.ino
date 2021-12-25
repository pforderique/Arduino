//This is the code from mechatronics for the APP Inventor. 
//USE THIS CODE!!!

/* Arduino and HC-05 Bluetooth Module Tutorial
 * 
 * by Dejan Nedelkovski, www.HowToMechatronics.com
 * 
 */
 
#define ledPin 6
int state = 0;
void setup() {
 pinMode(ledPin, OUTPUT);
 digitalWrite(ledPin, LOW);
 Serial.begin(38400); // Default communication rate of the Bluetooth module
}
void loop() {
 if(Serial.available() > 0){ // Checks whether data is comming from the serial port
 state = (int) Serial.read(); // Reads the data from the serial port
 }
 if (state == 0) {
 digitalWrite(ledPin, LOW); // Turn LED OFF
 Serial.println("LED: OFF"); // Send back, to the phone, the String "LED: ON"
 state = 9;
 }
 else if (state == 120) {
 digitalWrite(ledPin, HIGH);
 Serial.println("LED: ON");;
 state = 9;
 }
 Serial.print("State: ");
 Serial.println(state);
}
