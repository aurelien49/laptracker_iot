#ifndef LED_H
#define LED_H

#include <Arduino.h>

enum LedState {
  LED_OFF,
  LED_ON,
  LED_FLASHING
};

class Led {
private:
  const uint8_t PIN;
  bool light;
  bool isFlashing;

public:
  Led(uint8_t pin);

  void toggle(LedState state);
  bool isMaybeFlashing() const;
  bool isLightOn() const;
};

#endif
