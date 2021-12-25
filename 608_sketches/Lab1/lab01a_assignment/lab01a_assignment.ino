#include<string.h>
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h
const int BUTTON = 5; //connect to pin 5

uint8_t draw_state;  //used for remembering state
uint8_t previous_value;  //used for remembering previous button
 
void setup(){
    Serial.begin(115200); //initialize serial!
    tft.init(); //initialize the screen
    tft.setRotation(2); //set rotation for our layout

    tft.setTextSize(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    
    pinMode(BUTTON,INPUT_PULLUP); //sets IO pin 5 as an input which defaults to a 3.3V signal when unconnected and 0V when the switch is pushed
    draw_state = 0; //initialize to 0
    previous_value = 1; //initialize to 1

//    tft.fillScreen(TFT_BLACK); 
//    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
//    tft.drawString("Hello World!",0,0,1);
}
 
void loop(){
  uint8_t value = digitalRead(BUTTON); //get reading
  if(value != previous_value){
    if(value == 0){
      draw_state += 1;
      draw_state %= 2;
    }
    
    if (draw_state==0){
      tft.fillScreen(TFT_BLACK); 
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.drawString("State 2",0,0,1);
      tft.drawCircle(5,20,3,TFT_GREEN);
      tft.fillCircle(64,80,30,TFT_PURPLE);
    }else if(draw_state==1){
      tft.fillScreen(TFT_YELLOW);
      tft.setTextColor(TFT_BLACK, TFT_BLACK);
      tft.drawString("State 1",0,0,1);
      tft.drawRect(10,10,5,30,TFT_BLUE);
      tft.fillRect(50,50,20,20,TFT_RED);
    }
  }
  previous_value = value; //remember for next time
}
