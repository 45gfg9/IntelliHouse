#ifndef __SH_REMOTE_H__
#define __SH_REMOTE_H__

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "common.hxx"

#define SERVER_PORT 80

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

#endif // __SH_REMOTE_H__
