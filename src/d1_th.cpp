#include <Arduino.h>
#include <Ticker.h>
#include <SimpleDHT.h>
#include <LiquidCrystal_I2C.h>
#include "remote.hxx"

static const int DHT_PIN = 5;

static const byte LCD_ADDR = 0x27;
static const byte LCD_COLS = 16;
static const byte LCD_ROWS = 2;

static const int UPDATE_S = 30;

SimpleDHT11 dht(DHT_PIN);
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);
Ticker update;

int scroll_counter = 0;

void updateFun();

void setup()
{
    delay(1000);
    Serial.begin(115200);
    Serial.println();

    remote::connect();

    lcd.init();
    lcd.backlight();
    lcd.clear();

    update.attach(UPDATE_S, updateFun);
}

void loop()
{
    // TODO Change the way it displays

    // .75s * 40 + 30s = 60s
    // 300s / 60s = 5
    delay(750);
    if ((scroll_counter++ % 40) == 0)
        delay(30000);
    lcd.scrollDisplayLeft();
}

void updateFun()
{
    static const size_t LCD_BUF = 40;
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

    // FIXME connection to gateway board fails
    common::weather_data data = remote::getWeatherData();

    snprintf_P(line, LCD_BUF, PSTR("Indoor %s; %s %dC"), indoor, data.location.c_str(), data.temperature);

    scroll_counter = 0;
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print(line);
    lcd.setCursor(0, 1);
    lcd.print(data.weather);
}
