#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "remote.hxx"

#define PSK_STRLEN 18

ESP8266WebServer server(80);

char psk[PSK_STRLEN] = {};

uint32_t getTime()
{
  Wire.beginTransmission(IIC);
  Wire.write('T');
  Wire.endTransmission();

  Wire.requestFrom(IIC, 4);
  uint32_t time = 0;
  time |= (byte)Wire.read();
  time |= (byte)Wire.read() << 8;
  time |= (byte)Wire.read() << 16;
  time |= (byte)Wire.read() << 24;

  Serial.println(time);

  return time;
}

common::dht_data getTH() {
  Wire.beginTransmission(IIC);
  Wire.write('D');
  Wire.endTransmission();

  Wire.requestFrom(IIC, 2);
  byte temp = Wire.read();
  byte humid = Wire.read();

  return {temp, humid};
}

void setup()
{
  delay(1000);
  Serial.begin(115200);

  EEPROM.begin(PSK_STRLEN);
  for (int i = 0; i < PSK_STRLEN; i++)
    psk[i] = EEPROM.read(i);
  EEPROM.end();
  Serial.println();
  Serial.println(psk);

  remote::begin();
  Serial.println(remote::AP_ip);

  server.on("/weather", []() {
    const char *host = ("api.seniverse.com");
    const char *uri = ("/v3/weather/now.json");
    const char *partialQuery = ("?location=WQ7SF3WEPQRU&key=");

    WiFiClient client;
    if (!client.connect(host, 80))
    {
      server.send(500, "text/plain", "connection failed");
      client.stop();
      return;
    }

    client.printf_P(PSTR("GET %s%s%s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n"), uri, partialQuery, psk, host);

    unsigned long timeout = millis();
    while (client.available() == 0)
    {
      if (millis() - timeout > 5000)
      {
        server.send(504, "text/plain", "Connection timed out");
        client.stop();
        return;
      }
    }
    delay(500);

    String data('{');
    while (client.available())
    {
      if (client.read() == '{')
        data += client.readStringUntil('}');
    }
    data += '}';

    Serial.println(data);
    server.send(200, "text/plain", data);
  });

  server.on("/time", []() {
    uint32_t time = getTime();

    server.send(200, "text/plain", String(time));
  });

  server.on("/th", []() {
    common::dht_data data = getTH();

    String t(data.temp);
    String h(data.humid);

    server.send(200, "text/plain", t + ' ' + h);
  });

  server.begin();
}

void loop()
{
  server.handleClient(); // non-blocking
}
