#ifndef __FFF_REMOTE_H__
#define __FFF_REMOTE_H__

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <RTClib.h>
#include "climate.hxx"

namespace remote
{
    extern const char *EX_ssid;
    extern const char *EX_pass;
    extern const char *AP_ssid;
    extern const char *AP_pass;

    IPAddress begin();
    void connect();

    DateTime getTime();
    climate::dht_data getTH();
} // namespace remote

#endif // __FFF_REMOTE_H__
