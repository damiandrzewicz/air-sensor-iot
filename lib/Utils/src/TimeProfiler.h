#pragma once

#include "Arduino.h"
#include "Logger.h"

#define INSIDEPROFILE 0

#if INSIDEPROFILE == 1
    #define TIMEPROFILE(op) TimeProfiler tp(op)
#else
    #define TIMEPROFILE(op)
#endif

class TimeProfiler
{
public:
    TimeProfiler(const char* op)
    { 
        _operation = op;
        _start = millis(); 
    }

    ~TimeProfiler()
    { 
        _end = millis();
        LOG_PROFILE("[%s] took [%d ms]", _operation, getDiff());
    }

private:
    long getDiff(){ return _end - _start; }

private:
    long _start = 0;
    long _end = 0;
    const char* _operation;
};