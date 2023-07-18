#ifndef TEMPERATURE_HUMIDITY_H
#define TEMPERATURE_HUMIDITY_H

#include <Arduino.h>
#include <vector>

class TemperatureHumidity {
private:
  std::vector<float> temperatureList;
  std::vector<float> humidityList;

public:
  TemperatureHumidity();

  std::vector<float>& getTemperatureList();
  void setTemperatureList(const std::vector<float>& values);
  std::vector<float>& getHumidityList();
  void setHumidityList(const std::vector<float>& values);

  void addTemperature(float temperature);
  void addHumidity(float humidity);

  String toString();
};

#endif
