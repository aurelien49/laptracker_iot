#include "Button.h"
#include "Led.h"

const int BUTTON_PIN_POWER = 18;   // Broche du bouton de mise en service
const int BUTTON_PIN_RECORD = 21;  // Broche du bouton d'enregistrement
const int LED_PIN = 19;            // Broche de la diode

Button bpPower(BUTTON_PIN_POWER);
Button bpRecord(BUTTON_PIN_RECORD);
Led ld(LED_PIN);

void handleButtonInterruptPower() {
  bpPower.handleInterrupt();
}

void handleButtonInterruptRecord() {
  bpRecord.handleInterrupt();
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN_POWER, INPUT_PULLUP);
  pinMode(BUTTON_PIN_RECORD, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_POWER), handleButtonInterruptPower, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_RECORD), handleButtonInterruptRecord, FALLING);
}

void loop() {
  if (bpPower.isPressed()) {
    if (ld.isLightOn()) {
      ld.toggle(LED_OFF);
    } else {
      bpRecord.setPressed(false);
      ld.toggle(LED_ON);
    }
    bpPower.setPressed(false);
  }

  if (bpRecord.isPressed() && ld.isLightOn()) {
    if (ld.isMaybeFlashing()) {
      ld.toggle(LED_ON);
    } else {
      ld.toggle(LED_FLASHING);
    }
    bpRecord.setPressed(false);
  }

  if (ld.isMaybeFlashing()) {
    static bool ledState = false;
    static unsigned long lastToggleTime = 0;
    unsigned long currentTime = millis();
    if (currentTime - lastToggleTime >= 500) {
      lastToggleTime = currentTime;
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    }
  }
}
