#include <Arduino.h>
#include <Ticker.h>
#include <SimpleDHT.h>
#include <LiquidCrystal_I2C.h>
#include "remote.hxx"

const int interval = 5 * 60;

static const int DHT_PIN = 5;
SimpleDHT11 dht(DHT_PIN);

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
