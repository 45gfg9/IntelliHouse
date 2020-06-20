#ifndef __FFF_TASK_H__
#define __FFF_TASK_H__

#include <Arduino.h>
#include <RTClib.h>

namespace task
{
    void begin();
    DateTime getTime();
    void calibrate();
} // namespace task

#endif // __FFF_TASK_H__
