#include "DateTime.h"

/*
    This code is from RTCLib by NeiroN (https://github.com/NeiroNx/RTCLib)
    Original library is under MIT License.
*/

////////////////////////////////////////////////////////////////////////////////
// utility code, some of this could be exposed in the DateTime API if needed
#define SECONDS_FROM_1970_TO_2000 946684800L

static uint8_t bcd2bin(uint8_t val) { return val - 6 * (val >> 4); }
static uint8_t bin2bcd(uint8_t val) { return val + 6 * (val / 10); }

const uint8_t daysInMonth[] PROGMEM = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// number of days since 2000/01/01, valid for 2001..2099
static uint16_t date2days(uint16_t y, uint8_t m, uint8_t d)
{
    if (y >= 2000)
        y -= 2000;
    uint16_t days = d;
    for (uint8_t i = 1; i < m; i++)
        days += pgm_read_byte(daysInMonth + i - 1);
    if (m > 2 && y % 4 == 0)
        days++;
    return days + 365 * y + (y + 3) / 4 - 1;
}

static long time2long(uint16_t days, uint8_t h, uint8_t m, uint8_t s)
{
    return ((days * 24L + h) * 60 + m) * 60 + s;
}

static uint8_t conv2d(const char *p)
{
    uint8_t v = 0;
    if ('0' <= *p && *p <= '9')
        v = *p - '0';
    return 10 * v + *++p - '0';
}

////////////////////////////////////////////////////////////////////////////////
// DateTime implementation - ignores time zones and DST changes
// NOTE: also ignores leap seconds, see http://en.wikipedia.org/wiki/Leap_second
DateTime::DateTime(const DateTime &copy)
{
    yOff = copy.yOff;
    m = copy.m;
    d = copy.d;
    hh = copy.hh;
    mm = copy.mm;
    ss = copy.ss;
}

DateTime::DateTime(uint32_t t)
{
    setunixtime(t);
}

DateTime::DateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
{
    if (year >= 2000)
        year -= 2000;
    yOff = year;
    m = month;
    d = day;
    hh = hour;
    mm = min;
    ss = sec;
}

// A convenient constructor for using "the compiler's time":
//   DateTime now(__DATE__, __TIME__);
// NOTE: using PSTR would further reduce the RAM footprint
DateTime::DateTime(const char *date, const char *time)
{
    SetDate(date);
    SetTime(time);
}

// A convenient constructor for using "the compiler's time":
// This version will save RAM by using PROGMEM to store it by using the F macro.
// DateTime now(F(__DATE__), F(__TIME__));
DateTime::DateTime(const __FlashStringHelper *date, const __FlashStringHelper *time)
{
    // sample input: date = "Dec 26 2009", time = "12:34:56"
    char _date[11];
    char _time[8];
    memcpy_P(_date, date, 11);
    memcpy_P(_time, time, 8);
    DateTime(date, time);
}

DateTime::DateTime(const char *sdate)
{
    DateTime(sdate, sdate + 11);
}

/*********************************************/
/*         Comparison & Modification         */
/*********************************************/

bool DateTime::operator==(const DateTime &date) const
{
    return unixtime() == date.unixtime();
}

bool DateTime::operator==(const char *sdate) const
{
    return *this == DateTime(sdate);
}

bool DateTime::operator!=(const DateTime &date) const
{
    return unixtime() != date.unixtime();
}

bool DateTime::operator!=(const char *sdate) const
{
    return *this != DateTime(sdate);
}

bool DateTime::operator<(const DateTime &date) const
{
    return unixtime() < date.unixtime();
}

bool DateTime::operator>(const DateTime &date) const
{
    return unixtime() > date.unixtime();
}

bool DateTime::operator<=(const DateTime &date) const
{
    return unixtime() <= date.unixtime();
}

bool DateTime::operator>=(const DateTime &date) const
{
    return unixtime() >= date.unixtime();
}

uint8_t DateTime::dayOfWeek() const
{
    uint16_t day = date2days(yOff, m, d);
    return (day + 6) % 7; // Jan 1, 2000 is a Saturday, i.e. returns 6
}

void DateTime::SetTime(const char *time)
{
    hh = conv2d(time);
    mm = conv2d(time + 3);
    ss = conv2d(time + 6);
}

