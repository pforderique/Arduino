#include <WiFi.h> //Connect to WiFi Network
#include <SPI.h>
#include <TFT_eSPI.h>
#include <mpu6050_esp32.h>
#include<math.h>
#include<string.h>

TFT_eSPI tft = TFT_eSPI();
const int SCREEN_HEIGHT = 160;
const int SCREEN_WIDTH = 128;
const int BUTTON_PIN = 5;
const int LOOP_PERIOD = 40;

MPU6050 imu; //imu object called, appropriately, imu

char network[] = "PCP-0672";  //SSID for 6.08 Lab
char password[] = "Limaperuu1"; //Password for 6.08 Lab

//Some constants and some resources:
const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
char old_response[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP request
char response[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP request

uint32_t primary_timer;

int old_val;

//used to get x,y values from IMU accelerometer!
void get_angle(float* x, float* y) {
  imu.readAccelData(imu.accelCount);
  *x = imu.accelCount[0] * imu.aRes;
  *y = imu.accelCount[1] * imu.aRes;
}

void lookup(char* query, char* response, int response_size) {
  char request_buffer[200];

  sprintf(request_buffer, "GET http://608dev-2.net/sandbox/sc/porderiq/ex05/wiki_interfacer.py?topic=%s&len=200 HTTP/1.1\r\n", query);
  strcat(request_buffer, "Host: 608dev-2.net\r\n");
  strcat(request_buffer, "\r\n"); //new line from header to body

  do_http_request("608dev-2.net", request_buffer, response, response_size, RESPONSE_TIMEOUT, true);
}


class Button {
  //YOUR BUTTON CLASS IMPLEMENTATION!!!
  public:
  uint32_t state_2_start_time;
  uint32_t button_change_time;    
  uint32_t debounce_duration;
  uint32_t long_press_duration;
  uint8_t pin;
  uint8_t flag;
  bool button_pressed;
  uint8_t state; // This is public for the sake of convenience
  
  Button(int p) {
    flag = 0;  
    state = 0;
    pin = p;
    state_2_start_time = millis(); //init
    button_change_time = millis(); //init
    debounce_duration = 10;
    long_press_duration = 1000;
    button_pressed = 0;
  }
  void read() {
    uint8_t button_state = digitalRead(pin);  
    button_pressed = !button_state;
  }
  int update() {
    read();
    flag = 0;
    if (state==0) {
      if (button_pressed) {
        state = 1;
        button_change_time = millis(); // Start timer
      }
    } else if (state==1) {
      if (button_pressed) {
        if (millis() - button_change_time >= debounce_duration) {
          state = 2; 
          state_2_start_time = millis(); // start the state 2 timer
        }
      } 
      else {
        state = 0;  // The button returns to unpressed 
        button_change_time = millis(); // Start timer
      }
    } else if (state==2) {
        if (button_pressed) {
          if (millis() - state_2_start_time >= long_press_duration) {
            state = 3; // Enter long state
          }
        } 
        else {
          state = 4;  // The button returns to unpressed temp state
          button_change_time = millis(); // Start timer // add this to get 64%
        }
    } else if (state==3) {
        if (!button_pressed) {
          state = 4;
          button_change_time = millis(); // Start timer // add this to get 64%
        }
    } else if (state==4) {      
        if(!button_pressed){
          if (millis() - button_change_time >= debounce_duration) {
            if (millis() - state_2_start_time <= long_press_duration) {
              flag = 1; //This has come from the short press
            }
            else {
              flag = 2; //Came from long press
            }
           // button_change_time = millis(); // Start timer
            state = 0; //Reset state
          } 
        } 
        // button is pressed! go to 2 or 3
        else{
          button_change_time = millis(); // Start timer // add this to get 64%
          if (millis() - state_2_start_time >= long_press_duration) {
            state = 3; // go back to long state
          } 
          else if(millis() - state_2_start_time <= long_press_duration) {
            state = 2; // go back to long state
          } 
        }
    }
    return flag;
  }
};

class WikipediaGetter {
    char alphabet[50] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    char msg[400] = {0}; //contains previous query response
    char query_string[50] = {0};
    int char_index;
    int state;
    uint32_t scrolling_timer;
    const int scrolling_threshold = 150;
    const float angle_threshold = 0.3;
  public:

    WikipediaGetter() {
      state = 0;
      memset(msg, 0, sizeof(msg));
      strcat(msg, "Long Press to Start!");
      char_index = 0;
      scrolling_timer = millis();
    }
    void update(float angle, int button, char* output) {
    
      switch(state){
        case 0:
          strcpy(output, msg); // In this state, simply return the most recent query result
          if(button == 2){
            state = 1;
            // memset(query_string, 0, sizeof(query_string));
            // strcpy(output, query_string);
            scrolling_timer = millis();
            char_index = 0;
          }
          break;
        case 1:
          if(button == 1){
            char cc[2];
            cc[0] = alphabet[char_index];
            cc[1] = '\0';
            strcat(query_string, cc);
            strcpy(output, query_string);                  // doesnt make sense fabrizzio
            char_index = 0;
            break;
            
          } else if(button == 2){
            state = 2;
            strcpy(output, "");
            break;
            
          } else if(millis() - scrolling_timer > scrolling_threshold){
            if(abs(angle) > angle_threshold){
              if(angle < 0){
                char_index -= 1;
                if(char_index == -1) char_index = 36;
              }
              else if(angle > 0){
                char_index += 1;
                if(char_index == 37) char_index = 0;
              }
              scrolling_timer = millis();
            }
          }
          char c1[2];
          c1[0] = alphabet[char_index];
          c1[1] = '\0';
          strcpy(output, query_string); 
          strcat(output, c1); 
          
          break;  
        case 2:
          strcpy(output, "Sending Query");
          state = 3;
          break;
        case 3:
          strcat(query_string, "&len=200");
          lookup(query_string, msg, 200);
          strcpy(query_string, "");
          strcpy(output, msg);
          state = 0;
          break;
      }
    }


};

WikipediaGetter wg; //wikipedia object
Button button(BUTTON_PIN); //button object!

void setup() {
  Serial.begin(115200); //for debugging if needed.
  WiFi.begin(network, password); //attempt to connect to wifi
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
    Serial.printf("%d:%d:%d:%d (%s) (%s)\n", WiFi.localIP()[3], WiFi.localIP()[2],
                  WiFi.localIP()[1], WiFi.localIP()[0],
                  WiFi.macAddress().c_str() , WiFi.SSID().c_str());    delay(500);
  } else { //if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
  }
  if (imu.setupIMU(1)) {
    Serial.println("IMU Connected!");
  } else {
    Serial.println("IMU Not Connected :/");
    Serial.println("Restarting");
    ESP.restart(); // restart the ESP (proper way)
  }
  tft.init();
  tft.setRotation(2);
  tft.setTextSize(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK); //set color of font to green foreground, black background
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  primary_timer = millis();
}

void loop() {
  float x, y;
  get_angle(&x, &y); //get angle values
  int bv = button.update(); //get button value
  wg.update(-y, bv, response); //input: angle and button, output String to display on this timestep
  if (strcmp(response, old_response) != 0) {//only draw if changed!
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 0, 1);
    tft.println(response);
  }
  memset(old_response, 0, sizeof(old_response));
  strcat(old_response, response);
  while (millis() - primary_timer < LOOP_PERIOD); //wait for primary timer to increment
  primary_timer = millis();
}
