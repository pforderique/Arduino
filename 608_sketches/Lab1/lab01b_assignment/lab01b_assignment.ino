#include <WiFi.h> //Connect to WiFi Network
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> //Used in support of TFT Display
#include <string.h>  //used for some string handling and processing.

const uint8_t IDLE = 0; //example definition
const uint8_t DOWN = 1; //example...
const uint8_t UP = 2; //change if you want!
//add more if you want!!!

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

//Some constants and some resources:
const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
const int GETTING_PERIOD = 2000; //periodicity of getting a number fact.
const int BUTTON_TIMEOUT = 1000; //button timeout in milliseconds
const uint16_t IN_BUFFER_SIZE = 1000; //size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
char request_buffer[IN_BUFFER_SIZE]; //char array buffer to hold HTTP request
char response_buffer[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP response

char network[] = "PCP-0672";
char password[] = "Limaperuu1";

uint8_t channel = 1; //network channel on 2.4 GHz
byte bssid[] = {0x04, 0x95, 0xE6, 0xAE, 0xDB, 0x41}; //6 byte MAC address of AP you're targeting.


const int BUTTON = 5; //pin connected to button 
uint8_t state = IDLE;  //system state (feel free to use)
uint8_t displayText = 0;
uint8_t num_count = 0; //variable for storing the number of times the button has been pressed before timeout
unsigned long timer;  //used for storing millis() readings.
uint8_t prev_button_input; 

uint8_t TIMEOUT = 1000;

void setup(){
  tft.init();  //init screen
  tft.setRotation(2); //adjust rotation
  tft.setTextSize(1); //default font size
  tft.fillScreen(TFT_BLACK); //fill background
  tft.setTextColor(TFT_GREEN, TFT_BLACK); //set color of font to green foreground, black background
  Serial.begin(115200); //begin serial comms


  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      Serial.printf("%d: %s, Ch:%d (%ddBm) %s ", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "open" : "");
      uint8_t* cc = WiFi.BSSID(i);
      for (int k = 0; k < 6; k++) {
        Serial.print(*cc, HEX);
        if (k != 5) Serial.print(":");
        cc++;
      }
      Serial.println("");
    }
  }
  delay(100); //wait a bit (100 ms)

  //if using regular connection use line below:
  WiFi.begin(network, password);
  //if using channel/mac specification for crowded bands use the following:
  //WiFi.begin(network, password, channel, bssid);
  uint8_t count = 0; //count used for Wifi check times
  Serial.print("Attempting to connect to ");
  Serial.println(network);
  while (WiFi.status() != WL_CONNECTED && count<6) {
    delay(500);
    Serial.print(".");
    count++;
  }
  delay(2000);
  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
    Serial.println("CONNECTED!");
    Serial.printf("%d:%d:%d:%d (%s) (%s)\n",WiFi.localIP()[3],WiFi.localIP()[2],
                                            WiFi.localIP()[1],WiFi.localIP()[0], 
                                          WiFi.macAddress().c_str() ,WiFi.SSID().c_str());
    delay(500);
  } else { //if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
  }
  pinMode(BUTTON, INPUT_PULLUP); //set input pin as an input!
  state = IDLE; //start system in IDLE state!
}

void loop(){
  uint8_t in = digitalRead(BUTTON);
  number_fsm(in); //Call our FSM every time through the loop.
  prev_button_input = in;
}


/*------------------
 * number_fsm Function:
 * Use this to implement your finite state machine. It takes in an input (the reading from a switch), and can use
 * state as well as other variables to be your state machine.
 */
int TOO_FAST = 80;
void number_fsm(uint8_t input){
  switch(state){
    case IDLE:
      // if button is ever pushed, go to the DOWN state
      if(input == 0){
        state = DOWN;
        num_count = 1;
      }
      break; 
    case DOWN:
      // now that we are in down state,
      // if button is released, go to UP state and start a timer for next push
      if(input == 1){
        state = UP;
        timer = millis();
        Serial.print("num_count: ");
        Serial.println(num_count);
      }
      break;
    case UP:
      // now in up state,
      // if button is pushed 
      if(input == 0){
        // if pushed too quickly, break
        if(millis() - timer < TOO_FAST){
          break;
        }
        //else we have pushed the button so go back to DOWN state
        state = DOWN;
        num_count += 1;
      // else the time is up, go to the idle state
      }else if(millis() - timer > TIMEOUT){
        state = IDLE;
        get_response(num_count);
      }
      break;
  }
}

