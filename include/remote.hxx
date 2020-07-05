#ifndef __SH_REMOTE_H__
#define __SH_REMOTE_H__

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "common.hxx"

#define SERVER_PORT 80

#define EX_SSID nullptr
#define EX_PASS nullptr
#define AP_SSID "SH_FFF"
#define AP_PASS "nullptr!"

namespace remote
{
    extern IPAddress AP_ip;

    void begin();
    void connect();

    uint32_t getTime();
    String getWeatherJsonStr(String psk);
    common::weather_data getWeatherData();

    String header(IPAddress host, String uri, String query = emptyString);
    String header(String host, String uri, String query = emptyString);
} // namespace remote

#endif // __SH_REMOTE_H__
