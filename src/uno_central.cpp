#include <Arduino.h>
#include <Wire.h>
// #include "climate.hxx"
#include "task.hxx"

void setup()
{
  Wire.begin(2);

  task::begin();

  Wire.onReceive([](int n) {
    if (Wire.read() == 'T') {
      uint32_t time = task::getTime().unixtime();

      Wire.write(time & 0xFF);
      Wire.write(time & 0xFF00);
      Wire.write(time & 0xFF0000);
      Wire.write(time & 0xFF000000);
    }
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