void get_response(int num_count) {
    state = IDLE;
    //formulate GET request...first line:
    sprintf(request_buffer,"GET http://numbersapi.com/%d/trivia HTTP/1.1\r\n", num_count);
    strcat(request_buffer,"Host: numbersapi.com\r\n"); //add more to the end
    strcat(request_buffer,"\r\n"); //add blank line!
    // submist that to the API - which will return output using response_buffer
    do_http_GET("numbersapi.com",request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
//    Serial.println(response_buffer);
    // update display
    tft.fillScreen(TFT_YELLOW);
    tft.setTextColor(TFT_BLACK, TFT_BLACK);
    tft.drawString(response_buffer,0,0,1);
}

 /*********** TRASH ********/
//boolean printed = false;
//void number_fsm(uint8_t input){
//  //your logic here!
//  //use your inputs and the system's state to update state and perform actions
//  //This function should be non-blocking, meaning there should be minimal while loops and other things in it!
//  //Feel free to call do_http_GET from this function
//  //state variable globally defined at top
//  switch(state){
//    case IDLE:
//      if(response_buffer != "" and !printed){
//        Serial.println(response_buffer);
//        tft.fillScreen(TFT_YELLOW);
//        tft.setTextColor(TFT_BLACK, TFT_BLACK);
//        tft.drawString(response_buffer,0,0,1);
//        printed = true;
//      }
//
//      // if button was pressed, go to the down state
//      if (input != prev_button_input && input == 0){
////       num_count++;
////       timer = millis(); 
//       state = DOWN;
//      }
//      break; 
//    case DOWN:
////      Serial.println("DOWN STATE");
//      timer = millis();
//      num_count++;
//      while (millis()-timer < TIMEOUT){
//        // if released , go to up state
//        if(digitalRead(BUTTON) == 1){
////          timer = millis();
//          state = UP;
//          return;
//        }
//      }
//      // while loop over. get http req with num_count
//      state = IDLE;
//      //formulate GET request...first line:
//      sprintf(request_buffer,"GET http://numbersapi.com/%d/trivia HTTP/1.1\r\n", num_count);
//      strcat(request_buffer,"Host: numbersapi.com\r\n"); //add more to the end
//      strcat(request_buffer,"\r\n"); //add blank line!
//      do_http_GET("numbersapi.com",request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
//      num_count = 0;
//      printed = false;
//      break;
//    case UP:
////      Serial.println("UP STATE");
//      while (millis()-timer < TIMEOUT){
//        // if pressed , go to down state
//        if(digitalRead(BUTTON)== 0){
//          state = DOWN;
//          return;
//        }
//      }
//      // while loop over. gett http req
//      state = IDLE;
//      //formulate GET request...first line:
//      sprintf(request_buffer,"GET http://numbersapi.com/%d/trivia HTTP/1.1\r\n", num_count);
//      strcat(request_buffer,"Host: numbersapi.com\r\n"); //add more to the end
//      strcat(request_buffer,"\r\n"); //add blank line!
//      do_http_GET("numbersapi.com",request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
//      num_count = 0; 
//      printed = false;
//      break;
//  }
//}


/*----------------------------------
 * char_append Function:
 * Arguments:
 *    char* buff: pointer to character array which we will append a
 *    char c: 
 *    uint16_t buff_size: size of buffer buff
 *    
 * Return value: 
 *    boolean: True if character appended, False if not appended (indicating buffer full)
 */
uint8_t char_append(char* buff, char c, uint16_t buff_size) {
        int len = strlen(buff);
        if (len>buff_size) return false;
        buff[len] = c;
        buff[len+1] = '\0';
        return true;
}

/*----------------------------------
 * do_http_GET Function:
 * Arguments:
 *    char* host: null-terminated char-array containing host to connect to
 *    char* request: null-terminated char-arry containing properly formatted HTTP GET request
 *    char* response: char-array used as output for function to contain response
 *    uint16_t response_size: size of response buffer (in bytes)
 *    uint16_t response_timeout: duration we'll wait (in ms) for a response from server
 *    uint8_t serial: used for printing debug information to terminal (true prints, false doesn't)
 * Return value:
 *    void (none)
 */
void do_http_GET(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial){
  WiFiClient client; //instantiate a client object
  if (client.connect(host, 80)) { //try to connect to host on port 80
    if (serial) Serial.print(request);//Can do one-line if statements in C without curly braces
    client.print(request);
    memset(response, 0, response_size); //Null out (0 is the value of the null terminator '\0') entire buffer
    uint32_t count = millis();
    while (client.connected()) { //while we remain connected read out data coming back
      client.readBytesUntil('\n',response,response_size);
      if (serial) Serial.println(response);
      if (strcmp(response,"\r")==0) { //found a blank line!
        break;
      }
      memset(response, 0, response_size);
      if (millis()-count>response_timeout) break;
    }
    memset(response, 0, response_size);  
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      char_append(response,client.read(),OUT_BUFFER_SIZE);
    }
    if (serial) Serial.println(response);
    client.stop();
    if (serial) Serial.println("-----------");  
  }else{
    if (serial) Serial.println("connection failed :/");
    if (serial) Serial.println("wait 0.5 sec...");
    client.stop();
  }
}        
