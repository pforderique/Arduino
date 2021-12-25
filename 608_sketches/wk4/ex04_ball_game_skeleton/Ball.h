#ifndef Ball_h
#define Ball_h
#include "Arduino.h"

struct Vec { //C struct to represent 2D vector (position, accel, vel, etc)
  float x;
  float y;
};

class Ball {
    Vec pos;
    Vec vel;
    Vec accel;
    TFT_eSPI* local_tft;
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
    Ball(TFT_eSPI* tft_to_use, int dt, int rad = 4, float ms = 1,
         int ball_color = TFT_WHITE, int background_color = BACKGROUND,
         int left_lim = 0, int right_lim = 127, int top_lim = 0, int bottom_lim = 159);
    void step(float x_force = 0, float y_force = 0 );
    void reset(int x = 64, int y = 32); 
    int getX();
    int getY();
  private:
    void moveBall();
};
