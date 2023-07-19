#include "button.h"

Button::Button(uint8_t pin)
  : PIN(pin), pressed(false), debounceTime(0) {}

bool Button::isPressed() const {
  return pressed;
}

void Button::setPressed(bool value) {
  pressed = value;
}

void IRAM_ATTR Button::handleInterrupt() {
  unsigned long currentTime = millis();
  if (currentTime - debounceTime >= 250) {
    debounceTime = currentTime;

    if (digitalRead(PIN) == LOW) {
      pressed = true;
    } else {
      pressed = false;
    }
  }
}

void Button::displayMessage(String msg) {
  Serial.println(msg);
}