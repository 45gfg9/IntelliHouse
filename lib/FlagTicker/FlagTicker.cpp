#include "FlagTicker.h"

FlagTicker::FlagTicker() : trigger(false)
{
}

void FlagTicker::begin(float s)
{
    ticker.attach(s, [&]() { trigger = true; });
}

void FlagTicker::begin_ms(uint32_t ms)
{
    ticker.attach_ms(ms, [&]() { trigger = true; });
}

void FlagTicker::stop()
{
    ticker.detach();
}

void FlagTicker::done()
{
    trigger = false;
}

FlagTicker::operator bool()
{
    return trigger;
}
