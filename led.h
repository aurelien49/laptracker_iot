#ifndef LED_H
#define LED_H

#include <Arduino.h>
#include "led_state.h"

class Led {
private:
  const uint8_t PIN;
  LedState lightState;
  unsigned long lastToggleTime;
  bool blinkingLightOn;
  const unsigned long BLINKING_TIME_ON = 250;
  const unsigned long BLINKING_TIME_OFF = 1750;
  //#define BLINKING_TIME_ON 500
  //#define BLINKING_TIME_OFF 1500


public:
  Led(uint8_t pin);

  LedState getLedState();

  void setLedState(LedState state);

  void toggle(LedState state);

  void blinking();

  void displayMessage(String msg);
};

#endif
