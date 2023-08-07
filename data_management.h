#ifndef DATA_MANAGEMENT_H
#define DATA_MANAGEMENT_H

#include <Arduino.h>
#include "data_struct.h"
#include "DHT.h"
#include <vector>
#include <RTClib.h>

class DataManagement {
private:
  std::vector<DataStruct> dataList;
  DHT dht;
  RTC_DS1307* rtc;
  DateTime dateAndTimeM;
  unsigned long lastToggleTime;
  int RECORDING_TIME = 2000;
  int MAX_RECORDS = 2000;

public:
  DataManagement(int dhtPin, uint8_t dhtType, RTC_DS1307* rtc0, DateTime clock0, const int RECORDING_TIME0, const int MAX_RECORDS0);

  std::vector<DataStruct>& getdataList();

  void recordingData(unsigned long currentTime);

  void displayRecords();

  void eraseList();

  void updateDateAndTime(DateTime dateAndTime);

  DateTime getDateAndTime();

private:
  void setdataList(const std::vector<DataStruct>& values);

  void addTemperatureHumidity(DataStruct data);

  void displayMessage(String msg);
};

#endif
