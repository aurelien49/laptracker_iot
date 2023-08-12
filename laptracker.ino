#include "DHT.h"
#include <BluetoothSerial.h>
#include "button.h"
#include "data_management.h"
#include "Grafcet.h"
#include "led.h"
#include "led_state.h"
#include <vector>
#include <Wire.h>
#include <RTClib.h>

#define DHTTYPE DHT11

#define BUTTON_PIN_POWER 18
#define BUTTON_PIN_RECORD 33
#define DHTPIN 32
#define LED_BLUE_PIN 18
#define LED_GREEN_PIN 19
#define LED_RED_PIN 4
#define SDA_RTC 21
#define SCL_RTC 22

#define LAPTRACKERNAMECONTROLLER "ESP32-1"

const int RECORDING_TIME = 2000;
const int MAX_RECORDS = 2000;

BluetoothSerial SerialBT;

RTC_DS1307 rtc;
DateTime clock0 = DateTime(2019, 6, 2, 12, 30, 20);

float temperature = 0.0;
float humidity = 0.0;

Button* bpPower;
Button* bpRecord;
DataManagement* dataManagement;
Led* blueLed;
Led* greenLed;
Led* redLed;

const std::vector<int> grafcetStepNumbers = { 0, 1, 2, 3, 4, 5, 6 };
Grafcet grafcet(grafcetStepNumbers);

int recordSize = 0;
bool dataTimeUpdateRequired = false, readDateTimeRequired = false, readDataListRequired = false, razDataListRequired = false, maxRecordReached = false;

std::vector<DataStruct> dataList;

void handleButtonInterruptRecordFalling() {
  bpRecord->handleInterrupt();
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin(LAPTRACKERNAMECONTROLLER);

  bpPower = new Button(BUTTON_PIN_POWER);
  bpRecord = new Button(BUTTON_PIN_RECORD);
  dataManagement = new DataManagement(DHTPIN, DHTTYPE, &rtc, clock0, RECORDING_TIME, MAX_RECORDS);
  blueLed = new Led(LED_BLUE_PIN);
  greenLed = new Led(LED_GREEN_PIN);
  redLed = new Led(LED_RED_PIN);

  pinMode(BUTTON_PIN_POWER, INPUT_PULLUP);
  pinMode(BUTTON_PIN_RECORD, INPUT_PULLUP);
  pinMode(LED_BLUE_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_RECORD), handleButtonInterruptRecordFalling, FALLING);

  delay(1000);
  grafcet.update(0);
}

void loop() {
  transitions();
  posterieur();

  if (SerialBT.available()) {
    Serial.println("SerialBT.available");

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
  delay(1);
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
      //logInfo("Step 5 : records ereased from Bluetooth");
      break;
    case 6:
      //logInfo("Step 6 : maximum records reached");
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

  jsonListData += "{";
  jsonListData += "\"year\":\"" + String(rtc.now().year()) + "\",";
  jsonListData += "\"month\":\"" + String(rtc.now().month()) + "\",";
  jsonListData += "\"day\":\"" + String(rtc.now().day()) + "\",";
  jsonListData += "\"hour\":\"" + String(rtc.now().hour()) + "\",";
  jsonListData += "\"minute\":\"" + String(rtc.now().minute()) + "\",";
  jsonListData += "\"second\":\"" + String(rtc.now().second()) + "\"";
  jsonListData += "},";

  jsonListData = "[" + jsonListData.substring(0, jsonListData.length() - 1) + "]";

  SerialBT.print("INIT_READ_DATE_TIME::" + jsonListData + "_END");
  readDateTimeRequired = false;
  logInfo("Heure du module envoyée à l'app");
}

void sendStepChangeOta() {
  String jsonListData;
  if (dataList.empty()) { return; }
  for (const DataStruct& data : dataList) {
    jsonListData += "{";
    jsonListData += "\"setNumber\":\"" + String(grafcet.getActiveStepNumber()) + "\",";
    jsonListData += "\"state\":\"" + String(1) + "\"";
    jsonListData += "},";
  }
  jsonListData = "[" + jsonListData.substring(0, jsonListData.length() - 1) + "]";

  SerialBT.print("INIT_STEP_NUMBER_UPDATE::" + jsonListData + "_END");
}

void sendDataListOverBluetooth() {
  String jsonListData;
  if (dataList.empty()) { return; }
  for (const DataStruct& data : dataList) {
    jsonListData += "{";
    jsonListData += "\"year\":\"" + String(data.year) + "\",";
    jsonListData += "\"month\":\"" + String(data.month) + "\",";
    jsonListData += "\"day\":\"" + String(data.day) + "\",";
    jsonListData += "\"hour\":\"" + String(data.hour) + "\",";
    jsonListData += "\"minute\":\"" + String(data.minute) + "\",";
    jsonListData += "\"second\":\"" + String(data.second) + "\",";
    jsonListData += "\"temperature\":\"" + String(data.temperature) + "\",";
    jsonListData += "\"humidity\":\"" + String(data.humidity) + "\"";
    jsonListData += "},";
  }
  jsonListData = "[" + jsonListData.substring(0, jsonListData.length() - 1) + "]";

  SerialBT.print("INIT_READ_DATA_LIST::" + jsonListData + "_END");
}


DateTime decodeDateTimeString(String dateTimeString) {
  int yearPos = dateTimeString.indexOf("-");
  int monthPos = dateTimeString.indexOf("-", yearPos + 1);
  int dayPos = dateTimeString.indexOf(" ", monthPos + 1);
  int hourPos = dateTimeString.indexOf(" ", dayPos + 1);
  int minutePos = dateTimeString.indexOf(":", hourPos + 1);
  int secondPos = dateTimeString.indexOf(":", minutePos + 1);

  int year = dateTimeString.substring(0, yearPos).toInt();
  int month = dateTimeString.substring(yearPos + 1, monthPos).toInt();
  int day = dateTimeString.substring(monthPos + 1, dayPos).toInt();
  int hour = dateTimeString.substring(dayPos + 1, hourPos).toInt();
  int minute = dateTimeString.substring(hourPos + 1, minutePos).toInt();
  int second = dateTimeString.substring(minutePos + 1, secondPos).toInt();

  return DateTime(year, month, day, hour, minute, second);
}

void logInfo(const String& message) {
  Serial.print("[INFO] ");
  Serial.println(message);
}

void logWarning(const String& message) {
  Serial.print("[WARNING] ");
  Serial.println(message);
}

void logError(const String& message) {
  Serial.print("[ERROR] ");
  Serial.println(message);
}
