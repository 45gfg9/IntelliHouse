#include <Arduino.h>
#include <Ticker.h>
#include <SimpleDHT.h>
#include <LiquidCrystal_I2C.h>
#include "remote.hxx"

#define LCD_BUF 40

static const int DHT_PIN = 5;
SimpleDHT11 dht(DHT_PIN);

Ticker update;

LiquidCrystal_I2C lcd(0x27, 16, 2);

static const int update_s = 30;

int scroll_counter = 0;

void updateFun();

void setup()
{
    remote::connect();

    lcd.init();
    lcd.backlight();
    lcd.clear();

    update.attach(update_s, updateFun);
}

void loop()
{
    // .5s * 20 + 10s = 20s
    // 300s / 20s = 15
    delay(500);
    if ((scroll_counter % 40) == 0)
        delay(10000);
    lcd.scrollDisplayLeft();
}

void updateFun()
{
    static char line[LCD_BUF];
    static char indoor[LCD_BUF];

    byte temp, humid;
    if ((dht.read(&temp, &humid, nullptr)) == SimpleDHTErrSuccess)
    {
        snprintf_P(indoor, LCD_BUF, PSTR("%dC %d%%H"), temp, humid);
    }
    else
    {
        strncpy_P(indoor, PSTR("Error"), LCD_BUF);
    }

    common::weather_data data = remote::getWeatherData();

    snprintf_P(line, LCD_BUF, PSTR("Indoor %s; %s %dC"), indoor, data.location, data.temperature);

    scroll_counter = 0;
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print(line);
    lcd.setCursor(0, 1);
    lcd.print(data.weather);
}
