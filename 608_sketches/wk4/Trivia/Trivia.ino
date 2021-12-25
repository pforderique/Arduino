#include <WiFi.h> //Connect to WiFi Network
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> //Used in support of TFT Display
#include <string.h>  //used for some string handling and processing.
#include <ArduinoJson.h> // for JSONing with easeee

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

// ** TITLE SCREEN 
const char TITLE_SCREEN[] = "WELCOME TO TRIVIA\n\n\n\n\nPress any button";

// ** for holding the question and correct answer, and user response **
char QUESTION[1000];
char DIFFICULTY[15]; 
char CORRECT_ANSWER[10]; // "True\0" or "False/0"
char total_score[5];    // maxes out at 9999
char RESULT[25];
const char correct_message[] = "CORRECT! You rock!";
const char incorrect_message[] = "WRONG :(";

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
const int TOO_FAST = 100;
uint16_t b1_timer;
uint16_t b2_timer; 
uint8_t prev_b1 = 1;
uint8_t prev_b2 = 1;

uint8_t state; 
const uint8_t WELCOME_SCREEN = 0;
const uint8_t TRIVIA_SCREEN = 1;  
const uint8_t RESULT_SCREEN = 2;

void setup() {
  tft.init();   
  tft.setRotation(1); //2 is normal
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
  
  // show welcome screen menu
  welcome_screen();
  
  pinMode(BUTTON1, INPUT_PULLUP); //set input pin as an input!
  pinMode(BUTTON2, INPUT_PULLUP); //set input pin as an input!

  state = WELCOME_SCREEN; 
  b1_timer = millis();
  b2_timer = millis();

}

void loop() {
  //get button readings:
  uint8_t input1 = digitalRead(BUTTON1);
  uint8_t input2 = digitalRead(BUTTON2);
  trivia(input1, input2);

//  // testing these buttons for now for getting trivia
//  if( button_pressed(input1, prev_b1, &b1_timer) ){
//    GET_score(0);
//    //send_result("True");
//  }; 
//  if( button_pressed(input2, prev_b2, &b2_timer) ){
//    GET_trivia();
//    //display_trivia();
//  }

  prev_b1 = input1;
  prev_b2 = input2;
}

