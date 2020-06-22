#include "common.hxx"

common::tah common::fromMonth(uint8_t month)
{
    if ((month + 10) % 12 / 6)
        return {{18, 23}, {30, 80}};
    else
        return {{23, 28}, {30, 60}};
}
