#include "temperature_humidity_records.h"

std::vector<TemperatureHumidity>& TemperatureHumidityRecords::getTemperatureHumidityList() {
  return temperatureHumidityList;
}

void TemperatureHumidityRecords::recording(unsigned long currentTime) {
  if (currentTime - lastToggleTime >= RECORDING_TIME) {
    lastToggleTime = currentTime;

    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature)) {
      displayMessage("Echec reception température");
      return;
    }

    if (temperatureHumidityList.size() < MAX_RECORDS) {
      TemperatureHumidity temperatureHumidity = { humidity, temperature };

      addTemperatureHumidity(temperatureHumidity);
      displayMessage("Temperature: " + String(temperatureHumidity.temperature) + "°C, " + "humidity: " + String(temperatureHumidity.humidity));
    }
  }
}

void TemperatureHumidityRecords::setTemperatureHumidityList(const std::vector<TemperatureHumidity>& values) {
  temperatureHumidityList = values;
}

void TemperatureHumidityRecords::addTemperatureHumidity(TemperatureHumidity values) {
  temperatureHumidityList.push_back(values);
}

void TemperatureHumidityRecords::displayMessage(String msg) {
  Serial.println(msg);
}

void TemperatureHumidityRecords::displayRecords() {
  for (const TemperatureHumidity& data : temperatureHumidityList) {
    Serial.print("Temperature: ");
    Serial.print(data.temperature);
    Serial.print(" °C, Humidity: ");
    Serial.print(data.humidity);
    Serial.println(" %");
  }
}