#include "AirSensor.h"

#include "uptime.h"
#include "../../../src/settings.h"
#include "Logger.h"
#include "TimeProfiler.h"


void AirSensor::setup()
{
    LOG_DEBUG("exec...");

    LOG_NOTICE("Starting radio... Frequency: [868MHz], NodeId: [%d], NetworkId: [%d], ATC_RSSI: [%d]",
        NODEID, NETWORKID, ATC_RSSI);
    // Init radio
    {
        TIMEPROFILE("Init radio");
        _radio.initialize(FREQUENCY,NODEID,NETWORKID);
        _radio.setHighPower(); //must include this only for RFM69HW/HCW!
        _radio.encrypt(ENCRYPTKEY);
        _radio.enableAutoPower(ATC_RSSI);
    }


    LOG_NOTICE("Setting radio in sleep mode...");
    _radio.sleep();

    LOG_VERBOSE("Starting BME280... Addr: [0x%02X]", 0x76);
    {
        TIMEPROFILE("Init bme280");

        _bme280.setI2CAddress(0x76);
        _bme280.beginI2C();
    }

    LOG_NOTICE("Setting BME280 in sleep mode...");
    _bme280.setMode(MODE_SLEEP);

    LOG_NOTICE("Initial readings...");
    readAirData();
    readBatteryData();
    readUptime();

    LOG_NOTICE("Setting PowerSleep delay to [%d] ms", LOOPDELAYMS);
}

void AirSensor::readAirData()
{
    LOG_DEBUG("exec...");
    LOG_DEBUG("Setting bme280 to ForcedMode...");
    _bme280.setMode(MODE_FORCED); //Wake up sensor and take reading

    {
        TIMEPROFILE("BME280 measure");
        while(_bme280.isMeasuring() == false) ; //Wait for sensor to start measurment
        while(_bme280.isMeasuring() == true) ; //Hang out while sensor completes the reading    
    }

    {
        TIMEPROFILE("BME280 read");
        _data.air.temperature = _bme280.readTempC();
        _data.air.humidity = _bme280.readFloatHumidity();
        _data.air.pressure = _bme280.readFloatPressure() / 100.0F; //hPa
    }
}

void AirSensor::readBatteryData()
{
    LOG_DEBUG("exec...");

    unsigned int readings=0;

    {
        TIMEPROFILE("Analog read");
        for (byte i=0; i<5; i++) //take several samples, and average
            readings+=analogRead(A0);
    }

    {
        TIMEPROFILE("Battery calc");
        readings /= 5;
        _data.batteryLevel = (readings * 3.0) / 1023.0;
        static constexpr uint16_t LowestAnalogLevelBattery = (2.55 * 1023.0) / 3.0;
        _data.batteryPercent = ((readings - LowestAnalogLevelBattery) / (1023.0 - LowestAnalogLevelBattery)) * 100;
    }

    LOG_DEBUG("readings: %d", readings);
}

void AirSensor::readUptime()
{
    LOG_DEBUG("exec...");

    unsigned long milliseconds = (unsigned long)(LOOPDELAYMS) * _cycle;
    LOG_DEBUG("milliseconds= %lu", milliseconds);

    {
        TIMEPROFILE("Uptime calc");
        uptime::calculateUptime(milliseconds);
    }

    sprintf(_data.uptime, "%lu;%lu:%lu", uptime::getDays(), uptime::getHours(), uptime::getMinutes());

    LOG_DEBUG("uptime: %s", _data.uptime);
}   

void AirSensor::sendData()
{
    LOG_DEBUG("exec...");
    sprintf(_payload, "%d#%.2f#%.2f#%.2f#%.2f#%d#%s", 
        NODEID,
        _data.air.temperature,
        _data.air.humidity,
        _data.air.pressure,
        _data.batteryLevel,
        _data.batteryPercent,
        _data.uptime
    );

    bool res;
    {
        TIMEPROFILE("RFM send");
        res = _radio.sendWithRetry(GATEWAYID, _payload, strlen(_payload));
    }

    {
        TIMEPROFILE("RFM sleep");
        _radio.sleep();
    }


    if(!res)
    {
        LOG_WARNING("missing ack...");
    }

    LOG_VERBOSE("Sent Payload: [%s]", _payload);
}

unsigned long AirSensor::getCycle()
{
    return _cycle;
}

void AirSensor::loop()
{
    LOG_DEBUG("exec...");
    _cycle++;
    LOG_VERBOSE("_cycle: [%lu]", _cycle);

    {
        TIMEPROFILEEXT("loop measure");

        readAirData();

        if(_cycle % 30){
            readBatteryData();  // Check battery every 30 minutes
        }

        readUptime();
        
        sendData(); // Send data buffer 
  }
}