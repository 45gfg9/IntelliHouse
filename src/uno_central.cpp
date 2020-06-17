#include <Arduino.h>
#include <Wire.h>
// #include "climate.hxx"
#include "task.hxx"

void setup()
{
  Wire.begin(2);

  // task::getTime();
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
