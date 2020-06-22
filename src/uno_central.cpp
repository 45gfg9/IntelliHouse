#include <Arduino.h>
#include <Wire.h>
// #include "climate.hxx"
#include "task.hxx"

enum action
{
  TIME,
  DHT11,
  NONE
} act;

void setup()
{
  Wire.begin(2);

  task::begin();

  Wire.onReceive([](int n) {
    switch (Wire.read())
    {
    case 'T':
      act = TIME;
      break;
    case 'D':
      act = DHT11;
      break;
    }
  });

  Wire.onRequest([]() {
    switch (act)
    {
    case TIME:
      uint32_t time = task::getTime().unixtime();

      Wire.write(time);
      Wire.write(time >> 8);
      Wire.write(time >> 16);
      Wire.write(time >> 24);
      break;

    case DHT11:
      break;
    }
    act = NONE;
  });
}

void loop()
{
  // handleSerial(ss);
}

void handleSerial(Stream &serial)
{
  while (serial.available())
  {
  }
}
