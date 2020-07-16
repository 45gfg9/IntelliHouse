#include <Arduino.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <FlagTicker.h>
#include "remote.hxx"

#define TIME_INTERVAL 60

WiFiServer server(TCP_PORT);
WiFiUDP udp;
FlagTicker ft_time;

weather_data fetchWeatherData();
time_t fetchTime();

void setup()
{
    Serial.begin(115200);
    Serial.println();

    remote::begin();

    server.setNoDelay(true);

    udp.begin(UDP_PORT);
    server.begin();

    ft_time.begin(TIME_INTERVAL);
}

void loop()
{
    if (ft_time)
    {
        Serial.println("Sending UDP packet");
        const size_t size = sizeof(time_t);
        byte buf[size];

        time_t t = fetchTime();
        timeval tv = {t, 0};
        settimeofday(&tv, nullptr);

        for (size_t i = 0; i < size; i++)
            buf[i] = t >> 8 * i;

        udp.beginPacket(remote::getBroadcastIP(WiFi.softAPIP(), IPAddress(255, 255, 255, 0)), UDP_PORT);
        udp.write(buf, 4);
        udp.endPacket();

        ft_time.done();
    }

    if (server.hasClient())
    {
        Serial.print("TCP handling.. ");
        size_t sent = 0;
        WiFiClient client = server.available();
        weather_data data = fetchWeatherData();

        sent += client.write(data.location.length());
        sent += client.print(data.location);
        sent += client.write(data.weather.length());
        sent += client.print(data.weather);
        sent += client.write(data.temperature);

        delay(500);
        Serial.printf_P(PSTR("%d bytes sent\r\nClosing Client\r\n"), sent);
        client.stop();
    }
}

time_t fetchTime()
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

    udp.beginPacket(ntpIP, 123);
    udp.write(buf, NTP_PACKET_SIZE);
    udp.endPacket();

    unsigned long start = millis();
    while (!udp.parsePacket())
    {
        if (millis() - start >= TIMEOUT)
        {
            Serial.println(F("UDP Timeout"));
            return 0;
        }
    }
    udp.read(buf, NTP_PACKET_SIZE);

    time_t epoch = (buf[40] << 24 |
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

    if (WiFi.getMode() == WIFI_AP)
        return {"Error", "Not Connected :(", 0};

    if (*psk == 0)
    {
        EEPROM.begin(PSK_STRLEN);
        for (size_t i = 0; i < PSK_STRLEN; i++)
            psk[i] = EEPROM.read(i);
        EEPROM.end();
    }

    WiFiClient client;
    HTTPClient http;

    if (!http.begin(client, String(F("http://api.seniverse.com/v3/weather/now.json?language=en&location=ip&key=")) + psk))
        return {"Error", "Unable to Connect :(", 0};

    int code = http.GET();
    if (code == 0)
    {
        http.end();
        return {"Error", "HTTP GET Failed :(", 0};
    }
    if (code != HTTP_CODE_OK)
    {
        http.end();
        return {"Error", String(F("HTTP Code ")) + code, 0};
    }

    String json = http.getString();
    http.end();

    DynamicJsonDocument doc(JSON_BUFSIZE);
    deserializeJson(doc, json);

    JsonObject result = doc["results"][0];
    const char *location = result["location"]["name"];

    JsonObject result_now = result["now"];
    const char *weather = result_now["text"];

    byte temp = atoi(result_now["temperature"]);

    return {location, weather, temp};
}