void trivia(uint8_t input1, uint8_t input2){
  switch(state){
    case WELCOME_SCREEN:
      // if a button is pressed, go on to TRIVIA_SCREEN
      if(button_pressed(input1, prev_b1, &b1_timer) ||
         button_pressed(input2, prev_b2, &b2_timer)){
        state = TRIVIA_SCREEN;
        GET_score(0); //get total score from proxy
        GET_trivia(); // CALL TRIVIA FIRST
        display_trivia(); // THEN display the trivia question screen - score, question -- store correct answer
      }
      break; 

    case TRIVIA_SCREEN:
      // if user presses the True button - send "True" to send_result
      if(button_pressed(input1, prev_b1, &b1_timer)){
        state = RESULT_SCREEN;
        send_result("True");
        display_result();
        break;
      }
      // if user presses the False button -- send "False" to send_result
      if(button_pressed(input2, prev_b2, &b2_timer)){
        state = RESULT_SCREEN;
        send_result("False");
        display_result();
        break;
      }
      break;

    case RESULT_SCREEN:
      // if a button is pressed, go on to TRIVIA_SCREEN
      if(button_pressed(input1, prev_b1, &b1_timer) ||
         button_pressed(input2, prev_b2, &b2_timer)){
        state = TRIVIA_SCREEN;
        GET_trivia(); // CALL TRIVIA FIRST
        display_trivia(); // THEN display the trivia question screen - score, question -- store correct answer
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

// *** displays welcome screen on the tft display *** 
void welcome_screen() {
  tft.fillScreen(TFT_BLACK); 
  tft.setTextColor(TFT_GREEN, TFT_BLACK); //set color of font to green foreground, black background
  tft.setTextSize(1);
  tft.println(TITLE_SCREEN);
}

// *** show new question on the screen *** 
void display_trivia() {
  tft.fillScreen(TFT_BLACK); 
  tft.setTextColor(TFT_PINK, TFT_BLACK);
  tft.setCursor(0,1);
  Serial.println("LCD UPDATED:");
  Serial.print("Question:");
  Serial.println(QUESTION);     // stack smash error here ********************************************
  
  // place into actual TFT here
  char output[30];
  sprintf(output, "Total Global Score: %s\n", total_score);
  tft.println(output);

  char output1[30];
  sprintf(output1, "Difficulty: %s\n", DIFFICULTY);
  tft.println(output1);
  
  tft.println(QUESTION);
  tft.println("\n");
  tft.println("1) TRUE");
  tft.println("2) FALSE");
}

// *** show the results on the screen *** 
void display_result() {
  tft.fillScreen(TFT_BLACK); 
  tft.setTextColor(TFT_BLUE, TFT_BLACK);
  tft.setCursor(0,1);

  tft.println(RESULT); // either correct or incorrect message
  tft.println("\n");
  char output[30];
  sprintf(output, "Updated Score: %s\n", total_score);
  tft.println(output);
  tft.println("\n\n");
  tft.println("Any button to continue");
}

// *** process user response and update score accordingly ***
void send_result(char* user_response){
  Serial.print("User said: ");
  Serial.println(user_response);
  Serial.print("Correct Answer is: ");
  Serial.println(CORRECT_ANSWER);
  Serial.print("Comparison Result: ");
  Serial.println(strcmp(user_response, CORRECT_ANSWER));

  // if user got it right, write in correct_message to be displayed AND UPDATE the global score
  if(strcmp(user_response, CORRECT_ANSWER) < 0){ // for some reason, the Arduino was giving me a negative answer when passed in True 
    strcpy(RESULT, correct_message); 
    GET_score(1); // +1 for getting it correct!
  }
  // else write in incorrect_message to be displayed 
  else { 
    strcpy(RESULT, incorrect_message); 
    GET_score(-1); // -1 for getting it correct!
  }
}

// *** get trivia!! write into QUESTION and RESPONSE ***
void GET_trivia(){
  Serial.println("GETTING TRIVIA");

  strcpy(request_buffer,"GET http://608dev-2.net/sandbox/sc/porderiq/desex/triviaex/triviaquestion.py HTTP/1.1\r\n");
  strcat(request_buffer,"Host: 608dev-2.net\r\n"); 
  strcat(request_buffer,"\r\n"); 
  
  // submist that to the API - which will return output using response_buffer
  do_http_GET("608dev-2.net",request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
  Serial.println(response_buffer);

  parse_trivia(response_buffer); // parse the trivia in format - MacOS is based on Linux.:medium:False - to QUESTION, DIFFICULTY, and CORRECT_ANSWER
}

// *** parse trivia information from my python response *** 
void parse_trivia(char* response){
  char* ptr = strtok(response, ":");
  strcpy(QUESTION, ptr);
  ptr = strtok(NULL, ":");
  strcpy(DIFFICULTY, ptr);
  ptr = strtok(NULL, ":");
  strcpy(CORRECT_ANSWER, ptr);

  Serial.println(QUESTION); Serial.println(DIFFICULTY); Serial.println(CORRECT_ANSWER);
}

// *** get total score from python proxy! ***  -- call GET_score(0) to just get current score
void GET_score(int updatee){
  Serial.print("GETTING SCORE WITH UPDATE: ");
  Serial.println(updatee);

  sprintf(request_buffer,"GET http://608dev-2.net/sandbox/sc/porderiq/desex/triviaex/triviaproxy.py?num=%d HTTP/1.1\r\n", updatee);
  strcat(request_buffer,"Host: 608dev-2.net\r\n"); //add more to the end
  strcat(request_buffer,"\r\n"); //add blank line!
  
  // submist that to the API - which will return output using response_buffer
  do_http_GET("608dev-2.net",request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
  Serial.println(response_buffer);

  strcpy(total_score, response_buffer);
}
