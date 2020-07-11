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
        String location;
        String weather;
        int temperature;
    };
    extern const weather_data emptyData;

    class close_interval : public Printable
    {
    public:
        const int lower;
        const int upper;

        close_interval(int lower, int upper);

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
