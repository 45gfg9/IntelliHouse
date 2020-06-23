#include <Arduino.h>
#include <Ticker.h>
#include <SimpleDHT.h>
#include <LiquidCrystal_I2C.h>
#include "remote.hxx"

const int interval = 30;

static const int DHT_PIN = 5;
SimpleDHT11 dht(DHT_PIN);

Ticker update;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void updateFun();

void setup()
{
    remote::connect();

    lcd.init();
    lcd.backlight();
    lcd.clear();

    update.attach(interval, updateFun);
}

void loop()
{
}

void updateFun() {
}
