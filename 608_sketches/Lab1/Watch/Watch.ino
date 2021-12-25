#include <WiFi.h>     // Connect to WiFi Network
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>      // Used in support of TFT Display
#include <string.h>   // used for some string handling and processing.

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

//Some constants and some resources:
const int RESPONSE_TIMEOUT = 6000;      //ms to wait for response from host
const int GETTING_PERIOD = 2000;        //periodicity of getting a number fact.
const uint16_t IN_BUFFER_SIZE = 1000;   //size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 1000;  //size of buffer to hold HTTP response
char request_buffer[IN_BUFFER_SIZE];    //char array buffer to hold HTTP request
char response_buffer[OUT_BUFFER_SIZE];  //char array buffer to hold HTTP response

char network[] = "PCP-0672";
char password[] = "Limaperuu1";

uint8_t channel = 1;                                  //network channel on 2.4 GHz
byte bssid[] = {0x04, 0x95, 0xE6, 0xAE, 0xDB, 0x41};  //6 byte MAC address of AP you're targeting.

// STATES of the watch
const uint8_t DIGITAL = 0; 
const uint8_t ANALOGG = 1;          //ANALOG gave an error - so name was changed to ANALOGG

const int BUTTON = 5;               //pin connected to button 
const int REQ_TIMEOUT = 1000*60;    //1MIN - request timeout for knowing when to send another http request for time
const int RENDER_TIMEOUT = 1000;      // render only every second

unsigned long internal_timer;       //internal timer for when getting time solo and for knowing when to request again
unsigned long button_timer;         //button_timer to handle bouncing
unsigned long render_timer;         //timer to handle how fast screen is rendered

uint8_t state;                      //system state starts with digital watch screen
uint8_t prev_button_input;    
// for the formatted time from API
char full_time[9];     // HH:MM:SS/0
char date[11];  //10 chars for date +1 for /0
char hour[3];   //10 chars for date +1 for /0
char minute[3];
char second[3];
// for the ACTUAL TIME using the updated internal_timer (NOT neccessarily in HH, MM, or SS format)
int hr;
int mm;
int ss;
int prev_ss; // for millisecond errors in getting time

int tens;
int ones;

// FOR THE ANALOG CLOCK DISPLAY
// WxH = 128x160
uint8_t radius = tft.width()/2 - 1;
uint8_t centerX = tft.width()/2;
uint8_t centerY = tft.height() - 3*centerX/2;

uint8_t SS_HAND_LEN = radius - 5;       // red
uint8_t MM_HAND_LEN = SS_HAND_LEN - 10;  // white
uint8_t HR_HAND_LEN = MM_HAND_LEN - 20;  // orange

void setup(){
  tft.init();  //init screen
  tft.setRotation(2); //adjust rotation
  tft.setTextSize(1); //default font size
  tft.fillScreen(TFT_BLACK); //fill background
  tft.setTextColor(TFT_GREEN, TFT_BLACK); //set color of font to green foreground, black background 
  tft.drawString("Connecting to WiFi..." ,0,0,1); // display a waiting screen
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

  // ********** MY SETUP ************ 
  pinMode(BUTTON, INPUT_PULLUP);
  state = DIGITAL; //start system in DIGITAL state

  get_time(); //will write in the time into response_buffer
  parse_time(response_buffer, full_time, date, hour, minute, second); // will write in to those variables 

  // start... your... timers!
  internal_timer = millis();
  button_timer = millis();
  render_timer = millis();
}

void loop(){
  uint8_t in = digitalRead(BUTTON);
  watch(in);
  prev_button_input = in;
}

void watch(uint8_t in){
  // lets make SURE we have the right time by resending that GET req every now and then
  if(millis() - internal_timer > REQ_TIMEOUT){
    get_time(); //will write in the time into response_buffer
    parse_time(response_buffer, full_time, date, hour, minute, second); // will write in to those variables 

    // reset the interal timer 
    internal_timer = millis();
  }
  // ****update our variables, via our internal_timer (maybe every 1 sec too?)
  set_actual_time();
  
  switch(state){
    case DIGITAL:
      render_digital(); // don't worry - screen isn't ACTUALLY getting drawn every single ms. See function.
      if(button_pressed(in)){
        state = ANALOGG;
      }
      break;
    case ANALOGG:
      render_analog();
      if(button_pressed(in)){
        state = DIGITAL;
      }
      break;
  }
}

// *** write the ACTUAL time - with internal clock to hh, mm, and ss
void set_actual_time(){
  // find out time that has elapsed
  unsigned long time_elapsed = millis() - internal_timer;

  // set the right ss
  tens = second[0] - '0';
  ones = second[1] - '0';
  ss = (10*tens + ones + time_elapsed/1000) % 60;

  // increment mm THE MOMENT a minute has passed
  if(ss == 0 and prev_ss != 0) {
    mm = (mm + 1)%60;
    // increment if this increment caused a new hour
    if(mm == 0) hr = (hr +1 )%24;
  }

  prev_ss = ss;
}

