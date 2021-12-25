#include <SPI.h>
#include <TFT_eSPI.h>
#include <WiFiClientSecure.h>
//WiFiClientSecure is a big library. It can take a bit of time to do that first compile

struct Riff {
  double notes[64]; //the notes (array of doubles containing frequencies in Hz. I used https://pages.mtu.edu/~suits/notefreqs.html
  int length; //number of notes (essentially length of array.
  float note_period; //the timing of each note in milliseconds (take bpm, scale appropriately for note (sixteenth note would be 4 since four per quarter note) then
};

//Kendrick Lamar's HUMBLE
Riff humble = {{
    311.13, 311.13, 0 , 0, 311.13, 311.13, 0, 0,  //beat 1
    0, 0, 0, 0, 329.63, 329.63, 0, 0,             //beat 2
    311.13, 311.13, 0, 0, 207.65, 0, 207.65, 0,   //beat 3
    0, 0, 207.65, 207.65, 329.63, 329.63, 0, 0    //beat 4
  }, 32, 100.15 //32 32nd notes in measure, 108.15 ms per 32nd note
};

//Beyonce aka Sasha Fierce's song Formation off of Lemonade. Don't have the echo effect
Riff formation = {{
    261.63, 0, 261.63 , 0,   0, 0, 0, 0, 261.63, 0, 0, 0, 0, 0, 0, 0, //measure 1 Y'all haters corny with that illuminati messssss
    311.13, 0, 311.13 , 0,   0, 0, 0, 0, 311.13, 0, 0, 0, 0, 0, 0, 0 //measure 2 Paparazzi catch my fly and my cocky freshhhhhhh
  }, 32, 120.95 //32 32nd notes in measure, 108.15 ms per 32nd note
};

//Justin Bieber's Sorry:
Riff sorry = {{ 1046.50, 1244.51 , 1567.98, 0.0, 1567.98, 0.0, 1396.91, 1244.51, 1046.50, 0, 0, 0, 0, 0, 0, 0}, 16, 150};

//Twinkle
Riff twinkle = {{262, 0, 262, 0, 392, 0, 392, 0, 440, 0, 440, 0, 392, 392, 392, 392, 349, 0, 349, 0, 330, 0, 330, 0, 294, 0, 294, 0, 262, 262, 262, 262}, 32, 115.15};

// Ode 
Riff ode_to_joy ={{392, 0, 392, 0, 349, 0, 330, 0, 330, 0, 349, 0, 392, 0, 294, 0, 262, 0, 262, 0, 294, 0, 330, 0, 330, 0, 294, 0, 294, 294}, 32, 130.15};

// ho hey
Riff ophelia = {{
  293.66, 0, 293.66, 0, 261.63, 0, 293.66, 220.00, 220.00, 220.00, 220.00, 220.00,
  196.00, 220.00, 196.00, 174.61, 0, 146.83, 0, 146.83, 0, 174.61, 0, 220.00, 174.61,
}, 25, 200};

Riff song_to_play = humble;  //select one of the riff songs

/// SELECTIONS
//const int NUM_SONGS = 3;
//char song_strings[NUM_SONGS][30] = {"humble", "formation", "sorry"}; 
//Riff songs[] = {humble, formation, sorry};
//int riff_idx = 0;

