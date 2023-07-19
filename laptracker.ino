#include "DHT.h"
#include "button.h"
#include "led.h"
#include "led_state.h"
#include "temperature_humidity.h"
#include "temperature_humidity_records.h"
#include <vector>

#define DHTPIN 22
#define DHTTYPE DHT11

const int BUTTON_PIN_POWER = 18;
const int BUTTON_PIN_RECORD = 21;
const int LED_PIN = 19;

float temperature = 0.0;
float humidity = 0.0;

DHT dht(DHTPIN, DHTTYPE);
TemperatureHumidity temperatureHumidity = { 0.0, 0.0 };
TemperatureHumidityRecords temperatureHumidityRecords(dht);

Button bpPower(BUTTON_PIN_POWER);
Button bpRecord(BUTTON_PIN_RECORD);
Led greenLed(LED_PIN);

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

  dht.begin();

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_POWER), handleButtonInterruptPower, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_RECORD), handleButtonInterruptRecord, FALLING);
}

void loop() {
  if (bpPower.isPressed()) {
    if (greenLed.getLedState() == LED_ON || greenLed.getLedState() == LED_FLASHING) {
      greenLed.toggle(LED_OFF);
      mainDisplayRecords(temperatureHumidityRecords.getTemperatureHumidityList());
    } else {
      greenLed.toggle(LED_ON);
      bpRecord.setPressed(false);
    }
    bpPower.setPressed(false);
  }

  if (bpRecord.isPressed()) {
    if (greenLed.getLedState() == LED_FLASHING) {
      greenLed.toggle(LED_ON);
    } else if (greenLed.getLedState() == LED_ON) {
      greenLed.toggle(LED_FLASHING);
    }
    bpRecord.setPressed(false);
  }

  if (greenLed.getLedState() == LED_FLASHING) {
    try {
      greenLed.blinking();
    } catch (const char* message) { Serial.println(message); }
  }

  if (greenLed.getLedState() == LED_FLASHING) {
    temperatureHumidityRecords.recording(millis());
  }

  delay(1);
}

void mainDisplayRecords(std::vector<TemperatureHumidity> temperatureHumidityList) {
  if (temperatureHumidityList.empty()) {
    Serial.println("La liste de température et d'humidité est vide !");
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
