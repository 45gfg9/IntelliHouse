#include "task.hxx"

static DS1302 rtc;

void task::begin()
{
    rtc.begin();
}

DateTime task::getTime()
{
    return rtc.now();
}
