#ifndef __FFF_REMOTE_H__
#define __FFF_REMOTE_H__

#include <Arduino.h>
#include <ESP8266WiFi.h>

namespace remote
{
    const char *EX_ssid = nullptr;
    const char *EX_pass = nullptr;
    const char *AP_ssid = "SH_FFF";
    const char *AP_pass = "nullptr!";
    IPAddress begin();
    wl_status_t connect();
} // namespace remote

IPAddress remote::begin()
{
    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(EX_ssid, EX_pass);
    while (WiFi.status() != WL_CONNECTED)
        delay(100);
    WiFi.softAP(AP_ssid, AP_pass);
    return WiFi.softAPIP();
}

wl_status_t remote::connect()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(AP_ssid, AP_pass);

    return WiFi.status();
}

#endif // __FFF_REMOTE_H__
