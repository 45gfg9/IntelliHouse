#ifndef __SH_REMOTE_H__
#define __SH_REMOTE_H__

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include "common.hxx"

namespace remote
{
    extern char *EX_ssid;
    extern char *EX_pass;
    extern const char *AP_ssid;
    extern const char *AP_pass;

    extern IPAddress AP_ip;

    void begin();
    void connect();

    uint32_t getTime();
    String getWeatherJsonStr(String psk);
    common::weather_data getWeatherData();

    String header(IPAddress host, String uri);
    String header(String host, String uri);
    String header(IPAddress host, String uri, String query);
    String header(String host, String uri, String query);
} // namespace remote

#endif // __SH_REMOTE_H__
