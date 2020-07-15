#include <Arduino.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include "remote.hxx"

WiFiServer server(TCP_PORT);
WiFiUDP udp;

weather_data fetchWeatherData();
uint32_t fetchTime();

void setup()
{
    Serial.begin(115200);
    Serial.println();

    remote::begin();

    server.setNoDelay(true);

    udp.begin(UDP_PORT);
    server.begin();
}

void loop()
{
}

uint32_t fetchTime()
{
    static const size_t NTP_PACKET_SIZE = 48;
    static const uint32_t SECONDS_FROM_1900_TO_1970 = 2208988800UL;
    static const uint32_t TIMEOUT = 1000;

    IPAddress ntpIP;
    WiFi.hostByName("ntp.ntsc.ac.cn", ntpIP);

    byte buf[NTP_PACKET_SIZE];
    buf[0] = 0xE3;
    buf[2] = 0x06;
    buf[3] = 0xEC;
    buf[12] = 0x31;
    buf[13] = 0x4E;
    buf[14] = 0x31;
    buf[15] = 0x34;

    Serial.println(F("Sending UDP packet"));
    udp.beginPacket(ntpIP, 123);
    udp.write(buf, NTP_PACKET_SIZE);
    udp.endPacket();

    unsigned long start = millis();
    while (!udp.parsePacket())
    {
        if (millis() - start >= TIMEOUT)
        {
            Serial.println(F("Timeout waiting for UDP response"));
            return 0;
        }
    }
    Serial.println(F("Receiving packet"));
    udp.read(buf, NTP_PACKET_SIZE);

    uint32_t epoch = (buf[40] << 24 |
                      buf[41] << 16 |
                      buf[42] << 8 |
                      buf[43]) -
                     SECONDS_FROM_1900_TO_1970;

    return epoch;
}

weather_data fetchWeatherData()
{
    static const size_t JSON_BUFSIZE = 512;
    static const size_t PSK_STRLEN = 18;
    static char psk[PSK_STRLEN];

    if (*psk == 0)
    {
        EEPROM.begin(PSK_STRLEN);
        for (size_t i = 0; i < PSK_STRLEN; i++)
            psk[i] = EEPROM.read(i);
        EEPROM.end();
        Serial.println(psk);
    }

    HTTPClient http;

    if (!http.begin(String(F("http://api.seniverse.com/v3/weather/now.json?language=en&location=ip&key=")) + psk))
        return {"Error", "Unable to Connect :(", 0};

    int code = http.GET();
    if (code == 0)
        return {"Error", "HTTP GET Failed :(", 0};
    if (code != HTTP_CODE_OK)
        return {"Error", String(F("HTTP Code ")) + code, 0};

    String json = http.getString();

    DynamicJsonDocument doc(JSON_BUFSIZE);
    deserializeJson(doc, json);

    JsonObject result = doc["results"][0];
    const char *location = result["location"]["name"];

    JsonObject result_now = result["now"];
    const char *weather = result_now["text"];

    byte temp = atoi(result_now["temperature"]);

    return {location, weather, temp};
}
