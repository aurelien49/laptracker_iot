#include "temperature_humidity.h"

TemperatureHumidity::TemperatureHumidity() {}

std::vector<float>& TemperatureHumidity::getTemperatureList() {
  return temperatureList;
}

void TemperatureHumidity::setTemperatureList(const std::vector<float>& values) {
  temperatureList = values;
}

std::vector<float>& TemperatureHumidity::getHumidityList() {
  return humidityList;
}

void TemperatureHumidity::setHumidityList(const std::vector<float>& values) {
  humidityList = values;
}

void TemperatureHumidity::addTemperature(float temperature) {
  temperatureList.push_back(temperature);
}

void TemperatureHumidity::addHumidity(float humidity) {
  humidityList.push_back(humidity);
}

String TemperatureHumidity::toString() {
  return "Has to be implemented !!!";
}
