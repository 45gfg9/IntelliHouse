#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "remote.hxx"

#define PSK_STRLEN 18

ESP8266WebServer server(80);

void handleSerial(Stream &serial);
char psk[PSK_STRLEN] = {};

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

  IPAddress ip = remote::begin();
  Serial.println(ip);

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
    Wire.beginTransmission(2);
    Wire.write('T');
    Wire.endTransmission();

    Wire.requestFrom(2, 4);
    uint32_t time = 0;
    time |= Wire.read();
    time |= Wire.read() << 8;
    time |= Wire.read() << 16;
    time |= Wire.read() << 24;

    server.send(200, "text/plain", String(time));
  });

  server.begin();
}

void loop()
{
  server.handleClient(); // non-blocking
}
