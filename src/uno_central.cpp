#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>
#include <SimpleDHT.h>
#include "common.hxx"

enum action
{
  TIME,
  DHT,
  NONE = -1
} act;

static const int RTC_CLK = 8;
static const int RTC_DAT = 9;
static const int RTC_RST = 10;
static const int DHT_PIN = 5;

DS1302 rtc(RTC_RST, RTC_CLK, RTC_DAT);
SimpleDHT11 dht(DHT_PIN);

void setup()
{
  Serial.begin(115200);
  Wire.begin(IIC);

  rtc.begin();

  Wire.onReceive([](int n) {
    act = action(Wire.read());

    Serial.println(act);
  });

  Wire.onRequest([]() {
    uint32_t time;
    byte t, h;

    switch (act)
    {
    case TIME:
      time = rtc.now().unixtime();

      Wire.write(time);
      Wire.write(time >> 8);
      Wire.write(time >> 16);
      Wire.write(time >> 24);
      break;

    case DHT:
      if (dht.read(&t, &h, nullptr) == SimpleDHTErrSuccess)
        t = h = 127;
      Wire.write(t);
      Wire.write(h);
      break;

    case NONE:
      break;
    }

    act = NONE;
  });
}

void loop()
{
  // Serial.println("?");
  // delay(2000);
}
