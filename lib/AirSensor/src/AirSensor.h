#pragma once

#include <Arduino.h>
#include <SPI.h>
#include "ArduinoWrapper.h"
#include "RFM69_ATC.h"
#include "SparkFunBME280.h"

class AirSensor : public ArduinoWrapper
{
private:
    struct Data
    {
        struct Air
        {
            double temperature = 0;
            double humidity = 0;
            double pressure = 0;
        } air;
        double batteryLevel = 0;
        int batteryPercent = 0;
        char uptime[10];
    };

public:
    virtual void setup() override;
    virtual void loop() override;

    void readAirData();
    void readBatteryData();
    void readUptime();
    void sendData();
    unsigned long getCycle();

private:
    RFM69_ATC _radio;
    BME280 _bme280; // I2C

    unsigned long _cycle = 0;
    Data _data;
    char _payload[50];

};

//extern AirSensorWrapper AirSensor;