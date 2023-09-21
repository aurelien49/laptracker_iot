#include "data_management.h"

DataManagement::DataManagement(DHT* dht0, RTC_DS1307* rtc0, Adafruit_MPU6050* mpu0, DateTime clock0, const int RECORDING_TIME0, const int MAX_RECORDS0)
  : dht(dht0),
    rtc(rtc0),
    mpu(mpu0),
    RECORDING_TIME(RECORDING_TIME0),
    MAX_RECORDS(MAX_RECORDS0),
    dateAndTimeM(clock0),
    dataList(),
    lastToggleTime(0) {
}

std::vector<DataStruct>& DataManagement::getdataList() {
  return dataList;
}

void DataManagement::recordingData(unsigned long currentTime) {
  if (currentTime - lastToggleTime >= RECORDING_TIME) {
    lastToggleTime = currentTime;

    dateAndTimeM = rtc->now();

    float humidity = dht->readHumidity();
    float temperature = dht->readTemperature();

    if (isnan(humidity) || isnan(temperature)) {
      displayMessage("Echec reception température");
      return;
    }

    sensors_event_t accel, gyro, temper;
    mpu->getEvent(&accel, &gyro, &temper);

    float accelX = accel.acceleration.x;
    float accelY = accel.acceleration.y;
    float accelZ = accel.acceleration.z;

    // Calcul des angles en degrés
    roll = atan2(accelY, accelZ) * RAD_TO_DEG;
    pitch = atan2(-accelX, sqrt(accelY * accelY + accelZ * accelZ)) * RAD_TO_DEG;
    yaw = gyro.gyro.z;

    float gyroX = gyro.gyro.x;
    float gyroY = gyro.gyro.y;
    float gyroZ = gyro.gyro.z;
    logInfo("gyroX : " + String(gyroX));
    logInfo("gyroY : " + String(gyroY));
    logInfo("gyroZ : " + String(gyroZ));

    //logInfo(", gyro.gyro.z :" + String(gyro.gyro.z));

    if (dataList.size() < MAX_RECORDS) {
      DataStruct data = { dateAndTimeM.year(), dateAndTimeM.month(), dateAndTimeM.day(), dateAndTimeM.hour(), dateAndTimeM.minute(), dateAndTimeM.second(), temperature, humidity, roll, pitch, yaw };

      addTemperatureHumidity(data);
      displayMessage(String(data.day) + "-" + String(data.month) + "-" + String(data.year) + " - " + String(data.hour) + "-" + String(data.minute) + "-" + String(data.second) + " :" + ", temperature: " + String(data.temperature) + "°C, " + ", humidity : " + String(data.humidity) + "%, Roll x: " + String(roll) + "°, pitch y: " + String(pitch) + "°, yaw z: " + String(yaw) + "°");
    } else {
      displayMessage("Maximum records reached !!! value = " + String(dataList.size() + ", maximum = " + String(MAX_RECORDS)));
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