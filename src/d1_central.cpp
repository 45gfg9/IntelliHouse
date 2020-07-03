#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include "remote.hxx"

static const int udpPort = 1337;

ESP8266WebServer server(SERVER_PORT);
WiFiUDP udp;

common::weather_data fetchWeatherData();
uint32_t fetchTime();

void setup()
{
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  remote::begin();
  udp.begin(udpPort);

  server.on("/", [&]() {
    Serial.println(F("Pong!"));
    server.send_P(200, "text/plain", PSTR("Pong!"));
  });

  server.on("/weather", [&]() {
    Serial.println(F("Fetching weather"));
    String data;
    if (WiFi.getMode() == WIFI_AP_STA)
    {
      common::weather_data weather = fetchWeatherData();
      data = weather.location + ',' + weather.weather + ',' + weather.temperature;
    }
    else
    {
      data = "NC,NC,0";
    }
    server.send(200, "text/plain", data);
  });

  server.on("/time", [&]() {
    Serial.println(F("Fetching time"));
    uint32_t epoch;
    if (WiFi.getMode() == WIFI_AP_STA)
      epoch = fetchTime();
    else
      epoch = millis() / 1000;
    server.send(200, "text/plain", String(epoch));
  });

  server.onNotFound([&]() {
    server.send_P(404, "text/plain", PSTR("Where are you looking at?!"));
  });

  server.begin();
}

void loop()
{
  server.handleClient(); // non-blocking
}

uint32_t fetchTime()
{
  static const size_t NTP_PACKET_SIZE = 48;
  static const uint32_t SEVENTY_YEARS = 2208988800UL;
  static const uint32_t TIMEOUT = 1000;
  static IPAddress ntpIP;
  static byte buf[NTP_PACKET_SIZE];

  WiFi.hostByName("ntp.ntsc.ac.cn", ntpIP);

  buf[0] = 0xE3;
  buf[2] = 6;
  buf[3] = 0xEC;
  buf[12] = 49;
  buf[13] = 0x4E;
  buf[14] = 49;
  buf[15] = 52;

  Serial.println(F("Sending NTP packet"));
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
                   SEVENTY_YEARS;

  return epoch;
}

common::weather_data fetchWeatherData()
{
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

  String json = remote::getWeatherJsonStr(psk);
  if (json.length() == 0)
    return common::emptyData;

  DynamicJsonDocument doc(512);
  deserializeJson(doc, json);

  JsonObject result = doc["results"][0];
  const char *location = result["location"]["name"];

  JsonObject result_now = result["now"];
  const char *weather = result_now["text"];

  byte temp = atoi(result_now["temperature"]);

  return {location, weather, temp};
}
