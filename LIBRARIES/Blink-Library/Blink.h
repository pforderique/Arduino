/*
  Blink_h - Library for blinking lights code.
  Created by Piero Orderique on 24 March 2019.
*/
#ifndef Blink_h
#define Blink_h

#include "Arduino.h"

class Blink
{
  private:
    int _pin;
    int _time;

  public:
    Blink(int pin);
    void blink();
    void blinkTime(int time);
};

#endif
