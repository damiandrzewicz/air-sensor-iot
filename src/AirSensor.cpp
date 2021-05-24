#include "AirSensor.h"

#include "uptime.h"
#include "Settings.h"
#include "Credentials.h"
#include "Logger.h"
#include "TimeProfiler.h"
#include <ArduinoUniqueID.h>


void AirSensor::setup()
{
    LOG_DEBUG("exec...");

    uint16_t id = build_unique_id();

    LOG_NOTICE("Starting radio... Frequency: [868MHz], NodeId: [%d], NetworkId: [%d], ATC_RSSI: [%d]",
        id, NETWORKID, ATC_RSSI);
    // Init radio
    {
        //TIMEPROFILE("Init radio");
        _radio.initialize(FREQUENCY, id, NETWORKID);
        _radio.setHighPower(); //must include this only for RFM69HW/HCW!
        _radio.encrypt(ENCRYPTKEY);
        _radio.enableAutoPower(ATC_RSSI);
    }

    LOG_NOTICE("Setting radio in sleep mode...");
    _radio.sleep();

    LOG_VERBOSE("Starting BME280... Addr: [0x%02X]", 0x76);
    {
        //TIMEPROFILE("Init bme280");

        _bme280.setI2CAddress(0x76);
        _bme280.beginI2C();
    }

    LOG_NOTICE("Setting BME280 in sleep mode...");
    _bme280.setMode(MODE_SLEEP);

    LOG_NOTICE("Initial readings...");
    read_air_data();
    read_battery_data();
    read_uptime();

    LOG_NOTICE("Setting PowerSleep delay to [%lu] ms", LOOPDELAYMS);
}

void AirSensor::loop()
{
    LOG_DEBUG("exec...");
    _cycle++;
    LOG_VERBOSE("cycle: [%lu]", _cycle);

    {
        //TIMEPROFILE("loop measure");

        read_air_data();

        if(!(_cycle % 30)){
            read_battery_data();  // Check battery every 30 minutes
        }

        read_uptime();
        send_data(); // Send data buffer 
  }
}

void AirSensor::read_air_data()
{
    LOG_DEBUG("exec...");
    LOG_DEBUG("Setting bme280 to ForcedMode...");
    _bme280.setMode(MODE_FORCED); //Wake up sensor and take reading

    {
        //TIMEPROFILE("BME280 measure");
        while(_bme280.isMeasuring() == false) ; //Wait for sensor to start measurment
        while(_bme280.isMeasuring() == true) ; //Hang out while sensor completes the reading    
    }

    {
        //TIMEPROFILE("BME280 read");
        _data.air_data.temperature = _bme280.readTempC();
        _data.air_data.humidity = _bme280.readFloatHumidity();
        _data.air_data.pressure = _bme280.readFloatPressure() / 100.0F; //hPa
    }
}

void AirSensor::read_battery_data()
{
    LOG_DEBUG("exec...");

    unsigned int readings=0;

    {
        //TIMEPROFILE("Analog read");
        for (byte i=0; i<5; i++) //take several samples, and average
            readings+=analogRead(A0);
    }

    {
        //TIMEPROFILE("Battery calc");
        readings /= 5;
        _data.battery_level = (readings * 3.0) / 1023.0;
        static constexpr uint16_t LowestAnalogLevelBattery = (2.55 * 1023.0) / 3.0;
        _data.battery_percent = ((readings - LowestAnalogLevelBattery) / (1023.0 - LowestAnalogLevelBattery)) * 100;
    }

    LOG_DEBUG("readings: %d", readings);
}

void AirSensor::read_uptime()
{
    LOG_DEBUG("exec...");

    unsigned long milliseconds = (unsigned long)(LOOPDELAYMS) * _cycle;
    LOG_DEBUG("milliseconds= %lu", milliseconds);

    {
        //TIMEPROFILE("Uptime calc");
        uptime::calculateUptime(milliseconds);
    }

    sprintf(_data.uptime, "%lu:%lu:%lu", uptime::getDays(), uptime::getHours(), uptime::getMinutes());

    LOG_DEBUG("uptime: %s", _data.uptime);
}   

void AirSensor::send_data()
{
    LOG_DEBUG("exec...");
    sprintf(_payload, "%.2f;%.2f;%.2f;%d;%s", 
        _data.air_data.temperature,
        _data.air_data.humidity,
        _data.air_data.pressure,
        _data.battery_percent,
        _data.uptime
    );

    bool res;
    {
        //TIMEPROFILE("RFM send");
        res = _radio.sendWithRetry(GATEWAYID, _payload, strlen(_payload));
    }

    {
        //TIMEPROFILE("RFM sleep");
        _radio.sleep();
    }

    if(!res)
    {
        LOG_WARNING("missing ack...");
    }

    LOG_VERBOSE("Sent Payload: [%s]", _payload);
}

uint16_t AirSensor::build_unique_id()
{
    uint16_t id = 0;

	for (size_t i = 0; i < 8; i++)
	{
        LOG_DEBUG("id: %d=%d", i, UniqueID[i]);
        id += UniqueID[i];
	}
    return id;
}

