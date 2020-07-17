#ifndef __FLAGTICKER_H__
#define __FLAGTICKER_H__

#include <Ticker.h>

class FlagTicker
{
    Ticker ticker;
    bool trigger;

public:
    FlagTicker();

    void begin(float s);
    void begin_ms(uint32_t ms);
    void stop();
    void done();

    operator bool();
};

#endif // __FLAGTICKER_H__
