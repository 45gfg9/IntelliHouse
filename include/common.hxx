#ifndef __SH_COMMON_H__
#define __SH_COMMON_H__

#include <Arduino.h>

struct weather_data
{
    String location;
    String weather;
    int temperature;

    String toString()
    {
        return location + ',' + weather + ',' + temperature;
    }
};

const weather_data emptyWeatherData = {"Unknown", "Empty Data :(", 0};

#endif // __SH_COMMON_H__
