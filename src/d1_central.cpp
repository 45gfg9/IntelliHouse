#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include "remote.hxx"

#define PSK_STRLEN 18

char psk[PSK_STRLEN] = {};

ESP8266WebServer server(80);
WiFiUDP ntpUdp;
NTPClient ntp(ntpUdp, "ntp.ntsc.ac.cn");

common::weather_data fetchWeatherData();
uint32_t fetchNTPTime();

void setup()
{
  delay(1000);
  Serial.begin(115200);
  Wire.begin();

  EEPROM.begin(PSK_STRLEN);
  for (int i = 0; i < PSK_STRLEN; i++)
    psk[i] = EEPROM.read(i);
  EEPROM.end();
  Serial.println();
  Serial.println(psk);

  remote::begin();

  server.on("/", [&]() {
    Serial.println(F("Pong!"));
    server.send(200, "text/plain", "Pong!");
  });

  server.on("/weather", [&]() {
    Serial.println(F("Fetching weather"));
    common::weather_data weather = fetchWeatherData();

    String data = String(weather.location) + ',' + weather.weather + ',' + weather.temperature;

    server.send(200, "text/plain", data);
  });

  server.on("/time", [&]() {
    Serial.println(F("Fetching time"));
    Wire.requestFrom(IIC, 4);

    uint32_t time = common::read_uint32_t(Wire, LSBFIRST);

    server.send(200, "text/plain", String(time));
  });

  server.on("/ntp", [&]() {
    Serial.println(F("Fetching NTP time"));

    uint32_t epoch = fetchNTPTime();

    server.send(200, "text/plain", String(epoch));
  });

  ntp.begin();
  server.begin();

  Wire.beginTransmission(IIC);
  common::write_uint32_t(Wire, LSBFIRST, fetchNTPTime());
  Wire.endTransmission();
}

void loop()
{
  server.handleClient(); // non-blocking
}

uint32_t fetchNTPTime()
{
  ntp.update();

  return ntp.getEpochTime();
}

common::weather_data fetchWeatherData()
{
  String json = remote::getWeatherJsonStr(psk);

  DynamicJsonDocument doc(512);

  deserializeJson(doc, json);

  JsonObject result = doc["results"][0];
  const char *location = result["location"]["name"];

  JsonObject result_now = result["now"];
  const char *weather = result_now["text"];

  byte temp = atoi(result_now["temperature"]);

  return {location, weather, temp};
}
