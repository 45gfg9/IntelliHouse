#include "common.hxx"

bool common::close_interval::in(int num) const
{
    return lower <= num && num <= upper;
}

common::close_interval::close_interval(int lower, int upper) : lower(lower), upper(upper)
{
}

size_t common::close_interval::printTo(Print &out) const
{
    size_t size;

    size += out.print('[');
    size += out.print(lower);
    size += out.print(", ");
    size += out.print(upper);
    size += out.print(']');

    return size;
}

common::tah common::fromMonth(uint8_t month)
{
    return ((month + 10) % 12 / 6) ? (common::tah){{18, 23}, {30, 80}} : (common::tah){{23, 28}, {30, 60}};
}
