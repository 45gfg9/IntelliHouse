#pragma once

#include <Arduino.h>

class DateTime {
  uint8_t y, m, d, hh, mm, ss;
  time_t ut() const;

public:
  DateTime(time_t t);
  DateTime(const String &date, const String &time);
  DateTime(
      uint16_t y, uint8_t m, uint8_t d, uint8_t hh, uint8_t mm, uint8_t ss);
  DateTime(const DateTime &copy);

  String toString() const;

  uint16_t getYear() const { return 2000 + y; }
  uint16_t getMonth() const { return m; }
  uint16_t getDate() const { return d; }
  uint16_t getHour() const { return hh; }
  uint16_t getMinute() const { return mm; }
  uint16_t getSecond() const { return ss; }

  bool operator<(const DateTime &rhs) const { return ut() < rhs.ut(); }
  bool operator>(const DateTime &rhs) const { return ut() > rhs.ut(); }
  bool operator<=(const DateTime &rhs) const { return ut() <= rhs.ut(); }
  bool operator>=(const DateTime &rhs) const { return ut() >= rhs.ut(); }
};
