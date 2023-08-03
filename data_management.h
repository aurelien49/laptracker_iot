#ifndef DATA_MANAGEMENT_H
#define DATA_MANAGEMENT_H

#include <Arduino.h>
#include "data_struct.h"
#include "DHT.h"
#include <DS3231.h>
#include <vector>

class DataManagement {
private:
  std::vector<DataStruct> dataList;
  DHT& dht;
  DS3231 clock3;
  RTCDateTime dateAndTime;
  unsigned long lastToggleTime;
  const int RECORDING_TIME = 2000;
  const int MAX_RECORDS = 2000;

public:
  DataManagement(DHT& dht, DS3231& clock2)
    : dataList(), dht(dht), clock3(clock2), lastToggleTime(0) {}

  std::vector<DataStruct>& getdataList();

  void recordingData(unsigned long currentTime);

  void displayRecords();

private:
  void setdataList(const std::vector<DataStruct>& values);

  void addTemperatureHumidity(DataStruct data);

  void displayMessage(String msg);
};

#endif
