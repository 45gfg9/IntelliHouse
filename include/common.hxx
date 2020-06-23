#ifndef __SH_COMMON_H__
#define __SH_COMMON_H__

#include <Arduino.h>

namespace common
{
    struct dht_data
    {
        byte temp;
        byte humid;
    };

    struct weather_data
    {
        const char *location;
        const char *weather;
        const byte temperature;
    };

    class close_interval : public Printable
    {
    public:
        const int upper;
        const int lower;

        close_interval(int upper, int lower);

        bool in(int num) const;
        size_t printTo(Print &out) const;
    };

    struct tah
    {
        close_interval temp;
        close_interval humid;
    };

    tah fromMonth(uint8_t month);
} // namespace common

#endif // __SH_COMMON_H__
