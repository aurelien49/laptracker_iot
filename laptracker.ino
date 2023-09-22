#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include "DHT.h"
#include <BluetoothSerial.h>
#include "button.h"
#include "data_management.h"
#include "Grafcet.h"
#include "helpers.h"
#include "led.h"
#include "led_state.h"
#include <RTClib.h>
#include <vector>
#include <Wire.h>

#define BUTTON_PIN_POWER 18
#define BUTTON_PIN_RECORD 33
#define DHTPIN 32
#define DHTTYPE DHT11
#define I2C_FREQ 400000
#define IOT_DEVICE_NAME "ESP32-1"
#define LED_BLUE_PIN 18
#define LED_GREEN_PIN 19
#define LED_RED_PIN 4
#define SCL_GYR 26
#define SCL_RTC 22
#define SDA_GYR 25
#define SDA_RTC 21

const uint8_t I2C_GY_521_ADDRESS = 0x68;
const uint16_t MAX_RECORDS = 2000;
const uint16_t RECORDING_TIME = 2000;
const int32_t GYR_SENSOR_ID = 0;

const size_t bufferSize = 14;

Adafruit_MPU6050 mpu;
BluetoothSerial SerialBT;
Button* bpPower;
Button* bpRecord;
DataManagement* dataManagement;
DateTime clock0;
DHT dht(DHTPIN, DHTTYPE);
Led* blueLed;
Led* greenLed;
Led* redLed;
RTC_DS1307 rtc;
TwoWire i2c_rtc = TwoWire(0);
TwoWire i2c_gyr = TwoWire(1);

bool dataTimeUpdateRequired = false, readDateTimeRequired = false, readDataListRequired = false, razDataListRequired = false, maxRecordReached = false;
int recordSize = 0;
const std::vector<int> grafcetStepNumbers = { 0, 1, 2, 3, 4, 5, 6 };
Grafcet grafcet(grafcetStepNumbers);
std::vector<DataStruct> dataList;

void handleButtonInterruptRecordFalling() {
  bpRecord->handleInterrupt();
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin(IOT_DEVICE_NAME);

  pinMode(BUTTON_PIN_POWER, INPUT_PULLUP);
  pinMode(BUTTON_PIN_RECORD, INPUT_PULLUP);
  pinMode(LED_BLUE_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);

  bpPower = new Button(BUTTON_PIN_POWER);
  bpRecord = new Button(BUTTON_PIN_RECORD);
  clock0 = DateTime(2020, 12, 31, 23, 59, 30);
  blueLed = new Led(LED_BLUE_PIN);
  greenLed = new Led(LED_GREEN_PIN);
  redLed = new Led(LED_RED_PIN);

  i2c_gyr.setPins(SDA_GYR, SCL_GYR);

  bool isI2cGyrOK = i2c_gyr.begin(SDA_GYR, SCL_GYR, I2C_FREQ);
  bool isI2cRtcOK = i2c_rtc.begin(SDA_RTC, SCL_RTC, I2C_FREQ);

  logInfo("isI2cGyrOK = " + String(isI2cGyrOK));
  logInfo("isI2cRtcOK = " + String(isI2cRtcOK));

  delay(10);

  bool isMpuBeginOK = mpu.begin(I2C_GY_521_ADDRESS, &i2c_gyr, GYR_SENSOR_ID);
  logInfo("isMpuBeginOK = " + String(isMpuBeginOK));

  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  bool isRtcBeginOK = rtc.begin(&i2c_rtc);
  logInfo("isRtcBeginOK = " + String(isRtcBeginOK));
  rtc.adjust(clock0);  // Has to be commented

  dht.begin();

  dataManagement = new DataManagement(&dht, &rtc, &mpu, clock0, RECORDING_TIME, MAX_RECORDS);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_RECORD), handleButtonInterruptRecordFalling, FALLING);

  delay(1000);
  grafcet.update(0);
  displayMemorySizes();
}

void loop() {
  transitions();
  posterieur();
  otaManagement();
  delay(1);  // for async calls
}

void otaManagement() {
  if (SerialBT.available()) {
    logInfo("SerialBT.available");

    String commandReceived = SerialBT.readStringUntil('\n');

    size_t separatorPos = commandReceived.indexOf("::");
    String command = commandReceived.substring(0, separatorPos);
    logInfo("commande received : " + command);

    if (command == "UPDATE_TIME") {
      String temp = commandReceived.substring(separatorPos + 2);
      clock0 = decodeDateTimeString(temp);
      dataTimeUpdateRequired = true;
    } else if (command == "GET_DATE_TIME") {
      logInfo("L'app demande l'heure du module");
      sendDateTimeOverBluetooth();
      readDateTimeRequired = true;
    } else if (command == "GET_DATA_LIST") {
      readDataListRequired = true;
    } else if (command == "DELETE_DATA_LIST") {
      razDataListRequired = true;
    } else if (command == "APP_REQUEST_BLUE_LED_ON") {
      appRequestBlueLedOn();
    } else if (command == "APP_REQUEST_GREEN_LED_ON") {
      appRequestGreenLedOn();
    } else if (command == "APP_REQUEST_RED_LED_ON") {
      appRequestRedLedOn();
    } else if ((command == "APP_REQUEST_LEDS_OFF")) {
      appRequestLedsOff();
    } else {
      logError("Invalid Bluetooth command received : " + commandReceived);
    }
  }
}

