#include <WiFi.h> //Connect to WiFi Network
#include <string.h>  //used for some string handling and processing.

/*#########################################
  This is a  simple Test of the ESP32.
  Don't worry too much about the code...we'll deal with that in class
  INSTRUCTIONS:

  Below change the network and password variables to a 2.4 GHz Wifi network and password.
  If the network of interest is open, leave the password blank.  For example:

  Network is  MY_WIFI and the password is SUPER_SECRET then you'd do:

  char network[] = "MY_WIFI";
  char password[] = "SUPER_SECRET";

  If the network is inSecureSite and there is no password, you'd do:

  char network[] = "inSecureSite";
  char password[] = "";

  If you are on campus, avoid using MIT_SECURE

  Once you've changed that,  you should be ready to upload.
  Save the code, make sure the ESP32 board is selected under Tools>Ports,
  And press the upload button!

  Once it uploads, open the Serial monitor (under Tools) and watch to see what gets printed.


*/

char network[] = "ipadMITfab";
char password[] = "Swimming1234";


const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
const int GETTING_PERIOD = 5000; //periodicity of getting a number fact.
const uint16_t IN_BUFFER_SIZE = 1000; //size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
char request_buffer[IN_BUFFER_SIZE]; //char array buffer to hold HTTP request
char response_buffer[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP response

uint32_t loop_controller; //used for timing
uint32_t last_time; //used for timing


void setup() {
  Serial.begin(115200); //begin serial
  delay(100); //wait a bit (100 ms)
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      Serial.printf("%d: %s, Ch:%d (%ddBm) %s ", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "open" : "");
      Serial.println("");
    }
  }
  Serial.println("");

  WiFi.begin(network, password); //attempt to connect to wifi
  uint8_t count = 0; //count used for Wifi check times
  Serial.print("Attempting to connect to ");
  Serial.println(network);
  while (WiFi.status() != WL_CONNECTED && count < 6) { //can change this to more attempts
    delay(500);
    Serial.print(".");
    count++;
  }
  delay(2000);  //acceptable since it is in the setup function.
  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
    Serial.println("CONNECTED!");
    Serial.printf("%d:%d:%d:%d (%s) (%s)\n", WiFi.localIP()[3], WiFi.localIP()[2],
                  WiFi.localIP()[1], WiFi.localIP()[0],
                  WiFi.macAddress().c_str() , WiFi.SSID().c_str());
    delay(500);
  } else { //if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
  }
  randomSeed(analogRead(A0)); //"seed" random number generator
}


void loop() {
  if ((millis() - last_time) > GETTING_PERIOD) { // GETTING_PERIOD since last lookup? Look up again
    //formulate GET request...first line:
    sprintf(request_buffer, "GET http://numbersapi.com/%d/trivia HTTP/1.1\r\n", random(200));
    strcat(request_buffer, "Host: numbersapi.com\r\n"); //add more to the end
    strcat(request_buffer, "\r\n"); //add blank line!
    //submit to function that performs GET.  It will return output using response_buffer char array
    do_http_GET("numbersapi.com", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
    //Serial.println(response_buffer); //print to serial monitor
    last_time = millis();//remember when this happened so we perform next lookup in GETTING_PERIOD milliseconds
  }
}


uint8_t char_append(char* buff, char c, uint16_t buff_size) {
  int len = strlen(buff);
  if (len > buff_size) return false;
  buff[len] = c;
  buff[len + 1] = '\0';
  return true;
}


void do_http_GET(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial) {
  WiFiClient client; //instantiate a client object
  if (client.connect(host, 80)) { //try to connect to host on port 80
    if (serial) Serial.print(request);//Can do one-line if statements in C without curly braces
    client.print(request);
    memset(response, 0, response_size); //Null out (0 is the value of the null terminator '\0') entire buffer
    uint32_t count = millis();
    while (client.connected()) { //while we remain connected read out data coming back
      client.readBytesUntil('\n', response, response_size);
      if (serial) Serial.println(response);
      if (strcmp(response, "\r") == 0) { //found a blank line! (end of response header)
        break;
      }
      memset(response, 0, response_size);
      if (millis() - count > response_timeout) break;
    }
    memset(response, 0, response_size);  //empty in prep to store body
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      char_append(response, client.read(), OUT_BUFFER_SIZE);
    }
    if (serial) Serial.println(response);
    client.stop();
    if (serial) Serial.println("-----------");
  } else {
    if (serial) Serial.println("connection failed :/");
    if (serial) Serial.println("wait 0.5 sec...");
    client.stop();
  }
}
