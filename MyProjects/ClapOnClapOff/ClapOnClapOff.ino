#include <SPI.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

/* PINS */
const uint8_t LIGHT_PIN = 27; // Light pin (LED or lamp)
const uint8_t PIN_1 = 5; //button 1
const int MIC_PIN = A0;

/* VARS */
uint16_t value;
uint16_t old_val;
uint16_t dulled_value;
uint16_t MAX_SEEN = 0;
uint32_t mic_timer = 0;

/* CONSTS */
const uint16_t MIC_HIGH_THLD = 2700;
const uint16_t MIC_LOW_THLD = 2000;
const uint16_t CLAP_DELAY_LOW = 250; // INTERVAL -- you have between 600ms and 1200ms to clap again
const uint16_t CLAP_DELAY_HIGH = 1000;

/* STATES */
const uint8_t IDLE = 0;
const uint8_t H1 = 1;
const uint8_t L1 = 2;
const uint8_t H2 = 3;
uint8_t STATE = IDLE;
int light_state = LOW;

void setup() {
  Serial.begin(115200); // Set up serial port
  pinMode(LIGHT_PIN, OUTPUT);
  Serial.println("Program Started.");
}

void loop() {
  value = analogRead(MIC_PIN);
  dulled_value = (uint16_t)(old_val + value)/2.0;

  mic_fsm(dulled_value);
    
  old_val = value;
}

void mic_fsm(uint16_t value){
  switch(STATE){
    case IDLE:
      if(value > MIC_HIGH_THLD){
        Serial.println("H1 Case entered");
        STATE = H1;
      }
      break;

    case H1:
      if(value < MIC_LOW_THLD){
        Serial.println("L1 Case entered");
        STATE = L1;
        mic_timer = millis();
      }
      break;

    case L1:
      // check if we clap again within the interval range
      if(CLAP_DELAY_LOW <= millis() - mic_timer && millis() - mic_timer <= CLAP_DELAY_HIGH){
        if(value > MIC_HIGH_THLD) {
          Serial.println("H2 Case entered");
          mic_timer = 0;
          STATE = H2;
          activate(); // activate components
        }
      }
      
      // if over time, then return to IDLE
      else if (millis() - mic_timer > CLAP_DELAY_HIGH) {
        Serial.println("Returned to IDLE");
        STATE = IDLE;
      }
      
      break;

    case H2:
      if(value < MIC_LOW_THLD){
        Serial.println("IDLE Case entered");
        STATE = IDLE;
      }
      break;
  }
}

// activate any components here. In this project, switch state of light
void activate(){
  if(light_state == LOW) light_state = HIGH;
  else light_state = LOW;
  Serial.print("--- LIGHT SWITCHED --- to ");
  Serial.println(light_state); 
  digitalWrite(LIGHT_PIN, light_state);
}
