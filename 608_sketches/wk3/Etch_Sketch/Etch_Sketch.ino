#include <WiFi.h> //Connect to WiFi Network
#include <TFT_eSPI.h> 
#include <SPI.h> 
#include <string.h> 
#include <mpu6050_esp32.h>
#include <math.h>
 
TFT_eSPI tft = TFT_eSPI();
MPU6050 imu; //imu object called
 
#define BACKGND_COLOR TFT_BLUE
#define BALL_COLOR TFT_ORANGE

// WIFI 
char network[] = "PCP-0672";
char password[] = "Limaperuu1";
uint8_t channel = 1;                                  //network channel on 2.4 GHz
byte bssid[] = {0x04, 0x95, 0xE6, 0xAE, 0xDB, 0x41};  //6 byte MAC address of AP you're targeting.

// Vectors for acceleration, velocity, and position
struct Vec {
  float x;
  float y;
};
// POINT Structure 
struct Point {
  int x;
  int y;
};

// THE ACTUAL DRAWING REPRESENTED BY PAST POINTS
const int MAX_POINTS = 150; // **********10 for now... should be 246!!!! 
struct Point drawing[MAX_POINTS];
int drawing_idx = 0;  // our current index to record our new points in. %MAX_POINTS somewhere.
char drawingID[15] = "-1"; // to store drawing ID response when we POST -- drawing ID:151917766919
char formatted_points[2000]; // contains 24,45&23,34&,55,60 for example
char output[1000]; // MAX 1000 chars getting send to server

