#include <SPI.h>
#include <TFT_eSPI.h>
#include <mpu6050_esp32.h>
#include "Ball.h"

MPU6050 imu; //imu object called, appropriately, imu
TFT_eSPI tft = TFT_eSPI();
// Set up the TFT object

const int LOOP_PERIOD = 40;
const int EXCITEMENT = 10000; //how much force to apply to ball
unsigned long primary_timer; //main loop timer

#define BACKGROUND TFT_BLACK
#define BALL_COLOR TFT_WHITE

const uint8_t BUTTON_PIN = 5; //CHANGE YOUR WIRING TO PIN 16!!! (FROM 19)

struct Vec { //C struct to represent 2D vector (position, accel, vel, etc)
  float x;
  float y;
};

class Game {
    int score;
    Vec food_pos;
    int food_half_width; // Like "radius", except it's a square
    int left_limit = 0; //left side of screen limit
    int right_limit = 127; //right side of screen limit
    int top_limit = 0; //top of screen limit
    int bottom_limit = 159; //bottom of screen limit
    int player_radius = 4;
    TFT_eSPI* game_tft; //object point to TFT (screen)
  public:
    Ball player; //make instance of "you"
    Game(TFT_eSPI* tft_to_use, int loop_speed):
      player(tft_to_use, loop_speed, player_radius, 1, TFT_RED, BACKGROUND,
             left_limit, right_limit, top_limit, bottom_limit) {
      game_tft = tft_to_use;
      score = 0;
      food_pos.x = 40; // Initial x pos
      food_pos.y = 40; // Initial y pos
      food_half_width = 1;
    }
    void step(float x, float y) {
      player.step(x, y);
      int new_score = collisionDetect(); //checks if collision occurred (food found/lost)
      if (new_score != score) {//got some nomnoms!  (score can only increase currently so change in score means food found)
        score = new_score;
        //erase old food position
        game_tft->fillRect(food_pos.x - food_half_width, food_pos.y - food_half_width, 2 * food_half_width, 2 * food_half_width, BACKGROUND);
        food_pos.x = random(right_limit - left_limit - 2 * food_half_width)
                     + left_limit + food_half_width;
        food_pos.y = random(bottom_limit - top_limit - 2 * food_half_width)
                     + top_limit + food_half_width;
      }
      int top_left_x = food_pos.x - food_half_width;
      int top_left_y = food_pos.y - food_half_width;
      int side = 2 * food_half_width;
      //draw new food position
      game_tft->fillRect(top_left_x, top_left_y, side, side, TFT_GREEN);
      game_tft->setCursor(0, 0, 1);
      game_tft->print("Score: ");
      game_tft->println(score);
    }
  private:
    int collisionDetect() {
      if(abs(player.getX() - food_pos.x) < (player_radius + food_half_width) &&
         abs(player.getY() - food_pos.y) < (player_radius + food_half_width) ){
        return score + 1;
      }
      return score;
    }
};

//make instance of Game class. Call it game. Hand it reference to TFT (the LCD):
//also hand it the LOOP_PERIOD...these will (get handed to ball instance hidden inside game)
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
