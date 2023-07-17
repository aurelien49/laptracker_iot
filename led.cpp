#include "Led.h"

Led::Led(uint8_t pin)
  : PIN(pin), light(false), isFlashing(false) {}

void Led::toggle(LedState state) {
  switch (state) {
    case LED_OFF:
      digitalWrite(PIN, LOW);
      light = false;
      isFlashing = false;
      break;
    case LED_ON:
      digitalWrite(PIN, HIGH);
      light = true;
      isFlashing = false;
      break;
    case LED_FLASHING:
      isFlashing = true;
      break;
  }
}

bool Led::isMaybeFlashing() const {
  return isFlashing;
}

bool Led::isLightOn() const {
  return light;
}
