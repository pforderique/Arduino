#include <SPI.h>
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();
// Set up the TFT object
 
const int LOOP_SPEED = 25;
 
unsigned long primary_timer; //main loop timer
 
#define BACKGROUND TFT_BLACK
#define BALL_COLOR TFT_WHITE
 
const uint8_t BUTTON = 5; 

struct Vec { //C struct to represent 2D vector (position, accel, vel, etc)
  float x;
  float y;
};

 
class Ball{ 
  Vec pos; //position
  Vec vel; //velocity
  Vec accel; //accel
  int BALL_CLR;
  int BKGND_CLR;
  float MASS; //for starters
  int RADIUS; //radius of ball
  float K_FRICTION;  //friction coefficient
  float K_SPRING;  //spring coefficient
  int LEFT_LIMIT; //left side of screen limit
  int RIGHT_LIMIT; //right side of screen limit
  int TOP_LIMIT; //top of screen limit
  int BOTTOM_LIMIT; //bottom of screen limit
  int DT; //timing for integration
  public:
    Ball(int rad=4, float ms=1, int ball_color=TFT_WHITE, int background_color=BACKGROUND){
      pos.x = 64; //x position
      pos.y = 80; //y position
      vel.x = 0; //x velocity
      vel.y = 0; //y velocity
      accel.x = 0; //x acceleration
      accel.y = 0; //y acceleration
      BALL_CLR = ball_color; //ball color
      BKGND_CLR = background_color;
      MASS = ms; //for starters
      RADIUS = rad; //radius of ball
      K_FRICTION = 0.05;  //friction coefficient
      K_SPRING = 0.9;  //spring coefficient
      LEFT_LIMIT =RADIUS; //left side of screen limit
      RIGHT_LIMIT = 127-RADIUS; //right side of screen limit
      TOP_LIMIT = RADIUS; //top of screen limit
      BOTTOM_LIMIT = 159-RADIUS; //bottom of screen limit
      DT = LOOP_SPEED; //timing for integration
    }
    void step(float x_force=0, float y_force=0 ){
      tft.drawCircle(pos.x, pos.y, RADIUS,BKGND_CLR);
      //update acceleration (from f=ma)
      accel.x = (x_force - K_FRICTION*vel.x)/MASS;
      accel.y = (y_force - K_FRICTION*vel.y)/MASS;
      //integrate to get velocity from current acceleration
      vel.x = vel.x + 0.001*DT*accel.x; //integrate, 0.001 is conversion from milliseconds to seconds
      vel.y = vel.y + 0.001*DT*accel.y; //integrate!!
      //
      moveBall(); //you'll write this from scratch!
      tft.drawCircle(pos.x, pos.y, RADIUS,BALL_CLR);
    }
    void reset(int x=64, int y=32){
      pos.x = x;
      pos.y = y;
      vel.x = 0;
      vel.y = 0;
      accel.x = 0;
      accel.y = 0;
    }
  private:
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
};
 
Ball lots[20];
void setup() {
  Serial.begin(115200); //for debugging if needed.
  tft.init();
  tft.setRotation(2);
  tft.setTextSize(1);
  tft.fillScreen(BACKGROUND);
  randomSeed(analogRead(0));  //initialize random numbers
  for (int i=0; i<20; i++) {
    lots[i].reset();
  }
  delay(20); //wait 20 milliseconds
  for (int i=0; i<20; i++) {

    lots[i].step(random(-30000,30000),random(-30000,30000));
  }
  primary_timer = millis();
}
 
void loop() {
  for (int i=0; i<20; i++){
    lots[i].step();
  }
  while (millis()-primary_timer<LOOP_SPEED); //wait for primary timer to increment
  primary_timer = millis();
}