const char* CA_CERT = \
                      "-----BEGIN CERTIFICATE-----\n" \
                      "MIIDujCCAqKgAwIBAgILBAAAAAABD4Ym5g0wDQYJKoZIhvcNAQEFBQAwTDEgMB4G\n" \
                      "A1UECxMXR2xvYmFsU2lnbiBSb290IENBIC0gUjIxEzARBgNVBAoTCkdsb2JhbFNp\n" \
                      "Z24xEzARBgNVBAMTCkdsb2JhbFNpZ24wHhcNMDYxMjE1MDgwMDAwWhcNMjExMjE1\n" \
                      "MDgwMDAwWjBMMSAwHgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMjETMBEG\n" \
                      "A1UEChMKR2xvYmFsU2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjCCASIwDQYJKoZI\n" \
                      "hvcNAQEBBQADggEPADCCAQoCggEBAKbPJA6+Lm8omUVCxKs+IVSbC9N/hHD6ErPL\n" \
                      "v4dfxn+G07IwXNb9rfF73OX4YJYJkhD10FPe+3t+c4isUoh7SqbKSaZeqKeMWhG8\n" \
                      "eoLrvozps6yWJQeXSpkqBy+0Hne/ig+1AnwblrjFuTosvNYSuetZfeLQBoZfXklq\n" \
                      "tTleiDTsvHgMCJiEbKjNS7SgfQx5TfC4LcshytVsW33hoCmEofnTlEnLJGKRILzd\n" \
                      "C9XZzPnqJworc5HGnRusyMvo4KD0L5CLTfuwNhv2GXqF4G3yYROIXJ/gkwpRl4pa\n" \
                      "zq+r1feqCapgvdzZX99yqWATXgAByUr6P6TqBwMhAo6CygPCm48CAwEAAaOBnDCB\n" \
                      "mTAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUm+IH\n" \
                      "V2ccHsBqBt5ZtJot39wZhi4wNgYDVR0fBC8wLTAroCmgJ4YlaHR0cDovL2NybC5n\n" \
                      "bG9iYWxzaWduLm5ldC9yb290LXIyLmNybDAfBgNVHSMEGDAWgBSb4gdXZxwewGoG\n" \
                      "3lm0mi3f3BmGLjANBgkqhkiG9w0BAQUFAAOCAQEAmYFThxxol4aR7OBKuEQLq4Gs\n" \
                      "J0/WwbgcQ3izDJr86iw8bmEbTUsp9Z8FHSbBuOmDAGJFtqkIk7mpM0sYmsL4h4hO\n" \
                      "291xNBrBVNpGP+DTKqttVCL1OmLNIG+6KYnX3ZHu01yiPqFbQfXf5WRDLenVOavS\n" \
                      "ot+3i9DAgBkcRcAtjOj4LaR0VknFBbVPFd5uRHg5h6h+u/N5GJG79G+dwfCMNYxd\n" \
                      "AfvDbbnvRG15RjF+Cv6pgsH/76tuIMRQyV+dTZsXjAzlAcmgQWpzU/qlULRuJQ/7\n" \
                      "TBj0/VLZjmmx6BEP3ojY+x1J96relc8geMJgEtslQIxq/H5COEBkEveegeGTLg==\n" \
                      "-----END CERTIFICATE-----\n";

TFT_eSPI tft = TFT_eSPI();

const int DELAY = 1000;
const int SAMPLE_FREQ = 8000;                          // Hz, telephone sample rate
const int SAMPLE_DURATION = 3;                        // duration of fixed sampling (seconds)
const int NUM_SAMPLES = SAMPLE_FREQ * SAMPLE_DURATION;  // number of of samples
const int ENC_LEN = (NUM_SAMPLES + 2 - ((NUM_SAMPLES + 2) % 3)) / 3 * 4;  // Encoded length of clip

const uint16_t RESPONSE_TIMEOUT = 6000;
const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
char response[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP request

/* CONSTANTS */
//Prefix to POST request:
const char PREFIX[] = "{\"config\":{\"encoding\":\"MULAW\",\"sampleRateHertz\":8000,\"languageCode\": \"en-US\", \"speechContexts\": [{ \"phrases\": [\"Hey Google\",\"play\", \"Ode to Joy\", \"Ophelia\"] }] }, \"audio\": {\"content\":\"";
const char SUFFIX[] = "\"}}"; //suffix to POST request
const int AUDIO_IN = A0; //pin where microphone is connected
const char API_KEY[] = "AIzaSyCwyynsePu7xijUYTOgR7NdVqxH2FAG9DQ"; //don't change this

// ************ NOTES *********
//PWM Channels. The LCD will still be controlled by channel 0, we'll use channel 1 for audio generation
uint8_t LCD_PWM = 0;
uint8_t AUDIO_PWM = 1;
uint8_t AUDIO_TRANSDUCER = 26;

float new_note = 0;
float old_note = 0;

const uint8_t PIN_1 = 5; //button 1
const uint8_t PIN_2 = 0; //button 2

/* Global variables*/
uint8_t button_state; //used for containing button state and detecting edges
int old_button_state; //used for detecting button edges
uint32_t time_since_sample;      // used for microsecond timing


char speech_data[ENC_LEN + 200] = {0}; //global used for collecting speech data
const char* NETWORK     = "PCP-0672";     // your network SSID (name of wifi network)
const char* PASSWORD = "Limaperuu1"; // your network password
const char*  SERVER = "speech.google.com";  // Server URL

uint8_t old_val;
uint32_t timer;

WiFiClientSecure client; //global WiFiClient Secure object

void setup() {
  Serial.begin(115200);               // Set up serial port
  tft.init();  //init screen
  tft.setRotation(2); //adjust rotation
  tft.setTextSize(1); //default font size
  tft.fillScreen(TFT_BLACK); //fill background
  tft.setTextColor(TFT_GREEN, TFT_BLACK); //set color of font to green foreground, black background
  delay(100); //wait a bit (100 ms)
  pinMode(PIN_1, INPUT_PULLUP);
  pinMode(PIN_2, INPUT_PULLUP);
  pinMode(25,OUTPUT); digitalWrite(25,0);//in case you're controlling your screen with pin 25

  // THE PINS FOR AUDIO
  pinMode(AUDIO_TRANSDUCER, OUTPUT);

  WiFi.begin(NETWORK, PASSWORD); //attempt to connect to wifi
  uint8_t count = 0; //count used for Wifi check times
  Serial.print("Attempting to connect to ");
  Serial.println(NETWORK);
  while (WiFi.status() != WL_CONNECTED && count < 12) {
    delay(500);
    Serial.print(".");
    count++;
  }
  delay(2000);
  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
    Serial.println("CONNECTED!");
    Serial.printf("%d:%d:%d:%d (%s) (%s)\n", WiFi.localIP()[3], WiFi.localIP()[2],
                  WiFi.localIP()[1], WiFi.localIP()[0],
                  WiFi.macAddress().c_str() , WiFi.SSID().c_str());
    delay(500);
  } else { //if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
  }
  timer = millis();
  client.setCACert(CA_CERT); //set cert for https
  old_val = digitalRead(PIN_1);
}