void transitions() {
  // Etape 0 : Ajouter des initialisations nécessaires
  if (grafcet.getActiveStepNumber() == 0) {
    if (bpRecord->isPressed()) {
      // Etape 1 : la led verte est allumé pour signifier que le module est pret
      grafcet.update(1);
      //logInfo("Step 1 : ready");
    }
  } else if (grafcet.getActiveStepNumber() == 1) {
    if (bpRecord->isPressed() && !dataTimeUpdateRequired && !readDataListRequired && !razDataListRequired) {
      // Etape 2 : la led verte clignote et on enregistre la température, l'humidité et la date et l'heure toutes les 2 secondes
      grafcet.update(2);
      //recordSize = dataManagement->getdataList().size();
      //logInfo("Step 2 : il y a " + String(recordSize) + " enregistrements");
    }
    if (!bpRecord->isPressed() && dataTimeUpdateRequired && !readDataListRequired && !razDataListRequired) {
      // Etape 3 : Mise à jour de l'heure en Bluetooth avec l'app Flutter
      grafcet.update(3);
      //logInfo("Step 3 : date and time updated");
    }
    if (!bpRecord->isPressed() && !dataTimeUpdateRequired && readDataListRequired && !razDataListRequired) {
      // Etape 4 :  l'app Flutter vient lire en Bluetooth le contenu de la liste des données que l'on récupère par "dataManagement.getdataList()"
      grafcet.update(4);
    }
    if (!bpRecord->isPressed() && !dataTimeUpdateRequired && !readDataListRequired && razDataListRequired) {
      // Etape 5 : effacement de la liste de données via le Bluetooth avec l'app Flutter avec "dataManagement.eraseList()"
      grafcet.update(5);
    }
  } else if (grafcet.getActiveStepNumber() == 2) {
    if (bpRecord->isPressed() && !maxRecordReached) {
      grafcet.update(1);
      recordSize = dataManagement->getdataList().size();
      logInfo("Step 2 : il y a " + String(recordSize) + " enregistrements");
    } else if (!bpRecord->isPressed() && maxRecordReached) {
      grafcet.update(6);
      recordSize = dataManagement->getdataList().size();
      logInfo("Step 2 : il y a " + String(recordSize) + " enregistrements");
    }
  } else if (grafcet.getActiveStepNumber() == 3) {
    // Date and time update from Bluetooth
    if (!dataTimeUpdateRequired) {
      grafcet.update(1);
    }
  } else if (grafcet.getActiveStepNumber() == 4) {
    // Send records from Bluetooth
    if (!readDataListRequired) {
      grafcet.update(1);
    }
  } else if (grafcet.getActiveStepNumber() == 5) {
    // Erease records from Bluetooth
    if (!razDataListRequired) {
      grafcet.update(1);
    }
  } else if (grafcet.getActiveStepNumber() == 6) {
    // Max records reached
    if (bpRecord->isPressed()) {
      grafcet.update(1);
    }
  }
  bpRecord->setPressed(false);
}

void posterieur() {
  switch (grafcet.getActiveStepNumber()) {
    case 0:
      blueLed->toggle(LED_OFF);
      greenLed->toggle(LED_ON);
      redLed->toggle(LED_OFF);
      break;
    case 1:
      blueLed->toggle(LED_ON);
      greenLed->toggle(LED_OFF);
      redLed->toggle(LED_OFF);
      break;
    case 2:
      blueLed->toggle(LED_FLASHING);
      greenLed->toggle(LED_OFF);
      redLed->toggle(LED_OFF);
      dataManagement->recordingData(millis());
      recordSize = dataManagement->getdataList().size();
      maxRecordReached = recordSize > MAX_RECORDS ? true : false;
      break;
    case 3:
      rtc.adjust(clock0);
      dataTimeUpdateRequired = false;
      break;
    case 4:
      dataList = dataManagement->getdataList();
      sendDataListOverBluetooth();
      readDataListRequired = false;
      break;
    case 5:
      dataManagement->eraseList();
      razDataListRequired = false;
      logInfo("Step 5 : records ereased from Bluetooth");
      break;
    case 6:
      logInfo("Step 6 : maximum records reached");
      break;
    default:
      break;
  }
}

void appRequestBlueLedOn() {
  blueLed->toggle(LED_ON);
  greenLed->toggle(LED_OFF);
  redLed->toggle(LED_OFF);
}

void appRequestGreenLedOn() {
  blueLed->toggle(LED_OFF);
  greenLed->toggle(LED_ON);
  redLed->toggle(LED_OFF);
}