void DateTime::SetDate(const char *date)
{
    // sample input: date = "Dec 26 2009", time = "12:34:56"
    // or date = "26-12-2009"
    yOff = conv2d(date + 9);
    // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec
    d = conv2d(date + 4);
    switch (date[0])
    {
    case 'J':
        m = date[1] == 'a' ? 1 : date[2] == 'n' ? 6 : 7;
        break;
    case 'F':
        m = 2;
        break;
    case 'A':
        m = date[1] == 'p' ? 4 : 8;
        break;
    case 'M':
        m = date[2] == 'r' ? 3 : 5;
        break;
    case 'S':
        m = 9;
        break;
    case 'O':
        m = 10;
        break;
    case 'N':
        m = 11;
        break;
    case 'D':
        m = 12;
        break;
    default:
        yOff = conv2d(date + 8);
        m = conv2d(date + 3);
        d = conv2d(date);
    }
}

void DateTime::setyear(uint16_t year)
{
    yOff = year - (year >= 2000 ? 2000 : 0);
}

void DateTime::setmonth(uint8_t month)
{
    m = month % 12;
}

void DateTime::setday(uint8_t day)
{
    d = day % daysInMonth[m - 1];
}

void DateTime::setunixtime(uint32_t t)
{
    t -= SECONDS_FROM_1970_TO_2000; // bring to 2000 timestamp from 1970

    ss = t % 60;
    t /= 60;
    mm = t % 60;
    t /= 60;
    hh = t % 24;
    uint16_t days = t / 24;
    uint8_t leap;
    for (yOff = 0;; yOff++)
    {
        leap = yOff % 4 == 0;
        if (days < 365 + leap)
            break;
        days -= 365 + leap;
    }

    for (m = 1;; m++)
    {
        uint8_t daysPerMonth = pgm_read_byte(daysInMonth + m - 1);
        if (leap && m == 2)
            daysPerMonth++;
        if (days < daysPerMonth)
            break;
        days -= daysPerMonth;
    }

    d = days + 1;
}

uint32_t DateTime::unixtime() const
{
    uint16_t days = date2days(yOff, m, d);
    uint32_t t = time2long(days, hh, mm, ss);
    t += SECONDS_FROM_1970_TO_2000;

    return t;
}

// TODO follow strftime format, could be difficult
char *DateTime::format(char *ret) const
{
    for (uint8_t i = 0; i < strlen(ret); i++)
    {
        if (ret[i] == 'h' && ret[i + 1] == 'h')
        {
            ret[i] = '0' + hh / 10;
            ret[i + 1] = '0' + hh % 10;
        }

        if (ret[i] == 'm' && ret[i + 1] == 'm')
        {
            ret[i] = '0' + mm / 10;
            ret[i + 1] = '0' + mm % 10;
        }

        if (ret[i] == 's' && ret[i + 1] == 's')
        {
            ret[i] = '0' + ss / 10;
            ret[i + 1] = '0' + ss % 10;
        }

        if (ret[i] == 'D' && ret[i + 1] == 'D')
        {
            ret[i] = '0' + d / 10;
            ret[i + 1] = '0' + d % 10;
        }

        if (ret[i] == 'M' && ret[i + 1] == 'M')
        {
            ret[i] = '0' + m / 10;
            ret[i + 1] = '0' + m % 10;
        }

        if (ret[i] == 'Y')
        {
            if (ret[i + 3] == 'Y')
            {
                ret[i] = '2';
                ret[i + 1] = '0';
                ret[i + 2] = '0' + (yOff / 10) % 10;
                ret[i + 3] = '0' + yOff % 10;
            }
            else if (ret[i + 1] == 'Y')
            {
                ret[i] = '0' + (yOff / 10) % 10;
                ret[i + 1] = '0' + yOff % 10;
            }
        }
    }
    return ret;
}

char *DateTime::tostr(char *charr) const
{
    charr[0] = '2';
    charr[1] = '0';
    charr[4] = charr[7] = '/';
    charr[13] = charr[16] = ':';
    charr[10] = ' ';
    charr[2] = '0' + yOff / 10;
    charr[3] = '0' + yOff % 10;
    charr[5] = '0' + m / 10;
    charr[6] = '0' + m % 10;
    charr[8] = '0' + d / 10;
    charr[9] = '0' + d % 10;
    charr[11] = '0' + hh / 10;
    charr[12] = '0' + hh % 10;
    charr[14] = '0' + mm / 10;
    charr[15] = '0' + mm % 10;
    charr[17] = '0' + ss / 10;
    charr[18] = '0' + ss % 10;
    charr[19] = '\0';

    return charr;
}

String DateTime::tostr() const
{
    static char buf[20];

    return String(tostr(buf));
}
