#include "Arduino.h"
#include "Blink.h"

Blink::Blink(int pin, int time)
{
  pinMode(pin, OUTPUT);
  _pin = pin;
  _time = time;
}

void Blink::blink()
{
  digitalWrite(_pin, HIGH);
  delay(_time);
  digitalWrite(_pin, LOW);
  delay(_time);
}

void Blink::blinkTime(int _time){
  digitalWrite(_pin, HIGH);
  delay(_time);
  digitalWrite(_pin, LOW);
  delay(_time);
}
