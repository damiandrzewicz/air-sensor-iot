// =============================================================================
//
// Copyright (c) 2013-2016 Christopher Baker <http://christopherbaker.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =============================================================================


#pragma once

#include "Arduino.h"

#if defined(ARDUINO_ARCH_AVR)
  #include <avr/pgmspace.h>
#elif defined(ESP8266)
  #include <pgmspace.h>
#endif

#define LOGX(level, fmt, args...) Logger::log(level, __FUNCTION__, PSTR(fmt), ## args)
#define LOG_VERBOSE(fmt, args...) LOGX(Logger::Level::VERBOSE, fmt, ##args)
#define LOG_DEBUG(fmt, args...)  LOGX(Logger::Level::DEBUG, fmt, ##args)
#define LOG_NOTICE(fmt,args...)  LOGX(Logger::Level::NOTICE, fmt, ##args)
#define LOG_WARNING(fmt, args...)  LOGX(Logger::Level::WARNING, fmt, ##args)
#define LOG_ERROR(fmt, args...)  LOGX(Logger::Level::ERROR, fmt, ##args)
#define LOG_FATAL(fmt, args...)  LOGX(Logger::Level::FATAL, fmt, ##args)
#define LOG_PROFILE(fmt,args...)  LOGX(Logger::Level::PROFILE, fmt, ##args)

class Logger
{
public:
    enum Level
    {
        VERBOSE = 0,
        DEBUG,
        NOTICE,
        WARNING,
        ERROR,
        FATAL,
        PROFILE,
        SILENT
    };

    typedef void (*LoggerOutputFunction)(Level level, const char* module, const char* message);

    static void setLogLevel(Level level);
    static Level getLogLevel();

    static Logger& getInstance();
    static void setOutputFunction(LoggerOutputFunction loggerOutputFunction);
    static const char* asString(Level level);

    template<typename... Args> 
    static void log(Level level, const char* module, PGM_P fmt, Args... args){
      if (level >= getLogLevel()){
          sprintf_P(MessageBuffer, fmt, args...);
          getInstance()._loggerOutputFunction(level, module, MessageBuffer);
      }
    }

    Logger(const Logger&) = delete;
    void operator = (const Logger&) = delete;

private:
    Logger();

    static void defaultOutputFunction(Level level, const char* module, const char* message);

    static char MessageBuffer[100];
    static char LevelBuffer[10];
    static char OutputBuffer[300];
    
    Level _level;
    LoggerOutputFunction _loggerOutputFunction;
};
