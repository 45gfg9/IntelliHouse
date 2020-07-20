#pragma once

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
