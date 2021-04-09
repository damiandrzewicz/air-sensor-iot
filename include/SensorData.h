#pragma once

#include "AirData.h"

struct SensorData
{
    double battery_level = 0;
    int battery_percent = 0;
    char uptime[10];
    AirData air_data;
};