#pragma once

/*
    This code is from RTCLib by NeiroN (https://github.com/NeiroNx/RTCLib)
    Original library is under MIT License.
*/

#include <Arduino.h>

// Simple general-purpose date/time class (no TZ / DST / leap second handling!)
class DateTime
{
public:
    DateTime(uint32_t t = 0);
    DateTime(uint16_t year, uint8_t month, uint8_t day,
             uint8_t hour = 0, uint8_t min = 0, uint8_t sec = 0);
    DateTime(const char *date, const char *time);
    DateTime(const __FlashStringHelper *date, const __FlashStringHelper *time);
    DateTime(const DateTime &copy);
    uint16_t year() const { return 2000 + yOff; }
    uint8_t month() const { return m; }
    uint8_t day() const { return d; }
    uint8_t hour() const { return hh; }
    uint8_t minute() const { return mm; }
    uint8_t second() const { return ss; }
    uint8_t dayOfWeek() const;
    void SetTime(const char *time);
    void SetDate(const char *date);
    void setyear(uint16_t year);
    void setmonth(uint8_t month);
    void setday(uint8_t day);
    void sethour(uint8_t hour) { hh = hour % 24; }
    void setminute(uint8_t minute) { mm = minute % 60; }
    void setsecond(uint8_t second) { ss = second % 60; }
    // 32-bit UNIX timestamp
    // An uint32_t should be able to store up to 2106,
    // which is beyond most chip's upper bound 2099
    void setunixtime(uint32_t t);
    uint32_t unixtime() const;
    String tostr() const;

    bool operator==(const DateTime &date) const;
    bool operator!=(const DateTime &date) const;
    bool operator<(const DateTime &date) const;
    bool operator>(const DateTime &date) const;
    bool operator<=(const DateTime &date) const;
    bool operator>=(const DateTime &date) const;

protected:
    uint8_t yOff, m, d, hh, mm, ss;
};
