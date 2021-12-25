/*
  MotionCollector Project
  Piero Orderique 
  Started 7/22/21

  Collect motion sensor data from porch and sends data to 
  the server to be stored in the database. Includes a page
  to see motion detections over time.
*/

#include <WiFi.h> //Connect to WiFi Network
#include <string.h>

/* PINS */
const uint8_t LIGHT_PIN = 27; // Light pin (LED or lamp)
const uint8_t MOTION_SENSOR_PIN = 12;

/* WiFi setup */
const char USER[] = "porderiq"; //CHANGE YOUR USER VARIABLE!!!
const char POST_URL[] = "POST http://127.0.0.1:5000/database HTTP/1.1\r\n";
char network[] = "PCP-0672";
char password[] = "Limaperuu1";
uint8_t channel = 1; //network channel on 2.4 GHz
byte bssid[] = {0x04, 0x95, 0xE6, 0xAE, 0xDB, 0x41}; //6 byte MAC address of AP you're targeting.

char host[] = "127.0.0.1:5000";

//Some constants and some resources:
const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
char response[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP request

/* states */
bool prev_detected;
bool detected;

void setup() {
  Serial.begin(115200); //for debugging if needed.
  delay(200);
  WiFi.disconnect();
  Serial.println("disconnected");
//  WiFi.begin("lolllool","loooooooool"); //Can be a nonexistent network
//  int n = WiFi.scanNetworks();
//  Serial.println("scan done");
//  if (n == 0) {
//    Serial.println("no networks found");
//  } else {
//    Serial.print(n);
//    Serial.println(" networks found");
//    for (int i = 0; i < n; ++i) {
//      Serial.printf("%d: %s, Ch:%d (%ddBm) %s ", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "open" : "");
//      uint8_t* cc = WiFi.BSSID(i);
//      for (int k = 0; k < 6; k++) {
//        Serial.print(*cc, HEX);
//        if (k != 5) Serial.print(":");
//        cc++;
//      }
//      Serial.println("");
//    }
//  }
//  delay(200); //wait a bit (100 ms)
//
//  //if using regular connection use line below:
//  WiFi.begin(network, password);
//  //if using channel/mac specification for crowded bands use the following:
//  //WiFi.begin(network, password, channel, bssid);
//
//
//  uint8_t count = 0; //count used for Wifi check times
//  Serial.print("Attempting to connect to ");
//  Serial.println(network);
//  while (WiFi.status() != WL_CONNECTED && count < 6) { //can change this to more attempts
//    delay(500);
//    Serial.print(".");
//    count++;
//  }
//  delay(2000);  //acceptable since it is in the setup function.
//  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
//    Serial.println("CONNECTED!");
//    Serial.printf("%d:%d:%d:%d (%s) (%s)\n", WiFi.localIP()[3], WiFi.localIP()[2],
//                  WiFi.localIP()[1], WiFi.localIP()[0],
//                  WiFi.macAddress().c_str() , WiFi.SSID().c_str());
//    delay(500);
//  } else { //if we failed to connect just Try again.
//    Serial.println("Failed to Connect :/  Going to restart");
//    Serial.println(WiFi.status());
//    ESP.restart(); // restart the ESP (proper way)
//  }

  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(MOTION_SENSOR_PIN, INPUT);
}

void loop() {
  detected = digitalRead(MOTION_SENSOR_PIN);
  motion_fsm(detected);
  prev_detected = detected;
}

void motion_fsm(bool detection) {
  switch (detection)
  {
  case 0:
    // no motion, do nothing
    break;
  
  case 1:
    // saw motion! send to database only if there was 
    // no motion previously
    if(prev_detected == 0) {
      ping_database();
    }
    break;
  }
}

void ping_database(){
  Serial.println("Motion Detected! -- ");
  // build up your POST request
//  char body[100]; //for body
//  sprintf(body, "user=%s", USER); //generate body, posting user
//  int body_len = strlen(body); //calculate body length (for header reporting)
//  sprintf(request_buffer, POST_URL);
//  strcat(request_buffer, "Host: 127.0.0.1:5000\r\n");
//  strcat(request_buffer, "Content-Type: application/x-www-form-urlencoded\r\n");
//  sprintf(request_buffer + strlen(request_buffer), "Content-Length: %d\r\n", body_len); //append string formatted to end of request buffer
//  strcat(request_buffer, "\r\n"); //new line from header to body
//  strcat(request_buffer, body); //body
//  strcat(request_buffer, "\r\n"); //new line
//  Serial.println(request_buffer);
//  do_http_request("127.0.0.1:5000", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
//  Serial.println(response_buffer); //viewable in Serial Terminal
}
