#pragma once

#include <Arduino.h>
#include <SPI.h>
#include "ArduinoWrapper.h"
#include "RFM69_ATC.h"
#include "SparkFunBME280.h"
#include "SensorData.h"

class AirSensor : public ArduinoWrapper
{
private:


public:
    virtual void setup() override;
    virtual void loop() override;

    void read_air_data();
    void read_battery_data();
    void read_uptime();
    void send_data();

private:
    uint16_t build_unique_id();

private:
    RFM69_ATC _radio;
    BME280 _bme280;

    unsigned long _cycle = 0;
    SensorData _data;
    char _payload[50];
};