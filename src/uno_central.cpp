#include <Arduino.h>
#include <SoftwareSerial.h>
#include "climate.hxx"
#include "task.hxx"

SoftwareSerial ss(2, 3);

void setup()
{
  ss.begin(115200);

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
