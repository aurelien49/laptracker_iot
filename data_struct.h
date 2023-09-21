#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H

struct DataStruct {
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  float temperature;
  float humidity;
  int16_t pitch;
  int16_t roll;
  int16_t yaw;
};

#endif
