#include "common.hxx"

common::tah common::fromMonth(uint8_t month)
{
    return ((month + 10) % 12 / 6) ? (common::tah){{18, 23}, {30, 80}} : (common::tah){{23, 28}, {30, 60}};
}
