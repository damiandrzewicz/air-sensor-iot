#include <Arduino.h>
#include <Wire.h>

#include "Logger.h"
#include "LowPowerWrapper.h"
#include "AirSensor.h"
#include "Settings.h"

AirSensor AirSensor_;

void setup() {
  Serial.begin(BAUD);

  Logger::setLogLevel(Logger::Level::VERBOSE);
  LOG_VERBOSE("Starting...");
  LOG_NOTICE("Initialized Serial at [%d] baud", BAUD);

  LOG_NOTICE("Increasing SPI speed...");
  Wire.begin();
  Wire.setClock(400000); //Increase to fast I2C speed!
  
  AirSensor_.setup();
}

void loop() {
  AirSensor_.loop();
  Serial.flush();
  LowPowerWrp.DeepSleep(LOOPDELAYMS);
}