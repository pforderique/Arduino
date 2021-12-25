#include<string.h>

void setup() {
  Serial.begin(115200);
  Serial.println("testing");
}

void loop() {
  //test case (taken from page or write your own!)
  Serial.println("Starting Test Case:");
  char in1[] = "Hi There";
  char in2[] = "Cats";
  char storage[300];
  memset(storage,0,sizeof(storage)); //fill array with all nulls
  interleaver(in1,in2,storage);
  Serial.println(storage);
  delay(500);
}

void interleaver(char* input_1, char* input_2, char* output) {
  // start with two pointers - one for each string
  uint8_t idx_main = 0;
  uint8_t idx1 = 0;
  uint8_t idx2 = 0;
  
  // loop IFF both indexes not out of length
  while(idx1 < strlen(input_1) && idx2 < strlen(input_2)){
    // add that character from each string
    output[idx_main] = input_1[idx1];
    idx_main += 1;
    output[idx_main] =input_2[idx2];
    idx_main += 1;
    idx1 += 1;
    idx2 += 1;
  }
  // at the end add any remainging characters
  for(uint8_t i = idx1; i < strlen(input_1); i++){
    output[idx_main] = input_1[i];
    idx_main += 1;
  }
  for(uint8_t i = idx2; i < strlen(input_2); i++){
    output[idx_main] = input_2[i];
    idx_main += 1;
  }
}