// *** show the "DIGITAL screen" on the tft! -- USING hr, mn, and sc... NOT hour, minute, second!!
void render_digital(){
  // no point in re-rendering if not even a second had passed! 
  if(millis() - render_timer > RENDER_TIMEOUT){
    render_timer = millis();
    
    // our ACTUAL TIME to char array
    char digitalSeconds[3];
    char digitalMinutes[3];
    char digitalHours[3];
    char digitalFull[9]; 

    sprintf(digitalSeconds, "%u", ss); // time is unsigned
    sprintf(digitalMinutes, "%u", mm); // time is unsigned
    sprintf(digitalHours, "%u", hr); // time is unsigned
    strcpy(digitalFull, digitalHours);
    strcat(digitalFull, ":");
    strcat(digitalFull, digitalMinutes);
    strcat(digitalFull, ":");
    strcat(digitalFull, digitalSeconds);
    
    // Draw to console and tft screen 
    Serial.print("DIGITAL Screen: ");
    Serial.println(digitalFull);
    tft.fillScreen(TFT_BLACK); 
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("iWatch" ,29,0,1);
    tft.drawString(digitalFull,23,60,1);
    tft.drawString(date, 2, 120, 1);
  }
}

// *** show the "ANALOG screen" on the tft! -- USING hr, mn, and sc... NOT hour, minute, second!!
void render_analog(){
  if(millis() - render_timer > RENDER_TIMEOUT){
    render_timer = millis();

    // console debug
    Serial.print("ANALOG Screen. Sec: "); 
    Serial.println(ss);
    float angle;
    float x1;
    float y1;

    // clock here
    tft.fillScreen(TFT_BLACK);
    tft.drawCircle(centerX, centerY, radius, TFT_GREEN);

    // draw the second hand
    angle = (PI/2) - (2*PI)*(ss/60.0); // never thought int division would catch up to me
    x1 = centerX + SS_HAND_LEN*cos(angle);
    y1 = centerY - SS_HAND_LEN*sin(angle);
    tft.drawLine(centerX, centerY, x1, y1, TFT_RED);

    // draw the minute hand
    angle = (PI/2) - (2*PI)*(mm/60.0); // never thought int division would catch up to me
    x1 = centerX + MM_HAND_LEN*cos(angle);
    y1 = centerY - MM_HAND_LEN*sin(angle);
    tft.drawLine(centerX, centerY, x1, y1, TFT_WHITE);

    // draw the hour hand
    angle = (PI/2) - (2*PI)*((hr%12)/12.0); // never thought int division would catch up to me
    x1 = centerX + HR_HAND_LEN*cos(angle);
    y1 = centerY - HR_HAND_LEN*sin(angle);
    tft.drawLine(centerX, centerY, x1, y1, TFT_ORANGE);
    
    tft.drawString(date, 2, 140, 1);    
  }
}

// *** return true ONLY if pressed AND RELEASED -- WORKING!! (now with button bounce no mo!)***
bool button_pressed(uint8_t in){
  int TOO_SPEEDY = 100;        // for handling button bounce
  // DONT return true if pressed too quickly
  if(millis() - button_timer < TOO_SPEEDY){
    return false;
  }
  if(in == 1 && in != prev_button_input){
    button_timer = millis(); // restart the button timer
//    Serial.println("PRESSED!"); // fo' debugin'
    return true;
  }
  return false;
}

// *** gets time from API and writes response to request_buffer *** --- WORKING!!!
void get_time() {
    //properly formulate GET request http://iesc-s3.mit.edu/esp32test/currenttime
    strcpy(request_buffer,"GET http://iesc-s3.mit.edu/esp32test/currenttime HTTP/1.1\r\n");
    strcat(request_buffer,"Host: iesc-s3.mit.edu\r\n"); 
    strcat(request_buffer,"\r\n"); 
    // submit that to the API - which will return output using response_buffer
    do_http_GET("iesc-s3.mit.edu",request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
    
    //Console Debugging:
    Serial.print("API Called. Received: ");
    Serial.println(response_buffer);
}

// *** parses time from current time to date, hr, min, and secs *** --- WORKING!!!
void parse_time(char* curr_time, char* full, char* date, char* hh, char* mins, char* secs) {
  // get the date which ends at idx = 10
  memcpy(date, curr_time, 10);
  // write in the FULL TIME (HH:MM:SS) to use for digital face
  memcpy(full, curr_time+11, 8);
  // now copy in the hr, mins, and secs (can do this since curr_time is really a pointer! haha!)
  memcpy(hh, curr_time+11, 2);
  memcpy(mins, curr_time+14, 2);
  memcpy(secs, curr_time+17, 2);

  //set right ss 
  tens = second[0] - '0';
  ones = second[1] - '0';
  ss = 10*tens + ones + 1;

  //set right mm
  tens = minute[0] - '0';
  ones = minute[1] - '0';
  mm = 10*tens + ones;

  // set hr 
  tens = hour[0] - '0';
  ones = hour[1] - '0';
  hr = 10*tens + ones;
}

// ******************* LAB CODE ************************

uint8_t char_append(char* buff, char c, uint16_t buff_size) {
        int len = strlen(buff);
        if (len>buff_size) return false;
        buff[len] = c;
        buff[len+1] = '\0';
        return true;
}

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
