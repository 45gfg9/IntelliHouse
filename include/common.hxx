#ifndef __SH_COMMON_H__
#define __SH_COMMON_H__

#include <Arduino.h>

struct weather_data
{
    String location;
    String weather;
    int temperature;

    String toString() const
    {
        return location + ',' + weather + ',' + temperature;
    }
};

#endif // __SH_COMMON_H__
