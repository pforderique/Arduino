#include "Arduino.h"
#include "Ball.h"

Ball::Ball(TFT_eSPI* tft_to_use, int dt, int rad = 4, float ms = 1,
     int ball_color = TFT_WHITE, int background_color = BACKGROUND,
     int left_lim = 0, int right_lim = 127, int top_lim = 0, int bottom_lim = 159) {
  pos.x = 64; //x position
  pos.y = 80; //y position
  vel.x = 0; //x velocity
  vel.y = 0; //y velocity
  accel.x = 0; //x acceleration
  accel.y = 0; //y acceleration
  local_tft = tft_to_use; //our TFT
  BALL_CLR = ball_color; //ball color
  BKGND_CLR = background_color;
  MASS = ms; //for starters
  RADIUS = rad; //radius of ball
  K_FRICTION = 0.15;  //friction coefficient
  K_SPRING = 0.9;  //spring coefficient
  LEFT_LIMIT = left_lim + RADIUS; //left side of screen limit
  RIGHT_LIMIT = right_lim - RADIUS; //right side of screen limit
  TOP_LIMIT = top_lim + RADIUS; //top of screen limit
  BOTTOM_LIMIT = bottom_lim - RADIUS; //bottom of screen limit
  DT = dt; //timing for integration
}

void Ball::step(float x_force = 0, float y_force = 0 ) {
  //update acceleration (from f=ma)
  accel.x = (x_force - K_FRICTION*vel.x)/MASS;
  accel.y = (y_force - K_FRICTION*vel.y)/MASS;
  //integrate to get velocity from current acceleration
  vel.x = vel.x + 0.001*DT*accel.x; //integrate, 0.001 is conversion from milliseconds to seconds
  vel.y = vel.y + 0.001*DT*accel.y; //integrate!!
  
  local_tft->drawCircle(pos.x, pos.y, RADIUS, BKGND_CLR);
  moveBall();
  local_tft->drawCircle(pos.x, pos.y, RADIUS, BALL_CLR);
}
void Ball::reset(int x = 64, int y = 32) {
  pos.x = x;
  pos.y = y;
  vel.x = 0;
  vel.y = 0;
  accel.x = 0;
  accel.y = 0;
}

int Ball::getX() {
  return pos.x;
}
int Ball::getY() {
  return pos.y;
}

void Ball::moveBall(){
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