//main body of code
void loop() {
  button_state = digitalRead(PIN_1);
  if (!button_state && button_state != old_button_state) {
    Serial.println("listening...");
    record_audio();
    Serial.println("sending...");
    Serial.print("\nStarting connection to server...");
    delay(300);
    bool conn = false;
    for (int i = 0; i < 10; i++) {
      int val = (int)client.connect(SERVER, 443);
      Serial.print(i); Serial.print(": "); Serial.println(val);
      if (val != 0) {
        conn = true;
        break;
      }
      Serial.print(".");
      delay(300);
    }
    if (!conn) {
      Serial.println("Connection failed!");
      return;
    } else {
      Serial.println("Connected to server!");
      Serial.println(client.connected());
      int len = strlen(speech_data);
      // Make a HTTP request:
      client.print("POST /v1/speech:recognize?key="); client.print(API_KEY); client.print(" HTTP/1.1\r\n");
      client.print("Host: speech.googleapis.com\r\n");
      client.print("Content-Type: application/json\r\n");
      client.print("cache-control: no-cache\r\n");
      client.print("Content-Length: "); client.print(len);
      client.print("\r\n\r\n");
      int ind = 0;
      int jump_size = 1000;
      char temp_holder[jump_size + 10] = {0};
      Serial.println("sending data");
      while (ind < len) {
        delay(80);//experiment with this number!
        //if (ind + jump_size < len) client.print(speech_data.substring(ind, ind + jump_size));
        strncat(temp_holder, speech_data + ind, jump_size);
        client.print(temp_holder);
        ind += jump_size;
        memset(temp_holder, 0, sizeof(temp_holder));
      }
      client.print("\r\n");
      //Serial.print("\r\n\r\n");
      Serial.println("Through send...");
      unsigned long count = millis();
      while (client.connected()) {
        Serial.println("IN!");
        String line = client.readStringUntil('\n');
        Serial.print(line);
        if (line == "\r") { //got header of response
          Serial.println("headers received");
          break;
        }
        if (millis() - count > RESPONSE_TIMEOUT) break;
      }
      Serial.println("");
      Serial.println("Response...");
      count = millis();
      while (!client.available()) {
        delay(100);
        Serial.print(".");
        if (millis() - count > RESPONSE_TIMEOUT) break;
      }
      Serial.println();
      Serial.println("-----------");
      memset(response, 0, sizeof(response));
      while (client.available()) {
        char_append(response, client.read(), OUT_BUFFER_SIZE);
      }
      Serial.println(response);
      char* trans_id = strstr(response, "transcript");
      if (trans_id != NULL) {
        char* foll_coll = strstr(trans_id, ":");
        char* starto = foll_coll + 2; //starting index
        char* endo = strstr(starto + 1, "\""); //ending index
        int transcript_len = endo - starto + 1;
        char transcript[100] = {0};
        strncat(transcript, starto, transcript_len);
        Serial.println(transcript);

        // STRING STORED IN TRANSCRIPT
        parse_transcript(transcript);
      }
      Serial.println("-----------");
      client.stop();
      Serial.println("done");
    }
  }

  //set up AUDIO_PWM which we will control in this lab for music:
  ledcSetup(AUDIO_PWM, 0, 12);//12 bits of PWM precision
  ledcWrite(AUDIO_PWM, 0); //0 is a 0% duty cycle for the NFET
  ledcAttachPin(AUDIO_TRANSDUCER, AUDIO_PWM);
  
  old_button_state = button_state;
}

