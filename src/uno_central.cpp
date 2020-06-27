#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>
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

  Wire.onRequest([&]() {
    uint32_t time = rtc.now().unixtime();
    Serial.print(F("Now: "));
    Serial.println(time);

    common::write_uint32_t(Wire, LSBFIRST, time);
  });

  Wire.onReceive([&](int n) {
    Serial.print(F("Received "));
    Serial.println(n);
    // FIXME error reading
    uint32_t epoch = common::read_uint32_t(Wire, LSBFIRST);

    Serial.println(epoch);

    rtc.adjust(DateTime(epoch));
  });
}

void loop()
{
}
