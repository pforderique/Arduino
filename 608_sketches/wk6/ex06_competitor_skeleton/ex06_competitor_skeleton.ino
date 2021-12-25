#include <SPI.h>
#include <TFT_eSPI.h>
#include <mpu6050_esp32.h>

MPU6050 imu; //imu object called, appropriately, imu
TFT_eSPI tft = TFT_eSPI();
// Set up the TFT object

const int LOOP_PERIOD = 40;
const int EXCITEMENT = 1000; //how much force to apply to ball
unsigned long primary_timer; //main loop timer

#define BACKGROUND TFT_BLACK
#define BALL_COLOR TFT_WHITE

const uint8_t BUTTON_PIN = 16; //CHANGE YOUR WIRING TO PIN 16!!! (FROM 19)

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
    void step(float x_force = 0, float y_force = 0 ) {
      //YOUR CODE HERE (PREV EXERCISE)
      local_tft->drawCircle(pos.x, pos.y, RADIUS, BKGND_CLR);
      moveBall();
      local_tft->drawCircle(pos.x, pos.y, RADIUS, BALL_CLR);
    }
    void reset(int x = 64, int y = 32) {
      pos.x = x;
      pos.y = y;
      vel.x = 0;
      vel.y = 0;
      accel.x = 0;
      accel.y = 0;
    }
    int getX() {
      return pos.x;
    }
    int getY() {
      return pos.y;
    }
  private:
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
    }
};


class Game {
    int score;
    Vec food_pos;  //food position
    int food_half_width; // Like "radius", except it's a square
    int left_limit = 0; //left side of screen limit
    int right_limit = 127; //right side of screen limit
    int top_limit = 0; //top of screen limit
    int bottom_limit = 159; //bottom of screen limit
    int player_radius = 4;
    int difficulty = 100; //decent difficulty
    TFT_eSPI* game_tft; //object point to TFT (screen)
  public:
    Ball player; //make instance of "you"
    Ball opponent;
    Game(TFT_eSPI* tft_to_use, int loop_speed):
      player(tft_to_use, loop_speed, player_radius, 1, TFT_RED, BACKGROUND,
             left_limit, right_limit, top_limit, bottom_limit),
      opponent(tft_to_use, loop_speed, player_radius, 1, TFT_GREEN, BACKGROUND,
               left_limit, right_limit, top_limit, bottom_limit)
    {
      game_tft = tft_to_use;
      score = 0;
      food_pos.x = 40; // Initial x pos
      food_pos.y = 40; // Initial y pos change if you like
      food_half_width = 1;
    }
    void step(float x, float y) {
      player.step(x, y);
      float opp_x_command;
      float opp_y_command;
      ai(&opp_x_command, &opp_y_command); //get x and y command of opponent...based on AI
      opponent.step(difficulty * opp_x_command, difficulty * opp_y_command);
      int new_score = collisionDetect(); //checks if collision occurred (food found/lost)
      if (new_score != score) {
        score = new_score;
        game_tft->fillRect(food_pos.x - food_half_width, food_pos.y - food_half_width, 2 * food_half_width, 2 * food_half_width, BACKGROUND);
        food_pos.x = random(right_limit - left_limit - 2 * food_half_width)
                     + left_limit + food_half_width;
        food_pos.y = random(bottom_limit - top_limit - 2 * food_half_width)
                     + top_limit + food_half_width;
      }
      int top_left_x = food_pos.x - food_half_width;
      int top_left_y = food_pos.y - food_half_width;
      int side = 2 * food_half_width;
      game_tft->fillRect(top_left_x, top_left_y, side, side, TFT_GREEN);
      game_tft->setCursor(0, 0, 1);
      char output[30];
      sprintf(output, "Score: %d    ", score);
      game_tft->print(output);

    }
  private:
    int collisionDetect() {
      if(abs(player.getX() - food_pos.x) < (player_radius + food_half_width) &&
         abs(player.getY() - food_pos.y) < (player_radius + food_half_width) ){
        return score + 1;
      }
      if(abs(opponent.getX() - food_pos.x) < (player_radius + food_half_width) &&
         abs(opponent.getY() - food_pos.y) < (player_radius + food_half_width) ){
        return score - 1;
      }
      return score;

    }
    void ai(float* x, float* y) {
      //YOUR CODE HERE...(NEW "AI")
      float x_dist = food_pos.x - opponent.getX();
      float y_dist = food_pos.y - opponent.getY();
      float magnitude = sqrt(pow(x_dist, 2) + pow(y_dist, 2));
      *x = x_dist/magnitude;
      *y = y_dist/magnitude;
    }
};


Game game(&tft, LOOP_PERIOD);

void setup() {
  Serial.begin(115200); //for debugging if needed.
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
  tft.fillScreen(BACKGROUND);
  tft.setTextColor(TFT_WHITE, TFT_BLACK); //set color of font to green foreground, black background

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  randomSeed(analogRead(0));  //initialize random numbers
  delay(20); //wait 20 milliseconds
  primary_timer = millis();
}

void loop() {
  imu.readAccelData(imu.accelCount);//read imu
  float x = imu.accelCount[0] * imu.aRes;
  float y = imu.accelCount[1] * imu.aRes;
  game.step(-y * EXCITEMENT, -x * EXCITEMENT); //apply force from imu
  while (millis() - primary_timer < LOOP_PERIOD); //wait for primary timer to increment
  primary_timer = millis();
}
