#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

class Button {
private:
  const uint8_t PIN;
  bool pressed;
  unsigned long debounceTime;

public:
  Button(uint8_t pin);
  
  bool isPressed() const;
  void setPressed(bool value);
  void IRAM_ATTR handleInterrupt();
  void displayMessage(String msg);
};

#endif
