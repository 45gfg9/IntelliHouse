#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "remote.hxx"

#define PSK_STRLEN 18

ESP8266WebServer server(80);

char psk[PSK_STRLEN] = {};

common::weather_data fetchWeatherData();

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
  Serial.println(remote::AP_ip);

  server.on("/", []() {
    Serial.println("Pong!");
    server.send(200, "text/plain", "Pong!");
  });

  server.on("/weather", []() {
    Serial.println("Fetching weather");
    common::weather_data weather = fetchWeatherData();

    String data = String(weather.location) + ',' + weather.weather + ',' + weather.temperature;

    server.send(200, "text/plain", data);
  });

  server.on("/time", []() {
    Serial.println("Fetching time");
    Wire.requestFrom(IIC, 4);

    uint32_t time = 0;
    time |= (byte)Wire.read();
    time |= (byte)Wire.read() << 8;
    time |= (byte)Wire.read() << 16;
    time |= (byte)Wire.read() << 24;

    server.send(200, "text/plain", String(time));
  });

  server.begin();
}

void loop()
{
  server.handleClient(); // non-blocking
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
