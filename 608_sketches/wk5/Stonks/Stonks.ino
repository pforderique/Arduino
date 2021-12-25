#include <WiFi.h> //Connect to WiFi Network
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> //Used in support of TFT Display
#include <string.h>  //used for some string handling and processing.
// #include <ArduinoJson.h> // for JSONing with easeee


TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

// ** SELECTION LIST to choose from 
const char SELECTION_LIST[] = "SELECT A STOCK:\n1) TSLA\n2) MSFT\n3) AAPL\n4) AMZN\n5) UAL\n6) DIS\n7) NCLH\n8) DAL\n9) NVDA\n10) TGT";

// ** for holding the ticker selected -- sent to get request**
int num_count;
char ticker[15]; 
char curr_price[15];
char prev_price[15] = "0";
uint16_t stock_timer; // only want to GET every 10 seconds

// WIFI Credentials
char network[] = "PCP-0672";
char password[] = "Limaperuu1";
uint8_t channel = 1; 

// HTTP constants
const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
const int POSTING_PERIOD = 6000; //periodicity of getting a number fact.
const uint16_t IN_BUFFER_SIZE = 1000; //size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 3000; //size of buffer to hold HTTP response
char request_buffer[IN_BUFFER_SIZE]; //char array buffer to hold HTTP request
char response_buffer[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP response

const uint8_t BUTTON1 = 5; 
const uint8_t BUTTON2 = 19; 
const uint8_t TOO_FAST = 100;
const int TIMEOUT = 1000; // when does user stop clicking
const uint16_t GET_TIMEOUT = 10000; // only want to GET every 10 seconds
uint16_t b1_timer;
uint16_t debounce_timer;
uint8_t prev_b2 = 1;

// states for stonks
uint8_t state; 
const uint8_t SELECTION_SCREEN = 0;
const uint8_t STOCK_SCREEN = 1;  
// states for button selector
uint8_t b_state;
const uint8_t IDLEE = 0;
const uint8_t UP = 1;
const uint8_t DOWN = 2;

// LED light pins
const uint8_t RED_PIN = 14;
const uint8_t GREEN_PIN = 27;
const uint8_t BLUE_PIN = 12;

void setup() {
  tft.init();   
  tft.setRotation(2); //1 for normal sideways
  tft.setTextSize(1);
  tft.fillScreen(TFT_BLACK); //fill background
  tft.setTextColor(TFT_GREEN, TFT_BLACK); //set color of font to green foreground, black background

  tft.println("Connecting to Wifi...");
    
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

  pinMode(BUTTON1, INPUT_PULLUP); //set input pin as an input!
  pinMode(BUTTON2, INPUT_PULLUP); //set input pin as an input!
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  
  // show selections right off the bat
  show_selections();
  
  state = SELECTION_SCREEN; 
  b_state = IDLEE;
  b1_timer = millis();
  debounce_timer = millis();
  stock_timer = millis();
}

void loop() {
  //get button readings:
  uint8_t input1 = digitalRead(BUTTON1);
  uint8_t input2 = digitalRead(BUTTON2);
  stock(input1, input2);

  prev_b2 = input2;
  strcpy(prev_price, curr_price);
}

void stock(uint8_t input1, uint8_t in2){
  switch(state){
    case SELECTION_SCREEN:
      // will switch the state once a number is selected
      number_fsm(input1);
      break; 

    case STOCK_SCREEN:
      // if user presses a button, return to selection screen 
      if(button_pressed(in2, prev_b2, &debounce_timer)){
        state = SELECTION_SCREEN;
        show_selections();
        
        // reset everything
        reset_everything();
        Serial.println("Resetted");
        break;
      }

      if (millis() - stock_timer > GET_TIMEOUT) {
        Serial.println("10 secs have passed! getting new update");
        get_stock_price(ticker); 
        display_stock();
        update_led();
        stock_timer = millis();
      }
      break;
  }
}

// *** how many times button has been pushed *** 
void number_fsm(uint8_t input){
  switch(b_state){
    case IDLEE:
      if(input == 0){
        b_state = DOWN;
        num_count = 1;
      }
      break; 
    case DOWN:
      if(input == 1){
        b_state = UP;
        b1_timer = millis();
        Serial.print("num_count: ");
        Serial.println(num_count);
      }
      break;
    case UP:
      if(input == 0){
        if(millis() - b1_timer < TOO_FAST){ break; }

        b_state = DOWN;
        num_count += 1;

      }else if(millis() - b1_timer > TIMEOUT){
        b_state = IDLEE;
        state = STOCK_SCREEN; // go to stock screen!

        create_ticker(num_count);
        get_stock_price(ticker); 
        display_stock();
        strcpy(prev_price, "0"); // reset previous price
        update_led(); 
        
        num_count = 0;
        stock_timer = millis(); // reset the clock
      }
      break;
  }
}

// *** returns whether or not button has been pushed *** 
bool button_pressed(uint8_t in, uint8_t prev_in, uint16_t* timer){
  // DONT return true if pressed too quickly
  if(millis() - *timer < TOO_FAST){ return false; }

  if(in == 1 && in != prev_in){
    *timer = millis(); // restart the actualy timer for this button
    Serial.println("PRESSED!"); // fo' debugin'
    return true;
  }
  return false;
}

// *** Given a selection (num), assign a ticker to the ticker varaible. Returns False if nothing assigned *** 
bool create_ticker(int num){
  if(num < 1 || num > 10) return false;
  
  // write ticker in appropriately 
  if(num == 1)      { strcpy(ticker, "TSLA"); }
  else if(num == 2) { strcpy(ticker, "MSFT"); }
  else if(num == 3) { strcpy(ticker, "AAPL"); }
  else if(num == 4) { strcpy(ticker, "AMZN"); }
  else if(num == 5) { strcpy(ticker, "UAL"); }
  else if(num == 6) { strcpy(ticker, "DIS"); }
  else if(num == 7) { strcpy(ticker, "NCLH"); }
  else if(num == 8) { strcpy(ticker, "DAL"); }
  else if(num == 9) { strcpy(ticker, "NVDA"); }
  else if(num == 10){ strcpy(ticker, "TGT"); }

  return true;
}

// *** show new stock *** 
void display_stock() {
  tft.fillScreen(TFT_BLACK); 
  tft.setTextSize(2);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setCursor(0,1);
  Serial.println("LCD UPDATED:");
  Serial.print("STOCK TICKER:");
  Serial.println(ticker);    
  
  // place into actual TFT here
  char output[30];
  sprintf(output, "Stock %s\n", ticker);
  tft.println(output);

  char output1[30];
  sprintf(output1, "Current\nPrice:\n%s", curr_price);
  tft.println(output1);
}

// *** displays 10 stock options on the tft display *** 
void show_selections() {
  tft.fillScreen(TFT_BLACK); 
  tft.setTextSize(1);
  tft.setTextColor(TFT_GREEN, TFT_BLACK); //set color of font to green foreground, black background
  tft.setCursor(0,1);
  tft.println(SELECTION_LIST); 
}

// *** update the led color based on new stock price *** 
void update_led(){
  // turn off everything for good measure
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);
  // we will always "start" at green since prev price gets set to "0" after each new selection
  float prev = atof(prev_price);
  float curr = atof(curr_price);
  Serial.print("Previous Price: ");
  Serial.println(prev);
  Serial.print("Current Price :");
  Serial.println(curr);
  if(curr > prev){ Serial.println("GREEN"); digitalWrite(GREEN_PIN, HIGH);} 
  else if(curr < prev){ Serial.println("RED"); digitalWrite(RED_PIN, HIGH);} 
  else { Serial.println("PURPLE"); digitalWrite(BLUE_PIN, HIGH); digitalWrite(RED_PIN, HIGH);} 
}

void reset_everything(){
  strcpy(prev_price, "0");
  b_state = IDLEE;
  b1_timer = millis();
  debounce_timer = millis();
  stock_timer = millis();
  num_count = 0;
  // turn off everything for good measure  --- can get rid of this i guess?
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);
}

// *** get stock price from python proxy! *** 
void get_stock_price(char* ticker){
  Serial.print("GETTING PRICE OF TICKER: ");
  Serial.println(ticker);

  sprintf(request_buffer,"GET http://608dev-2.net/sandbox/sc/porderiq/desex/stonksex/stonksproxy.py?ticker=%s HTTP/1.1\r\n", ticker);
  strcat(request_buffer,"Host: 608dev-2.net\r\n"); //add more to the end
  strcat(request_buffer,"\r\n"); //add blank line!
  do_http_GET("608dev-2.net",request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
  Serial.println(response_buffer);

  // write stock price into this current price
  strcpy(curr_price, response_buffer);
}
