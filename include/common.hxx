#ifndef __SH_COMMON_H__
#define __SH_COMMON_H__

#include <Arduino.h>

struct weather_data
{
    String location;
    String weather;
    int temperature;
};

const weather_data emptyWeatherData = {"Unknown", "Unknown", 0};

#endif // __SH_COMMON_H__
