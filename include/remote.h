#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <Udp.h>

#define EX_SSID nullptr
#define EX_PASS nullptr
#define AP_SSID "SH_FFF"
#define AP_PASS "nullptr!"

struct weather_data {
  String location;
  String weather;
  int temperature;

  String toString() const {
    return location + ',' + weather + ',' + temperature;
  }
};

namespace remote {
  void begin();
  void connect();
  void mDNSsetup(const String &name, int port = 80);

  void listenTime(UDP &udp);

  IPAddress getBroadcastIP(const IPAddress &ip, const IPAddress &mask);

  weather_data getWeatherData();
} // namespace remote
