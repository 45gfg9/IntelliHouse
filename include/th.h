#ifndef __FFF_TH_H__
#define __FFF_TH_H__

#include <Arduino.h>
#include <SimpleDHT.h>

namespace th {
    struct data {
        byte temp;
        byte humid;
    };

    data getInside();
    data getOutside();
}

#endif // __FFF_TH_H__
