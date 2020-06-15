#ifndef __FFF_TASK_H__
#define __FFF_TASK_H__

#include <Arduino.h>
#include <RTClib.h>

namespace task
{
    void begin();
    DateTime getTime();
} // namespace task

static DS1302 rtc;

void task::begin()
{
    rtc.begin();
}

DateTime task::getTime()
{
    return rtc.now();
}

#endif // __FFF_TASK_H__
