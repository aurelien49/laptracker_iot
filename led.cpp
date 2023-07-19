#include "led.h"
#include "led_state.h"

Led::Led(uint8_t pin)
  : PIN(pin), lightState(LED_OFF), lastToggleTime(0), blinkingLightOn(false) {}

LedState Led::getLedState() {
  return lightState;
}

void Led::setLedState(LedState state) {
  lightState = state;
}

void Led::blinking() {
  unsigned long currentTime = millis();

  if (currentTime - lastToggleTime >= (blinkingLightOn == false ? BLINKING_TIME_OFF : BLINKING_TIME_ON)) {
    lastToggleTime = currentTime;

    if (blinkingLightOn == false) {
      digitalWrite(PIN, HIGH);
      blinkingLightOn = true;
    } else {
      digitalWrite(PIN, LOW);
      blinkingLightOn = false;
    }
  }
}

void Led::toggle(LedState state) {
  switch (state) {
    case LED_OFF:
      setLedState(LED_OFF);
      digitalWrite(PIN, LOW);
      break;
    case LED_ON:
      setLedState(LED_ON);
      digitalWrite(PIN, HIGH);
      break;
    case LED_FLASHING:
      setLedState(LED_FLASHING);
      blinking();
      break;
    default:
      throw 'Led state not implemented';
  }
}

void Led::displayMessage(String msg) {
  Serial.println(msg);
}
