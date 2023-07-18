#include "DHT.h"
#include "Button.h"
#include "Led.h"
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

TemperatureHumidityRecords temperatureHumidityRecords;
TemperatureHumidity temperatureHumidity;

Button bpPower(BUTTON_PIN_POWER);
Button bpRecord(BUTTON_PIN_RECORD);
Led ld(LED_PIN);
DHT dht(DHTPIN, DHTTYPE);

void handleButtonInterruptPower() {
  bpPower.handleInterrupt();
}

void handleButtonInterruptRecord() {
  bpRecord.handleInterrupt();
}

void displayTemperatureHumidityRecords() {
  // Obtenez la liste de température et d'humidité
  std::vector<TemperatureHumidity>& records = temperatureHumidityRecords.getTemperatureHumidityList();

  // Parcourez la liste et affichez les valeurs
  for (const TemperatureHumidity& data : records) {
    Serial.print("Temperature: ");
    Serial.print(data.temperature);
    Serial.print(", Humidity: ");
    Serial.println(data.humidity);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN_POWER, INPUT_PULLUP);
  pinMode(BUTTON_PIN_RECORD, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  dht.begin();

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_POWER), handleButtonInterruptPower, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_RECORD), handleButtonInterruptRecord, FALLING);

  // displayTemperatureHumidityRecords();
}

void loop() {
  static unsigned long lastToggleTime = 0;
  unsigned long currentTime = millis();

  if (bpPower.isPressed()) {
    if (ld.getLight() || ld.getRecording()) {
      ld.toggle(LED_OFF);
      ld.setRecording(false);
    } else {
      bpRecord.setPressed(false);
      ld.toggle(LED_ON);
      ld.setRecording(false);
    }
    bpPower.setPressed(false);
  }

  if (bpRecord.isPressed() && ld.getLight()) {
    if (ld.getRecording()) {
      ld.toggle(LED_ON);
    } else {
      ld.toggle(LED_FLASHING);
    }
    bpRecord.setPressed(false);
  }

  if (ld.getRecording()) {
    if (currentTime - lastToggleTime >= 2000) {
      lastToggleTime = currentTime;
      ld.toggle(LED_ON);

      humidity = dht.readHumidity();
      temperature = dht.readTemperature();

      if (isnan(humidity) || isnan(temperature)) {
        Serial.println("Echec reception");
        return;
      }

      if (temperatureHumidityRecords.getTemperatureHumidityList().size() < 2000) {
        temperatureHumidity.humidity = humidity;
        temperatureHumidity.temperature = temperature;

        temperatureHumidityRecords.addTemperatureHumidity(temperatureHumidity);
        Serial.println(temperatureHumidityRecords.toStringSavedValue(temperatureHumidity));
      }

      delay(1000);
      ld.toggle(LED_OFF);
    }
  }

  // Allow other tasks to run by adding a small delay
  delay(1);
}
