#ifndef DATA_MANAGEMENT_H
#define DATA_MANAGEMENT_H

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include "data_struct.h"
#include "DHT.h"
#include "helpers.h"
#include <RTClib.h>  //  ????
#include <vector>

class DataManagement {
private:
  std::vector<DataStruct> dataList;
  DHT* dht;
  RTC_DS1307* rtc;
  Adafruit_MPU6050* mpu;
  DateTime dateAndTimeM;
  unsigned long lastToggleTime;
  int RECORDING_TIME = 2000;  //  A tester
  int MAX_RECORDS = 2000;     //  A tester
  uint16_t minVal = 265;
  uint16_t maxVal = 402;
  int16_t pitch;
  int16_t roll;
  int16_t yaw;

public:
  DataManagement(DHT* dht0, RTC_DS1307* rtc0, Adafruit_MPU6050* mpu0, DateTime clock0, const int RECORDING_TIME0, const int MAX_RECORDS0);

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
