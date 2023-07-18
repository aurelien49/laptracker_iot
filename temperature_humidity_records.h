#ifndef TEMPERATURE_HUMIDITY_RECORDS_H
#define TEMPERATURE_HUMIDITY_RECORDS_H

#include <Arduino.h>
#include "temperature_humidity.h"
#include <vector>

class TemperatureHumidityRecords {
private:
  std::vector<TemperatureHumidity> temperatureHumidityList;

public:
  TemperatureHumidityRecords() : temperatureHumidityList() {}

  std::vector<TemperatureHumidity>& getTemperatureHumidityList();
  void setTemperatureHumidityList(const std::vector<TemperatureHumidity>& values);

  void addTemperatureHumidity(TemperatureHumidity temperatureHumidity);

  String toStringSavedValue(TemperatureHumidity temperatureHumidity);
};

#endif
