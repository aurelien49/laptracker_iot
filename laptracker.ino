#include "DHT.h"
#include "button.h"
#include "Grafcet.h"
#include "led.h"
#include "led_state.h"
#include "data_management.h"
#include <vector>
#include <Wire.h>

#define DHTTYPE DHT11

#define BUTTON_PIN_POWER 18
#define LED_PIN 19
#define SDA_RTC 21
#define SCL_RTC 22
#define DHTPIN 32
#define BUTTON_PIN_RECORD 33

DS3231 clock2;

float temperature = 0.0;
float humidity = 0.0;

DHT dht(DHTPIN, DHTTYPE);
DataManagement dataManagement(dht, clock2);

Button bpPower(BUTTON_PIN_POWER);
Button bpRecord(BUTTON_PIN_RECORD);

Led greenLed(LED_PIN);

// Numéros des étapes dans le Grafcet
const std::vector<int> stepNumbers = { 0, 1, 2, 3, 4, 5 };
Grafcet grafcet(stepNumbers);

bool dataTimeUpdateRequired = false, readDataListRequired = false, razDataListRequired = false;

void handleButtonInterruptPower() {
  bpPower.handleInterrupt();
}

void handleButtonInterruptRecordFalling() {
  bpRecord.handleInterrupt();
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN_POWER, INPUT_PULLUP);
  pinMode(BUTTON_PIN_RECORD, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  clock2.begin();
  clock2.setDateTime(2023, 8, 3, 21, 6, 0);

  dht.begin();

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_POWER), handleButtonInterruptPower, RISING);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_RECORD), handleButtonInterruptRecordFalling, FALLING);

  grafcet.update(0);
}

void loop() {
  transitions();
  posterieur();
  delay(1);
}

void transitions() {
  if (grafcet.getActiveStepNumber() == 0) {
    if (bpRecord.isPressed()) {
      grafcet.update(1);
    }
  } else if (grafcet.getActiveStepNumber() == 1) {
    if (bpRecord.isPressed() && !dataTimeUpdateRequired && !readDataListRequired && !razDataListRequired) {
      grafcet.update(2);
    }
    if (!bpRecord.isPressed() && dataTimeUpdateRequired && !readDataListRequired && !razDataListRequired) {
      grafcet.update(3);
    }
    if (!bpRecord.isPressed() && !dataTimeUpdateRequired && readDataListRequired && !razDataListRequired) {
      grafcet.update(4);
    }
    if (!bpRecord.isPressed() && !dataTimeUpdateRequired && !readDataListRequired && razDataListRequired) {
      grafcet.update(5);
    }
  } else if (grafcet.getActiveStepNumber() == 2 && bpRecord.isPressed()) {
    grafcet.update(1);
  }
  bpRecord.setPressed(false);
}

void posterieur() {
  switch (grafcet.getActiveStepNumber()) {
    case 1:
      greenLed.toggle(LED_ON);
      break;
    case 2:
      greenLed.toggle(LED_FLASHING);
      dataManagement.recordingData(millis());
      break;
    case 3:

      break;
    case 4:

      break;
    case 5:

      break;
    default:
      break;
  }
}

/*
void mainDisplayRecords(std::vector<TemperatureHumidity> temperatureHumidityList) {
  if (temperatureHumidityList.empty()) {
    Serial.println("La liste de temperature et d'humidite est vide !");
  } else {
    for (const TemperatureHumidity& data : temperatureHumidityList) {
      Serial.print("Temperature: ");
      Serial.print(data.temperature);
      Serial.print(" °C, Humidity: ");
      Serial.print(data.humidity);
      Serial.println(" %");
    }
  }
}
*/
