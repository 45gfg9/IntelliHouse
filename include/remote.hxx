#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <Udp.h>
#include "errmsg.hxx"

#define EX_SSID "CMCC-CUud"
#define EX_PASS "592492h2"
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

struct udp_packet {
  time_t epoch;
};

namespace remote {
  void begin();
  void connect();
  void mDNSsetup(const String &name, int port = 80);

  void listenTime(UDP &);

  IPAddress getBroadcastIP(const IPAddress &ip, const IPAddress &mask);

  bool configTcpClient(WiFiClient &);

  weather_data getWeatherData();
  void postEnvInfo(uint8_t temp, uint8_t humidity, uint8_t dust);
} // namespace remote
