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

    struct close_interval
    {
        int upper;
        int lower;

        bool in(int num)
        {
            return lower <= num && num <= upper;
        }
    };

    struct tah
    {
        close_interval temp;
        close_interval humid;
    };

    tah fromMonth(uint8_t month);
} // namespace common

#endif // __SH_COMMON_H__
