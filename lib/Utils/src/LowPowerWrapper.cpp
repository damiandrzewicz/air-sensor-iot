#include "LowPowerWrapper.h"

void LowPowerWrapper::DeepSleep(uint32_t sleepTime) {

    do {
        if (sleepTime > 8000)
        {
            LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
            sleepTime-=8000;
        }
        else if (sleepTime > 4000)
        {
            LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
            sleepTime-=4000;
        }
        else if (sleepTime > 2000)
        {
            LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
            sleepTime-=2000;
        }
        else if (sleepTime > 1000)
        {
            LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
            sleepTime-=1000;
        }
        else if (sleepTime > 512)
        {
            LowPower.powerDown(SLEEP_500MS, ADC_OFF, BOD_OFF);
            sleepTime-=512;
        }
        else if (sleepTime > 256)
        {
            LowPower.powerDown(SLEEP_250MS, ADC_OFF, BOD_OFF);
            sleepTime-=256;
        }
        else if (sleepTime > 128)
        {
            LowPower.powerDown(SLEEP_120MS, ADC_OFF, BOD_OFF);
            sleepTime-=128;
        }
        else if (sleepTime > 64)
        {
            LowPower.powerDown(SLEEP_60MS, ADC_OFF, BOD_OFF);
            sleepTime-=64;
        }
        else if (sleepTime > 32)
        {
            LowPower.powerDown(SLEEP_30MS, ADC_OFF, BOD_OFF);
            sleepTime-=32;
        }
        else if (sleepTime > 16)
        {
            LowPower.powerDown(SLEEP_15MS, ADC_OFF, BOD_OFF);
            sleepTime-=16;
        }
        else
        {
            sleepTime=0;
        }
    } while(sleepTime);
}

LowPowerWrapper LowPowerWrp;