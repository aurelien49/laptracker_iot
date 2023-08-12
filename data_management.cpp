#include "data_management.h"

DataManagement::DataManagement(int dhtPin, uint8_t dhtType, RTC_DS1307* rtc0, DateTime clock0, const int RECORDING_TIME0, const int MAX_RECORDS0)
  : dataList(),
    lastToggleTime(0),
    rtc(rtc0),
    dht(dhtPin, dhtType),
    dateAndTimeM(clock0),
    RECORDING_TIME(RECORDING_TIME0),
    MAX_RECORDS(MAX_RECORDS0) {
  dht.begin();
  rtc->begin();
  rtc->adjust(clock0);
}

std::vector<DataStruct>& DataManagement::getdataList() {
  return dataList;
}

void DataManagement::recordingData(unsigned long currentTime) {
  if (currentTime - lastToggleTime >= RECORDING_TIME) {
    lastToggleTime = currentTime;

    dateAndTimeM = rtc->now();

    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature)) {
      displayMessage("Echec reception température");
      return;
    }

    if (dataList.size() < MAX_RECORDS) {
      DataStruct data = { dateAndTimeM.year(), dateAndTimeM.month(), dateAndTimeM.day(), dateAndTimeM.hour(), dateAndTimeM.minute(), dateAndTimeM.second(), temperature, humidity };

      addTemperatureHumidity(data);
      displayMessage(String(data.day) + "-" + String(data.month) + "-" + String(data.year) + " / " + String(data.hour) + "-" + String(data.minute) + "-" + String(data.second) + " :" + ", humidity: " + String(data.temperature) + "°C, " + ", temperature: " + String(data.humidity));
    }
  }
}

void DataManagement::setdataList(const std::vector<DataStruct>& values) {
  dataList = values;
}

void DataManagement::addTemperatureHumidity(DataStruct values) {
  dataList.push_back(values);
}

void DataManagement::displayMessage(String msg) {
  Serial.println(msg);
}

void DataManagement::displayRecords() {
  for (const DataStruct& data : dataList) {
    Serial.print("Temperature: ");
    Serial.print(data.temperature);
    Serial.print(" °C, Humidity: ");
    Serial.print(data.humidity);
    Serial.println(" %");
  }
}

void DataManagement::eraseList() {
  dataList.clear();
}

void DataManagement::updateDateAndTime(DateTime d) {
  rtc->adjust(d);
}

DateTime DataManagement::getDateAndTime() {
  return rtc->now();
}