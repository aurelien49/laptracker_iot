#include "helpers.h"

void logInfo(const String& message) {
  Serial.print("[Log INFO] ");
  Serial.println(message);
}

void logWarning(const String& message) {
  Serial.print("[Log WARNING] ");
  Serial.println(message);
}

void logError(const String& message) {
  Serial.print("[Log ERROR] ");
  Serial.println(message);
}