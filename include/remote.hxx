#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "common.hxx"

#define EX_SSID nullptr
#define EX_PASS nullptr
#define AP_SSID "SH_FFF"
#define AP_PASS "nullptr!"

// namespace necessity?
namespace remote
{
    void begin();
    void connect();

    void listenTime(UDP &udp);

    IPAddress getBroadcastIP(const IPAddress &ip, const IPAddress &mask);

    weather_data getWeatherData();
} // namespace remote
