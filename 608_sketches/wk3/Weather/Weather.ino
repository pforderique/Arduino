#include <WiFi.h> //Connect to WiFi Network
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> //Used in support of TFT Display
#include <string.h>  //used for some string handling and processing.

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

// ** OPTIONS FOR TFT DISPLAY
const char OPTIONS_MENU[] = "OPTIONS MENU:\n1) temperature\n2) time\n3) date\n4) visibility\n5) humidity\n6) pressure";

// ** info array of char arrays for displaying on screen **
const int MAX_DISPLAY_LINES = 20;
char info[MAX_DISPLAY_LINES][100];  // 20 char arrays of size 100
int idx = MAX_DISPLAY_LINES - 1; // start index for adding new info to info array

char network[] = "PCP-0672";
char password[] = "Limaperuu1";
uint8_t channel = 1; //network channel on 2.4 GHz

uint32_t primary_timer = 0;
uint32_t button_timer = 0;
const char USER[] = "porderiq";

//Some constants and some resources:
const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
const int POSTING_PERIOD = 6000; //periodicity of getting a number fact.
const uint16_t IN_BUFFER_SIZE = 1000; //size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
char request_buffer[IN_BUFFER_SIZE]; //char array buffer to hold HTTP request
char response_buffer[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP response

const uint8_t BUTTON1 = 5; //pin connected to button
const uint8_t BUTTON2 = 19; //pin connected to button
const int TOO_FAST = 80;
const int TIMEOUT = 1000; // go ahead and request info if no push after 1 sec

uint8_t num_count = 0; //variable for storing the number of times the button has been pressed before timeout
uint8_t state;  //system state for step counting

// state vars
const uint8_t IDLE = 0;  //change if you'd like
const uint8_t DOWN = 1;  //change if you'd like
const uint8_t UP = 2;

void setup() {
  tft.init();  //init screen
  tft.setRotation(2); //adjust rotation
  tft.setTextSize(1); //default font size
  tft.fillScreen(TFT_BLACK); //fill background
  tft.setTextColor(TFT_GREEN, TFT_BLACK); //set color of font to green foreground, black background

  // show options menu
  display_options();
    
  Serial.begin(115200); //begin serial comms
  delay(100); //wait a bit (100 ms)

  // CONNECT TO WIFI
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

  uint8_t count = 0; //count used for Wifi check times
  Serial.print("Attempting to connect to ");
  Serial.println(network);
  while (WiFi.status() != WL_CONNECTED && count < 12) {
    delay(500);
    Serial.print(".");
    count++;
  }
  delay(2000);
  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
    Serial.println("CONNECTED!");
    Serial.println(WiFi.localIP().toString() + " (" + WiFi.macAddress() + ") (" + WiFi.SSID() + ")");
    delay(500);
  } else { //if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
  }
  
  //pinMode(BUTTON1, INPUT_PULLUP); //set input pin as an input!
  pinMode(BUTTON2, INPUT_PULLUP); //set input pin as an input!

  state = IDLE; 
  primary_timer = millis();

  // empty out the array of strings (info array)
  empty_info_array();
  Serial.println(info[0]);
}


void loop() {
  //get button readings:
  //uint8_t button1 = digitalRead(BUTTON1);
  uint8_t button2 = digitalRead(BUTTON2);
  number_fsm(button2);
}

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
        button_timer = millis();
        Serial.print("num_count: ");
        Serial.println(num_count);
      }
      break;
    case UP:
      // now in up state,
      // if button is pushed 
      if(input == 0){
        // if pushed too quickly, break
        if(millis() - button_timer < TOO_FAST){
          break;
        }
        //else we have pushed the button so go back to DOWN state
        state = DOWN;
        num_count += 1;
      // else the time is up, go to the idle state
      }else if(millis() - button_timer > TIMEOUT){
        state = IDLE;
        get_response(num_count); // **** HERE IS WHERE WE CALL OUR RESPONSE!!!!!!!!!!!!
        // now we add result of response_buffer to info array
        add_to_info(response_buffer);
        update_lcd();
      }
      break;
  }
}

// *** displays options on the tft display *** 
void display_options() {
  tft.fillScreen(TFT_BLACK); //fill background
  tft.setTextColor(TFT_GREEN, TFT_BLACK); //set color of font to green foreground, black background
  tft.setTextSize(1);
  tft.println(OPTIONS_MENU);
}

// *** update the information on the screen *** 
void update_lcd() {
  tft.fillScreen(TFT_BLACK); 
  tft.setTextColor(TFT_BLUE, TFT_BLACK);
  tft.setCursor(0,1);
  Serial.println("LCD UPDATED. INFO VALUES:");
  for(int i = MAX_DISPLAY_LINES - 1; i >= 0; i--){
    tft.println(info[i]);
    Serial.println(info[i]);
  }
}

// *** ADD TO "PRINT QUEUE" First In, First Printed  ***
void add_to_info(char* new_info){
  // SHIFT EVERYTHING BACK MY GUY 
  for(int i = 0; i < MAX_DISPLAY_LINES - 1; i++){
    strcpy(info[i], info[i+1]);
  }
  // write in new value to the queue
  strcpy(info[MAX_DISPLAY_LINES - 1], new_info);
}

// ** write empty strings to INFO array **
void empty_info_array(){
  for(int i = 0; i < MAX_DISPLAY_LINES; i++){
    strcpy(info[i], "");
  }
}

// *** get response for option that user selected and write to response_buffer ***
void get_response(int option) {
  char output[100];
  sprintf(output, "Number received for you to figure out how to request now: %d", num_count);  // SEND THIS NUM TO PROXY SERVER? 
  Serial.println(output);
  sprintf(request_buffer,"GET http://608dev-2.net/sandbox/sc/porderiq/weatherdesign/weatherproxy.py?num=%d HTTP/1.1\r\n", num_count);
  strcat(request_buffer,"Host: 608dev-2.net\r\n"); //add more to the end
  strcat(request_buffer,"\r\n"); //add blank line!
  
  // submist that to the API - which will return output using response_buffer
  do_http_GET("608dev-2.net",request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
  Serial.println(response_buffer);
}
