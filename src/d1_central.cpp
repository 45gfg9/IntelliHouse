#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include "remote.hxx"

#define PSK_STRLEN 18

SoftwareSerial ss(2, 3);
ESP8266WebServer server(80);

void handleSerial(Stream &serial);
char psk[18] = {};

void setup()
{
  delay(1000);
  Serial.begin(115200);
  ss.begin(115200);

  EEPROM.begin(18);
  for (int i = 0; i < PSK_STRLEN; i++)
    psk[i] = EEPROM.read(i);
  EEPROM.end();

  IPAddress ip = remote::begin();
  Serial.println(ip);

  server.on("/weather", []() {
    const uint8_t url_len = 100;
    const char *host = PSTR("api.seniverse.com");
    const char *uri = PSTR("/v3/weather/now.json");
    const char *incompleteQuery = PSTR("?location=WQ7SF3WEPQRU&key=");
    char url[url_len];
    snprintf_P(url, url_len, PSTR("%s%s%s%s"), host, uri, incompleteQuery, psk);

    WiFiClient client;
    if (!client.connect(url, 443))
    {
      server.send(500, "text/plain", "connection failed");
      client.stop();
      return;
    }

    if (client.connected())
    {
      client.printf_P(PSTR("GET %s%s%s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n"), uri, incompleteQuery, psk, host);
    }

    unsigned long timeout = millis();
    while (client.available() == 0)
    {
      if (millis() - timeout > 5000)
      {
        server.send(504, "text/plain", "connection timed out");
        client.stop();
        return;
      }
    }

    const int available = client.available();
    char data[available];
    for (int i = 0; i < available; i++)
    {
      data[i] = client.read();
    }

    server.send(200, "text/plain", data);
  });
  server.begin();
}

void loop()
{
  server.handleClient(); // non-blocking

  handleSerial(ss);
}

void handleSerial(Stream &serial)
{
  while (serial.available())
  {
    serial.read();
  }
}
