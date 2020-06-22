#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>
#include <SimpleDHT.h>
#include "common.hxx"

enum action
{
  TIME,
  DHT11,
  NONE
} act;

static const int RTC_CLK = 8;
static const int RTC_DAT = 9;
static const int RTC_RST = 10;
static const int DHT_PIN = 5;

DS1302 rtc(RTC_RST, RTC_CLK, RTC_DAT);
SimpleDHT11 dht(DHT_PIN);

void setup()
{
  Wire.begin(IIC);

  rtc.begin();

  Wire.onReceive([](int n) {
    switch (Wire.read())
    {
    case 'T':
      act = TIME;
      break;
    case 'D':
      act = DHT11;
      break;
    }
  });

  Wire.onRequest([]() {
    switch (act)
    {
    case TIME:
      uint32_t time = rtc.now().unixtime();

      Wire.write(time);
      Wire.write(time >> 8);
      Wire.write(time >> 16);
      Wire.write(time >> 24);
      break;

    case DHT11:
      byte t, h;

      if (dht.read(&t, &h, nullptr) == SimpleDHTErrSuccess)
        t = h = 127;

      Wire.write(t);
      Wire.write(h);

      break;
    }

    act = NONE;
  });
}

void loop()
{
}
