#ifndef TEMPERATURE_HUMIDITY_RECORDS_H
#define TEMPERATURE_HUMIDITY_RECORDS_H

#include <Arduino.h>
#include "DHT.h"
#include "temperature_humidity.h"
#include <vector>

class TemperatureHumidityRecords {
private:
  std::vector<TemperatureHumidity> temperatureHumidityList;
  DHT& dht;
  unsigned long lastToggleTime;
  const int RECORDING_TIME = 2000;
  const int MAX_RECORDS = 2000;

public:
  TemperatureHumidityRecords(DHT& dht)
    : temperatureHumidityList(), dht(dht), lastToggleTime(0) {}

  std::vector<TemperatureHumidity>& getTemperatureHumidityList();

  void recording(unsigned long currentTime);

  void displayRecords();

private:
  void setTemperatureHumidityList(const std::vector<TemperatureHumidity>& values);

  void addTemperatureHumidity(TemperatureHumidity temperatureHumidity);

  void displayMessage(String msg);
};

#endif
