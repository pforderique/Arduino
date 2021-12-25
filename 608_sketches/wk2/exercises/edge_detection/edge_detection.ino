const int PIN_1 = 16;
const int PIN_2 = 5;
int state_1;
int state_2;
const int LOOP_PERIOD = 30;
unsigned long timer;

void setup(){
  Serial.begin(115200);
  pinMode(PIN_1,INPUT_PULLUP);
  pinMode(PIN_2,INPUT_PULLUP);
  timer=millis();
  state_1 = 1; //initialize
  state_2 = 1; //initialize
}

void loop(){
  int input_1 = digitalRead(PIN_1);
  int input_2 = digitalRead(PIN_2);
  int output_1;
  int output_2;
  edge_detect(input_1,&state_1,&output_1);
  edge_detect(input_2,&state_2,&output_2);
  char print_buffer[300]={0};
  sprintf(print_buffer,"%d %d %d %d", output_1, output_2, state_1, state_2);
  Serial.println(print_buffer);
  //state logic below
  while (millis()-timer<LOOP_PERIOD);
  timer = millis();
}
 void edge_detect(int input, int* button_state, int* output) {

  switch(*button_state){
    case 1: // UP
      // if we get pressed
      if(input == 0 && *output != 1){
        *button_state = 0;
        *output = 1;
        break;
      }
      *output = 0;
      break;
      
    case 0: // PRESSED
      // if we get released after being pressed
      if(input == 1 && *output != -1 ){
        *button_state = 1;
        *output = -1;
        break;
      }
      *output = 0;
      break;
  }
}
