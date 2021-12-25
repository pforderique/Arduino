uint32_t time_counter;
const uint32_t DT = 1000; //sample period
const uint32_t FILTER_ORDER = 13; //size of filter
float values[50]; //used for remembering an arbitrary number of old previous values

void setup(){
  Serial.begin(115200);
  // Initialize vs to all zeros (to be safe)
  // Could also do to non-zero values if desired!
  for (int i=0; i<50; i++){ 
    values[i] = 0.0;
  }
  time_counter = millis();
  
}

void loop(){
//  float input = 1;// Serial.digitalead(5); //get some input value from a function that returns measurements (maybe a button for example)
//  // Process the new input, and get a new output
//  // In this case, we use a FILTER_ORDER moving average, but it can be anything
//  float average = averaging_filter(input, values, FILTER_ORDER);
////  Serial.println(average);
  float inputs[] = {248.000000, 32.760000, -478.400000, 155.100000, 0.024000};
  int ORDER = 6;

  for(int i=0; i < sizeof(inputs)/sizeof(inputs[0]); i++){
    float ans = averaging_filter(inputs[i], values, ORDER);
  }
  while(millis()-time_counter < DT);
  time_counter = millis();
  Serial.println("-----------------");
  Serial.println();
}
//  int SIZE = sizeof(stored_values)/sizeof(stored_values[0]);

float averaging_filter(float input, float* stored_values, int order) {
  // shift array values LEFT
//  int SIZE = sizeof(stored_values)/sizeof(stored_values[0]);
  for(int i=order; i>=1; i--){
    stored_values[i] = stored_values[i-1];
  }
  stored_values[0] = input;

  printArray(stored_values);
  
//  // get sum value
//  float sum = 0;
//  for(int i=0; i <= order; i++){
//    sum += stored_values[i];
//  }
//  sum *= (1.0/(1 + order));
//  return sum;
}

void printArray(float* arr){
  for(int i =0; i <sizeof(arr)/sizeof(arr[0]); i++){
    Serial.println(arr[i]);
  }
}
