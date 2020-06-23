#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "remote.hxx"

#define PSK_STRLEN 18

ESP8266WebServer server(80);

char psk[PSK_STRLEN] = {};

void setup()
{
  // delay(1000);
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

  server.on("/weather", []() {
    server.send(200, "text/plain", remote::getWeatherJsonStr(psk));
  });

  server.on("/time", []() {
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