// getting and posting
const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
const int POSTING_PERIOD = 6000; //periodicity of getting a number fact.
const uint16_t IN_BUFFER_SIZE = 2000; //size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
char request_buffer[IN_BUFFER_SIZE]; //char array buffer to hold HTTP request
char response_buffer[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP response

// BALL
const float MASS = 1; 
const int RADIUS = 2;
const int DIAMETER = 2*RADIUS;
const int ZOOM = 9.81; // keep?
// BALL: Physics
struct Vec accel;
struct Vec vel; 
struct Vec pos; 
struct Vec prev_pos;
const int SCALER = 1000; //how much force to apply to ball
const float K_FRICTION = 0.15;  //friction coefficient
const float K_SPRING = 0.9;  //spring coefficient

// boundary constants:
const int WIDTH = tft.width();
const int HEIGHT = tft.height();
const int LEFT_LIMIT = RADIUS; 
const int RIGHT_LIMIT = 127-RADIUS; 
const int TOP_LIMIT = RADIUS; 
const int BOTTOM_LIMIT = 159-RADIUS;

// BUTTONS
const uint8_t BUTTON1 = 5;  // the POST/ERASE button
const uint8_t BUTTON2 = 19; // the GET drawing button
uint8_t prev_b1;
uint8_t prev_b2;
uint8_t num_count;

// state vars
uint8_t state; // button 1 state
const uint8_t IDLE = 0;
const uint8_t DOWN = 1; 
const uint8_t UP = 2;

// TIMING 
const int DT = 40; 
const int RECORD_PERIOD = 100;
const int TOO_FAST = 80;
const int TIMEOUT = 1000; // go ahead and process #clicks
uint16_t button1_timer; // button one needs a way to distinguish if being used for POST or ERASE
uint16_t button2_timer; // for getting
uint32_t primary_timer; // main loop timer
uint16_t record_timer;  // only records every 100 millis

// ***** TESTING *******
//test variables here

//*** updates positions and velocities *** -- copied from exercises
void stepp(float x_force=0, float y_force=0 ){
  //update acceleration (from f=ma)
  accel.x = (x_force - K_FRICTION*vel.x)/MASS;
  accel.y = (y_force - K_FRICTION*vel.y)/MASS;
  //integrate to get velocity from current acceleration
  vel.x = vel.x + 0.001*DT*accel.x; //integrate, 0.001 is conversion from milliseconds to seconds
  vel.y = vel.y + 0.001*DT*accel.y; //integrate!!
  
  moveBall(); 
}
 
void setup() {
  tft.init();
  tft.setRotation(2);
  tft.setTextSize(1);
  tft.fillScreen(BACKGND_COLOR);

  delay(100);
  Serial.begin(115200); 
  pinMode(BUTTON1,INPUT_PULLUP);
  pinMode(BUTTON2,INPUT_PULLUP); // ***** MAY HAVE TO CHANGE TO REGULAR INPUT and then check for 1 FOR PRESSED

  if (imu.setupIMU(1)) {
    Serial.println("IMU Connected!");
  } else {
    Serial.println("IMU Not Connected :/");
    Serial.println("Restarting");
    ESP.restart(); // restart the ESP (proper way)
  }

  // WIFI SETUP
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

  //start ball in center of screen!
  pos.x = WIDTH/2; 
  pos.y = HEIGHT/2; 

  // start timers
  button1_timer = millis();
  button2_timer = millis();
  primary_timer = millis();
  record_timer = millis();
  
  state = IDLE;
  num_count = 0;

  // TESTING THE DRAWING ARRAY
  null_fill_drawing();
//  struct Point TEST_POINT = {50, 60};
//  drawing[0] = TEST_POINT;
//  drawing[1] = {55, 65};
//  drawing[2] = {60, 65};
//  drawing[3] = {65, 70};
//  drawing[4] = {60, 70};
//  drawing[5] = {55, 70};
//  drawing[6] = {50, 65};
//  drawing[7] = {45, 60};
//  drawing[8] = {40, 55};
//  drawing[9] = {35, 50};
//  print_drawing();
//  format_points(formatted_points);

//  char sample_response[] = "drawing~24,45&23,34&55,60";
//  write_to_drawing(sample_response);
  print_drawing();
}
 
void loop() {
  // *** CODE ***
  int input1 = digitalRead(BUTTON1);
  int input2 = digitalRead(BUTTON2);
  post_or_reset(input1);
  if(button2_pressed(input2)){ get_drawing(drawingID); }

  // GET IMU readings and update ball position
  imu.readAccelData(imu.accelCount);//read imu
  float y = -imu.accelCount[0] * imu.aRes;      // **** X ANY Y NEEDED TO BE SWAPPED AND NEGATED
  float x = -imu.accelCount[1] * imu.aRes;
  
  stepp(x * SCALER, y * SCALER); //apply force from imu

  tft.fillCircle(pos.x,pos.y,RADIUS,BALL_COLOR); //draw new ball location

  // ** DO AT END: set prev's = currents, restart timers, whatever
  while (millis()-primary_timer < DT); //wait for primary timer to increment
  primary_timer = millis();
  prev_b2 = input2;
  prev_pos.x = pos.x;
  prev_pos.y = pos.y;
}

void post_or_reset(uint8_t input){
  switch(state){
    case IDLE:
      if(input == 0){
        state = DOWN;
        num_count = 1;
      }
      break; 
    case DOWN:
      if(input == 1){
        state = UP;
        button1_timer = millis();
        Serial.print("num_count: ");
        Serial.println(num_count);
      }
      break;
    case UP:
      if(input == 0){
        // if pushed too quickly, break
        if(millis() - button1_timer < TOO_FAST){ break; }
        //else we have pushed the button so go back to DOWN state
        state = DOWN;
        num_count += 1;
      // else the time is up, go to the idle state
      }else if(millis() - button1_timer > TIMEOUT){
        state = IDLE;
        if(num_count == 1){ // post the drawing 
          post_drawing(); 
          tft.fillScreen(BACKGND_COLOR);
        } else if (num_count == 2) { 
          reset_everything();        // just erase
        }
      }
      break;
  }
}

bool button2_pressed(uint8_t in){
  int TOO_SPEEDY = 100;       
  if(millis() - button2_timer < TOO_SPEEDY){ return false;}
  if(in == 1 && in != prev_b2){
    button2_timer = millis(); // restart the button timer
    Serial.println("PRESSED!"); // fo' debugin'
    return true;
  }
  return false;
}

//*** MOVES THE BALL USING IMU READINGS *** -- copied from exercises
void moveBall() {
  // update position
  pos.x = pos.x + 0.001*DT*vel.x;
  pos.y = pos.y + 0.001*DT*vel.y;
  
  // check TOP, BOTTOM, LEFT, RIGHT
  if(pos.y < TOP_LIMIT){
    vel.y = -K_SPRING*vel.y;
    pos.y = TOP_LIMIT - K_SPRING*(pos.y - TOP_LIMIT);
  }
  if(pos.y > BOTTOM_LIMIT){
    vel.y = -K_SPRING*vel.y;
    pos.y = BOTTOM_LIMIT - K_SPRING*(pos.y - BOTTOM_LIMIT);
  }
  if(pos.x < LEFT_LIMIT){
    vel.x = -K_SPRING*vel.x;
    pos.x = LEFT_LIMIT - K_SPRING*(pos.x - LEFT_LIMIT);
  }
  if(pos.x > RIGHT_LIMIT){
    vel.x = -K_SPRING*vel.x;
    pos.x = RIGHT_LIMIT - K_SPRING*(pos.x - RIGHT_LIMIT);
  }

  // RECORD THIS POINT
  record_point(pos);
}
// records the CURRENT position of the ball
void record_point(Vec ball_position){
  if(millis() - record_timer > RECORD_PERIOD){
    record_timer = millis();
    // DON’T record if position equal previous position
    if ( ball_position.x == prev_pos.x && 
       ball_position.y == prev_pos.y ) {
       return;
    } else {
       drawing[drawing_idx].x = ball_position.x;
       drawing[drawing_idx].y = ball_position.y;
       drawing_idx ++;
       drawing_idx %= MAX_POINTS;  // start to rewrite past points if we've gone over the limit
    }
  }
}
  
// ******************** POSTING/GETTING METHODS ************************
void post_drawing(){
  Serial.print("Drawing Posted: ");
  print_drawing();
  format_points(formatted_points);
  
  char body[2000]; //for body
  sprintf(body,"{\"user\":\"porderiq\",\"drawing\":\"%s\"}", formatted_points);//generate body, posting to User, 1 step
  int body_len = strlen(body); //calculate body length (for header reporting)
  sprintf(request_buffer,"POST http://iesc-s3.mit.edu/esp32test/etchsketch HTTP/1.1\r\n");
  strcat(request_buffer,"Host: iesc-s3.mit.edu\r\n");
  strcat(request_buffer,"Content-Type: application/json\r\n");
  sprintf(request_buffer+strlen(request_buffer),"Content-Length: %d\r\n", body_len); //append string formatted to end of request buffer
  strcat(request_buffer,"\r\n"); //new line from header to body
  strcat(request_buffer,body); //body
  strcat(request_buffer,"\r\n"); //new line
  Serial.println(request_buffer);
  do_http_request("iesc-s3.mit.edu", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT,true);
  Serial.println(response_buffer); // something like drawing ID:161524403556

  char *ptr1 = strtok(response_buffer, ":");
  ptr1 = strtok(NULL, ":");
  strcpy(drawingID, ptr1);
  Serial.print("Drawing ID set to: ");
  Serial.println(drawingID);
}

void get_drawing(char* id){
  Serial.print("get drawing with ID: ");
  Serial.println(id);

  //properly formulate GET request
  sprintf(request_buffer,"GET http://iesc-s3.mit.edu/esp32test/etchsketch?drawing_id=%s HTTP/1.1\r\n", drawingID);
  strcat(request_buffer,"Host: iesc-s3.mit.edu\r\n"); 
  strcat(request_buffer,"\r\n"); 
  Serial.println(request_buffer);
  do_http_GET("iesc-s3.mit.edu",request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
  
  //Console Debugging:
  Serial.print("Drawing Received: ");
  write_to_drawing(response_buffer);
  print_drawing();
  display_current_drawing();
}

// ******************** HELPER METHODS ************************

void reset_everything(){
  null_fill_drawing();
  tft.fillScreen(BACKGND_COLOR);
}

// WRITES IN -1, -1 TO ALL THE POINTS IN DRAWING -- working.
void null_fill_drawing() {
  struct Point null_point = {-1, -1};
  for(int idx = 0; idx < MAX_POINTS; idx++){
    drawing[idx] = null_point;
  }
}

// *** PRINTS first few all points drawing *** -- working.
void print_drawing(){
  int limit = MAX_POINTS;
  for(int idx = 0; idx < limit; idx++){
    Serial.print(drawing[idx].x);
    Serial.print(",");
    Serial.print(drawing[idx].y);
    Serial.print("||");
  }
  Serial.println();
}

void display_current_drawing(){
  tft.fillScreen(BACKGND_COLOR);
  int limit = MAX_POINTS;
  for(int idx = 0; idx < limit; idx++){
    if(drawing[idx].x == -1) return;
    tft.fillCircle(drawing[idx].x,drawing[idx].y,RADIUS,BALL_COLOR); //draw new ball location
  }
}

// *** FORMATS POINTS IN YOUR DRAWING TO: 0,4&15,15&14,11 *** --- working!
void format_points(char* output){
  strcpy(output, ""); // clear output
  char next_point[9]; //at max, this holds 999,999&\0
  
  for(int idx = 0; idx < MAX_POINTS; idx++){
    struct Point curr = drawing[idx];
    if(curr.x == -1 or curr.y == -1) break; // IF WE REACH A {-1, -1} POINT, BREAK
    
    sprintf(next_point, "%d,%d&", curr.x, curr.y);
    strcat(output, next_point);
  }
  output[strlen(output)-1] = '\0';   // Make the last char into \0 to get rid of that extra & at the end
}

// *** PARSES THE RESPONSE FROM THE GET REQUEST THAT IS IN THE FORMAT drawing~0,4&15,15&14,11 ... ! *** --- working!
void write_to_drawing(char *response){
  null_fill_drawing(); // to clear our drawing :(
  drawing_idx = 0; // have index start at 0 for our drawing_idx

  response += 8*sizeof(response[0]); //SHIFTS drawing pointer to skip the "drawing~" part
  
  char *ptr1 = strtok(response, ",");
  int idx = 0;

  while (ptr1 != NULL){
    char x[4];
    char y[4];
    int curr_idx = 0;

    strcpy(x, ptr1);
    ptr1 = strtok(NULL, "&");
    strcpy(y, ptr1);
    ptr1 = strtok(NULL, ",");
    
    drawing[idx].x = atoi(x);
    drawing[idx].y = atoi(y);
    idx++;
  }
}
 
