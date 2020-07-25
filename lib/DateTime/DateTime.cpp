#include "DateTime.h"

static const time_t SECONDS_FROM_1970_TO_2000 = 946684800L;
static const uint8_t DAYS_IN_MONTH[] PROGMEM = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

time_t DateTime::ut() const
{
    uint8_t y = this->y;
    uint16_t d = this->d;
    for (int i = 0; i < m - 1; i++)
        d += DAYS_IN_MONTH[i];
    if (m > 2 && y % 4 == 0)
        d++;
    d += 365 * y + (y + 3) / 4 - 1;

    return SECONDS_FROM_1970_TO_2000 + ((d * 24L + hh) * 60 + mm) * 60 + ss;
}

DateTime::DateTime(time_t t)
{
    // copied from RTCLib; need validation
    t -= SECONDS_FROM_1970_TO_2000; // bring to 2000 timestamp from 1970

    ss = t % 60;
    t /= 60;
    mm = t % 60;
    t /= 60;
    hh = t % 24;
    uint16_t days = t / 24;
    uint8_t leap;
    for (y = 0;; y++)
    {
        leap = y % 4 == 0;
        if (days < 365 + leap)
            break;
        days -= 365 + leap;
    }

    for (m = 1;; m++)
    {
        uint8_t daysPerMonth = pgm_read_byte(DAYS_IN_MONTH + m - 1);
        if (m == 2 && leap)
            daysPerMonth++;
        if (days < daysPerMonth)
            break;
        days -= daysPerMonth;
    }

    d = days + 1;
}

DateTime::DateTime(const String &date, const String &time)
{
    // Example: 2020/07/22 16:55:22
    y = date.toInt();
    m = date.substring(5).toInt();
    d = date.substring(8).toInt();
    hh = time.toInt();
    mm = time.substring(3).toInt();
    ss = time.substring(6).toInt();
}

DateTime::DateTime(uint16_t y, uint8_t m, uint8_t d,
                   uint8_t hh, uint8_t mm, uint8_t ss) : y(y < 2000 ? y : y - 2000), m(m), d(d),
                                                         hh(hh), mm(mm), ss(ss) {}

DateTime::DateTime(const DateTime &copy) : y(copy.y), m(copy.m), d(copy.d),
                                           hh(copy.hh), mm(copy.mm), ss(copy.ss) {}

String DateTime::toStr() const
{
    static char buf[20];
    snprintf_P(buf, 20, PSTR("20%02d/%02d/%02d %02d:%02d:%02d"),
               y, m, d, hh, mm, ss);
    return String(buf);
}
