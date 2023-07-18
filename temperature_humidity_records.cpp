#include "temperature_humidity_records.h"

std::vector<TemperatureHumidity>& TemperatureHumidityRecords::getTemperatureHumidityList() {
  return temperatureHumidityList;
}

void TemperatureHumidityRecords::setTemperatureHumidityList(const std::vector<TemperatureHumidity>& values) {
  temperatureHumidityList = values;
}

void TemperatureHumidityRecords::addTemperatureHumidity(TemperatureHumidity values) {
  temperatureHumidityList.push_back(values);
}

String TemperatureHumidityRecords::toStringSavedValue(TemperatureHumidity temperatureHumidity) {
  String result = "Humidity: " + String(temperatureHumidity.humidity) + ", temperature: " + String(temperatureHumidity.temperature) + "°C.";
  return result;
}
