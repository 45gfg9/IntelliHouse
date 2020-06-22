#include <Arduino.h>
#include <Ticker.h>
#include "remote.hxx"

const int interval = 5 * 60;

Ticker update;

void updateFun();

void setup()
{
    remote::connect();

    update.attach(interval, updateFun);
}

void loop()
{
}

void updateFun() {
}
