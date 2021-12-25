#include <mpu6050_esp32.h>
#include<math.h>
#include<string.h>
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h


uint8_t state; //state variable
const uint8_t IDLE = 0;
const uint8_t PEAK = 1;

int steps; //counting steps
float THRESHOLD = 12;
uint32_t step_timer = 0;
uint8_t TOO_FAST = 200;

float old_acc_mag, older_acc_mag; //maybe use for remembering older values of acceleration magnitude
float acc_mag = 0;  //used for holding the magnitude of acceleration
float avg_acc_mag = 0; //used for holding the running average of acceleration magnitude

const float ZOOM = 9.81; //for zooming plot (converts readings into m/s^2)...used for visualizing only

const uint8_t LOOP_PERIOD = 10; //milliseconds
uint32_t primary_timer = 0;
float x, y, z; //variables for grabbing x,y,and z values

MPU6050 imu; //imu object called, appropriately, imu

void setup() {
  Serial.begin(115200); 
  delay(50); //pause to make sure comms get set up
  Wire.begin();
  delay(50); //pause to make sure comms get set up
  if (imu.setupIMU(1)) {
    Serial.println("IMU Connected!");
  } else {
    Serial.println("IMU Not Connected :/");
    Serial.println("Restarting");
    ESP.restart(); // restart the ESP (proper way)
  }
  tft.init(); //initialize the screen
  tft.setRotation(2); //set rotation for our layout
  primary_timer = millis();
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  state = IDLE;
  steps = 0; //initialize steps to zero!
  step_timer = millis();
}

void loop() {
  imu.readAccelData(imu.accelCount);
  x = imu.accelCount[0] * imu.aRes;
  y = imu.accelCount[1] * imu.aRes;
  z = imu.accelCount[2] * imu.aRes;

  // **** MY CODE *****
  acc_mag = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
  avg_acc_mag = (acc_mag + old_acc_mag + older_acc_mag)/3; // USE THIS!!
  update_step();

  older_acc_mag = old_acc_mag;
  old_acc_mag = acc_mag; 
  
  //Serial printing:
  char output[80];
  sprintf(output, "%4.2f,%4.2f", ZOOM * acc_mag, ZOOM * avg_acc_mag); //render numbers with %4.2 float formatting
  Serial.println(output); //print to serial for plotting
  
  //redraw for use on LCD (with new lines):
  //TOTALLY CHANGE WHAT YOU PRINT HERE. %d will place integer into string
  sprintf(output, "%d", steps); 
  tft.setCursor(0, 0, 4);
  tft.println(output);
  
  
  while (millis() - primary_timer < LOOP_PERIOD); //wait for primary timer to increment
  primary_timer = millis();
}

void update_step(){
  switch(state){
    case IDLE:
      if(ZOOM*avg_acc_mag >= THRESHOLD){
        state = PEAK;
        break;
      }
      break;
      
    case PEAK:
      if(millis() - step_timer > TOO_FAST){
        step_timer = millis();
        if(ZOOM*avg_acc_mag < THRESHOLD){
          state = IDLE;
          steps++;
          break;
        }
      }
      break;
  }
}
