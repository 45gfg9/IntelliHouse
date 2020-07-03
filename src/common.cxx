#include "common.hxx"

common::weather_data common::emptyData = {"Unknown", "Unknown", 0};

size_t common::write_uint32_t(Stream &stream, uint8_t order, uint32_t val)
{
    int written = 0;

    for (int i = 0; i < 4; i++)
        if (order == LSBFIRST)
            written += stream.write(val >> (8 * i));
        else
            written += stream.write(val >> (24 - 8 * i));

    return written;
}

uint32_t common::read_uint32_t(Stream &stream, uint8_t order)
{
    uint32_t val = 0;

    for (int i = 0; i < 4; i++)
        if (order == LSBFIRST)
            val |= stream.read() << (8 * i);
        else
            val |= stream.read() << (24 - (8 * i));

    return val;
}

bool common::close_interval::in(int num) const
{
    return lower <= num && num <= upper;
}

common::close_interval::close_interval(int lower, int upper) : lower(lower), upper(upper)
{
}

size_t common::close_interval::printTo(Print &out) const
{
    size_t size = 0;

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
