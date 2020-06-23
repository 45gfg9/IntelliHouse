#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>
#include <SimpleDHT.h>
#include "common.hxx"

static const int RTC_CLK = 8;
static const int RTC_DAT = 9;
static const int RTC_RST = 10;

DS1302 rtc(RTC_RST, RTC_CLK, RTC_DAT);

void setup()
{
  Serial.begin(115200);
  Wire.begin(IIC);

  rtc.begin();

  Wire.onRequest([]() {
    uint32_t time = rtc.now().unixtime();

    Wire.write(time);
    Wire.write(time >> 8);
    Wire.write(time >> 16);
    Wire.write(time >> 24);
  });
}

void loop()
{
  // Serial.println("?");
  // delay(2000);
}
