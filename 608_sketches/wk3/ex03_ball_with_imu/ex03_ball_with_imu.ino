#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> //Used in support of TFT Display
#include <string.h>  //used for some string handling and processing.
#include <mpu6050_esp32.h>

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

#define BACKGROUND TFT_GREEN
#define BALL_COLOR TFT_BLUE

const int DT = 40; //milliseconds
const int SCALER = 1000; //how much force to apply to ball
const uint8_t BUTTON = 5; 

struct Vec { //C struct to represent 2D vector (position, accel, vel, etc)
  float x;
  float y;
};

uint32_t primary_timer; //main loop timer

//state variables:
struct Vec pos; //position of ball
struct Vec vel; //velocity of ball
struct Vec accel; //acceleration of ball

//physics constants:
const float MASS = 1; //for starters
const int RADIUS = 5; //radius of ball
const float K_FRICTION = 0.15;  //friction coefficient
const float K_SPRING = 0.9;  //spring coefficient

//boundary constants:
const int LEFT_LIMIT = RADIUS; //left side of screen limit
const int RIGHT_LIMIT = 127 - RADIUS; //right side of screen limit
const int TOP_LIMIT = RADIUS; //top of screen limit
const int BOTTOM_LIMIT = 159 - RADIUS; //bottom of screen limit

bool pushed_last_time; //for finding change of button (using bool type...same as uint8_t)


MPU6050 imu; //imu object called, appropriately, imu

void step(float x_force=0, float y_force=0){
  //update acceleration (from f=ma)
  accel.x = (x_force - K_FRICTION*vel.x)/MASS;
  accel.y = (y_force - K_FRICTION*vel.y)/MASS;
  //integrate to get velocity from current acceleration
  vel.x = vel.x + 0.001*DT*accel.x; //integrate, 0.001 is conversion from milliseconds to seconds
  vel.y = vel.y + 0.001*DT*accel.y; //integrate!!
  //
  moveBall(); //you'll write this from scratch!
}

void moveBall(){
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
}

void setup() {
  tft.init();
  tft.setRotation(2);
  tft.setTextSize(1);
  tft.fillScreen(BACKGROUND);
  delay(100);
  Serial.begin(115200); //for debugging if needed.
  pinMode(BUTTON, INPUT_PULLUP);
  if (imu.setupIMU(1)) {
    Serial.println("IMU Connected!");
  } else {
    Serial.println("IMU Not Connected :/");
    Serial.println("Restarting");
    ESP.restart(); // restart the ESP (proper way)
  }
  randomSeed(analogRead(0));  //initialize random numbers
  step(random(-SCALER, SCALER), random(-SCALER, SCALER)); //apply initial force to lower right
  pushed_last_time = false;
  primary_timer = millis();
}

void loop() {
  //draw circle in previous location of ball in color background (redraws minimal num of pixels, therefore is quick!)
  tft.fillCircle(pos.x, pos.y, RADIUS, BACKGROUND);
  imu.readAccelData(imu.accelCount);//read imu
  float y = -imu.accelCount[0] * imu.aRes;      // **** X ANY Y NEEDED TO BE SWAPPED AND NEGATED
  float x = -imu.accelCount[1] * imu.aRes;

  step(x * SCALER, y * SCALER); //apply force from imu
  tft.fillCircle(pos.x, pos.y, RADIUS, BALL_COLOR); //draw new ball location

  while (millis() - primary_timer < DT); //wait for primary timer to increment
  primary_timer = millis();
}
