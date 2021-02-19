#include <Arduino.h>
#include <WiFiUdp.h>
#include <Adafruit_NeoPixel.h>
#include <DateTime.h>
#include "remote.h"

#define LED_COUNT 30
#define LED_PIN D0

/*
Time

on
 6:00 -  9:00 Yellow
 9:00 - 12:00 Blue
12:00 - 12:30 Orange
12:30 - 13:30 Green
13:30 - 17:30 Blue
17:30 - 18:30 Orange
18:30 - 22:00 Magenta
off (but manual)
*/

WiFiUDP udp;
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN);

void setup() {
  Serial.begin(BAUD_RATE);
  Serial.println();

  strip.begin();
  strip.show();

  remote::connect();

  udp.begin(UDP_PORT);
}

void loop() {
  remote::listenTime(udp);
}
