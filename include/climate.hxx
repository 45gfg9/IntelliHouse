#ifndef __FFF_TH_H__
#define __FFF_TH_H__

#include <Arduino.h>
#include <SimpleDHT.h>

namespace climate
{
    struct dht_data
    {
        byte temp;
        byte humid;
    };

    dht_data get();

    enum season
    {
        SPRING,
        SUMMER,
        AUTUMN,
        WINTER
    };

    season fromMonth(uint8_t month);
} // namespace climate

#endif // __FFF_TH_H__