//function used to record audio at sample rate for a fixed nmber of samples
void record_audio() {     
    int sample_num = 0;    // counter for samples
    int enc_index = strlen(PREFIX) - 1;  // index counter for encoded samples
    float time_between_samples = 1000000 / SAMPLE_FREQ;
    int value = 0;
    char raw_samples[3];   // 8-bit raw sample data array
    memset(speech_data, 0, sizeof(speech_data));
    sprintf(speech_data, "%s", PREFIX);
    char holder[5] = {0};
    Serial.println("starting");
    uint32_t text_index = enc_index;
    uint32_t start = millis();
    time_since_sample = micros();
    while (sample_num < NUM_SAMPLES) { //read in NUM_SAMPLES worth of audio data
      button_state = digitalRead(PIN_1);
      if(button_state == 1) break;
      value = analogRead(AUDIO_IN);  //make measurement
      raw_samples[sample_num % 3] = mulaw_encode(value - 1551); //remove 1.25V offset (from 12 bit reading)
      sample_num++;
      if (sample_num % 3 == 0) {
        base64_encode(holder, raw_samples, 3);
        strncat(speech_data + text_index, holder, 4);
        text_index += 4;
      }
      // wait till next time to read
      while (micros() - time_since_sample <= time_between_samples); //wait...
      time_since_sample = micros();
    }
    Serial.println(millis() - start);
    sprintf(speech_data + strlen(speech_data), "%s", SUFFIX);
    Serial.println("out");
}

int8_t mulaw_encode(int16_t sample) {
   const uint16_t MULAW_MAX = 0x1FFF;
   const uint16_t MULAW_BIAS = 33;
   uint16_t mask = 0x1000;
   uint8_t sign = 0;
   uint8_t position = 12;
   uint8_t lsb = 0;
   if (sample < 0)
   {
      sample = -sample;
      sign = 0x80;
   }
   sample += MULAW_BIAS;
   if (sample > MULAW_MAX)
   {
      sample = MULAW_MAX;
   }
   for (; ((sample & mask) != mask && position >= 5); mask >>= 1, position--)
        ;
   lsb = (sample >> (position - 4)) & 0x0f;
   return (~(sign | ((position - 5) << 4) | lsb));
}

void play_riff() {        
  for(int i = 0; i < song_to_play.length; i++){
    new_note = song_to_play.notes[i];
    
    // only play if its not the same note
    if(new_note != old_note){
      ledcWriteTone(AUDIO_PWM, new_note);
      if(new_note > old_note){
        tft.fillScreen(TFT_ORANGE);
        tft.setTextColor(TFT_BLACK);
        tft.setCursor(0,0,1);
        tft.printf("Current Frequency:\n%f", new_note);
        
      } else if(new_note < old_note){
        tft.fillScreen(TFT_BLUE);
        tft.setTextColor(TFT_BLACK);
        tft.setCursor(0,0,1);
        tft.printf("Current Frequency:\n%f", new_note);
      }
    }
    
    delay(song_to_play.note_period);
    old_note = new_note;
  }
  ledcWriteTone(AUDIO_PWM, 0);
}

void parse_transcript(char* transcript){
  if(strstr(transcript, "play") || strstr(transcript, "Play")){
    if(strstr(transcript, "sorry") || strstr(transcript, "Sorry")){
      song_to_play = sorry; play_riff();
    } else if (strstr(transcript, "formation") || strstr(transcript, "Formation")){
      song_to_play = formation; play_riff();
    } else if(strstr(transcript, "humble") || strstr(transcript, "Humble")){
       song_to_play = humble; play_riff();
    }
    else if(strstr(transcript, "twinkle") || strstr(transcript, "Twinkle")){
      song_to_play = twinkle; play_riff();
    }
    else if(strstr(transcript, "Ode to Joy") || strstr(transcript, "ode to joy")){
      song_to_play = ode_to_joy; play_riff();
    }
    else if(strstr(transcript, "Ophelia") || strstr(transcript, "ophelia")){
      song_to_play = ophelia; play_riff();
    }
  }
}
