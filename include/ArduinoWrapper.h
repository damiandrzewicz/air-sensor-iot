#pragma once

class ArduinoWrapper
{
public:
    virtual void setup() = 0;
    virtual void loop() = 0;
};