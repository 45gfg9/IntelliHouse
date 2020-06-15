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

const static int DHT_PIN = 8;
static SimpleDHT11 dht(DHT_PIN);

climate::dht_data climate::get()
{
    byte t, h;
    if ((dht.read(&t, &h, nullptr)) != SimpleDHTErrSuccess)
    {
        t = h = -1;
    }

    return {t, h};
}

climate::season climate::fromMonth(uint8_t month)
{
    return climate::season((month + 10) % 12 / 3);
}

#endif // __FFF_TH_H__
