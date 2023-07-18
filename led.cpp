#include "Led.h"

Led::Led(uint8_t pin)
  : PIN(pin), light(false), isFlashing(false) {}

bool Led::getLight() const {
  return light;
}

void Led::setLight(bool value) {
  light = value;
}

bool Led::getRecording() const {
  return isFlashing;
}

void Led::setRecording(bool value) {
  isFlashing = value;
}

void Led::toggle(LedState state) {
  switch (state) {
    case LED_OFF:
      digitalWrite(PIN, LOW);
      light = false;
      break;
    case LED_ON:
      digitalWrite(PIN, HIGH);
      light = true;
      break;
    case LED_FLASHING:
      isFlashing = true;
      break;
  }
}
