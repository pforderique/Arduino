#include<string.h>

int values[10];
void setup() {
  Serial.begin(115200);
}

void loop() {
  char message1[] = "";
  int_extractor(message1,values,'&');
  for (int i = 0; i<sizeof(values)/sizeof(int); i++){
    Serial.println(values[i]);
  }
  delay(1000);
}
void int_extractor(char* data_array, int* output_values, char delimiter){    
    // convert delimter to character array for strtok input
    char delim[2];
    delim[0] = delimiter;
    delim[1] = '\0';
    
    char *ptr = strtok(data_array, delim);
    int idx = 0;
    
    while (ptr != NULL){
      output_values[idx] = atoi(ptr);
      ptr = strtok(NULL, delim);
      idx ++;
    }
}
