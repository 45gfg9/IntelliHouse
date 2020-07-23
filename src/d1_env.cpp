#include <Arduino.h>
#include "remote.hxx"

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

void setup()
{
    Serial.begin(BAUD_RATE);
    Serial.println();

    remote::connect();
}

void loop()
{
}