void appRequestRedLedOn() {
  blueLed->toggle(LED_OFF);
  greenLed->toggle(LED_OFF);
  redLed->toggle(LED_ON);
}

void appRequestLedsOff() {
  blueLed->toggle(LED_OFF);
  greenLed->toggle(LED_OFF);
  redLed->toggle(LED_OFF);
}

void sendDateTimeOverBluetooth() {
  String jsonListData;
  if (!rtc.isrunning()) {
    logError("RTC module is not running !");
    return;
  }

  String formattedDate = formatDateTime(rtc.now().year(), rtc.now().month(), rtc.now().day(), rtc.now().hour(), rtc.now().minute(), rtc.now().second());

  jsonListData += "{";
  jsonListData += "\"dateTimeFromEsp\":\"" + formattedDate + "\"";
  jsonListData += "},";

  jsonListData = jsonListData.substring(0, jsonListData.length() - 1);

  SerialBT.print("INIT_READ_DATE_TIME::" + jsonListData + "_END");
  readDateTimeRequired = false;
  logInfo("Heure du module " + formattedDate + " envoyée à l'app");
}

void sendStepChangebyOta(bool isRecording) {
  String jsonListData;

  for (const DataStruct& data : dataList) {
    jsonListData += "{";
    jsonListData += "\"setNumber\":\"" + String(grafcet.getActiveStepNumber()) + "\",";
    jsonListData += "\"recordingState\":\"" + String(isRecording) + "\"";
    jsonListData += "},";
  }
  jsonListData = "[" + jsonListData.substring(0, jsonListData.length() - 1) + "]";

  SerialBT.print("INIT_STEP_NUMBER_STATE::" + jsonListData + "_END");
}

void sendDataListOverBluetooth() {
  String jsonListData;
  //if (dataList.empty()) { return; }

  String headerVariables = "\"iotDeviceName\":\"" + String(IOT_DEVICE_NAME) + "\",";

  for (const DataStruct& data : dataList) {
    jsonListData += "{";
    jsonListData += "\"year\":\"" + String(data.year) + "\",";
    jsonListData += "\"month\":\"" + String(data.month) + "\",";
    jsonListData += "\"day\":\"" + String(data.day) + "\",";
    jsonListData += "\"hour\":\"" + String(data.hour) + "\",";
    jsonListData += "\"minute\":\"" + String(data.minute) + "\",";
    jsonListData += "\"second\":\"" + String(data.second) + "\",";
    jsonListData += "\"temperature\":\"" + String(data.temperature) + "\",";
    jsonListData += "\"humidity\":\"" + String(data.humidity) + "\",";
    jsonListData += "\"roll\":\"" + String(data.roll) + "\",";
    jsonListData += "\"pitch\":\"" + String(data.pitch) + "\",";
    jsonListData += "\"yaw\":\"" + String(data.yaw) + "\"";
    jsonListData += "},";
  }
  jsonListData = "{" + headerVariables + "\"data\":[" + jsonListData.substring(0, jsonListData.length() - 1) + "]}";

  SerialBT.print("INIT_READ_DATA_LIST::" + jsonListData + "_END");
}

DateTime decodeDateTimeString(String dateTimeString) {
  logInfo("decodeDateTimeString dateTimeString: " + dateTimeString);
  int yearPos = dateTimeString.indexOf("-");
  int monthPos = dateTimeString.indexOf("-", yearPos + 1);
  int dayPos = dateTimeString.indexOf(" ", monthPos + 1);
  int hourPos = dateTimeString.indexOf(":", dayPos + 1);
  int minutePos = dateTimeString.indexOf(":", hourPos + 1);
  int secondPos = minutePos + 1;

  int year = dateTimeString.substring(0, yearPos).toInt();
  int month = dateTimeString.substring(yearPos + 1, monthPos).toInt();
  int day = dateTimeString.substring(monthPos + 1, dayPos).toInt();
  int hour = dateTimeString.substring(dayPos + 1, hourPos).toInt();
  int minute = dateTimeString.substring(hourPos + 1, minutePos).toInt();
  int second = dateTimeString.substring(minutePos + 1).toInt();

  return DateTime(year, month, day, hour, minute, second);
}

String formatDateTime(int year, int month, int day, int hour, int minute, int second) {
  char formatted[20];
  sprintf(formatted, "%04d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, minute, second);
  return String(formatted);
}

void displayMemorySizes() {
  logInfo("\n==================================");
  logInfo("Tailles de la mémoire de l'ESP32 :");

  logInfo("Mémoire SRAM libre : ");
  logInfo(String(ESP.getFreeHeap()));

  logInfo("Taille du programme : ");
  logInfo(String(ESP.getSketchSize()));

  logInfo("Espace libre pour le programme : ");
  logInfo(String(ESP.getFreeSketchSpace()));

  logInfo("Taille totale de la puce flash : ");
  logInfo(String(ESP.getFlashChipSize()));

  logInfo("==================================\n");
}