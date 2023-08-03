#include "data_management.h"

std::vector<DataStruct>& DataManagement::getdataList() {
  return dataList;
}

void DataManagement::recordingData(unsigned long currentTime) {
  if (currentTime - lastToggleTime >= RECORDING_TIME) {
    lastToggleTime = currentTime;

    dateAndTime = clock3.getDateTime();

    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature)) {
      displayMessage("Echec reception température");
      return;
    }

    if (dataList.size() < MAX_RECORDS) {
      DataStruct data = { dateAndTime.year, dateAndTime.month, dateAndTime.day, dateAndTime.hour, dateAndTime.minute, dateAndTime.second, humidity, temperature };

      addTemperatureHumidity(data);
      displayMessage(String(data.day) + "-" + String(data.month) + "-" + String(data.year) + " / " + String(data.hour) + "-" + String(data.minute) + "-" + String(data.seconde) + " :" + "Temperature: " + String(data.temperature) + "°C, " + "humidity: " + String(data.humidity));
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