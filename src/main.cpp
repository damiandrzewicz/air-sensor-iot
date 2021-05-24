#include <Arduino.h>
#include <Wire.h>

#include "Logger.h"
#include "LowPowerWrapper.h"
#include "AirSensor.h"
#include "Settings.h"

AirSensor air_sensor;

int led = 9;

void setup() {
  Serial.begin(BAUD);

  Logger::setLogLevel(Logger::Level::VERBOSE);
  LOG_VERBOSE("Starting...");
  LOG_NOTICE("Initialized Serial at [%d] baud", BAUD);

  LOG_NOTICE("Increasing SPI speed...");
  Wire.begin();
  Wire.setClock(400000); //Increase to fast I2C speed!
  
  air_sensor.setup();

  //pinMode(led, OUTPUT);  
}

void loop() {
  air_sensor.loop();
  Serial.flush();
  LowPowerWrp.DeepSleep(LOOPDELAYMS);
}