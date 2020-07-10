#include <Arduino.h>
#include <SimpleDHT.h>
#include <LiquidCrystal_I2C.h>
#include <TickerScheduler.h>
#include "remote.hxx"

#define DHT_PIN D5
#define LCD_ADDR 0x27
#define LCD_COLS 20
#define LCD_ROWS 4
#define WEATHER_INTERVAL 30
#define TIME_INTERVAL 10
#define UTC_8HR 28800

// D1 = SCL
// D2 = SDA

SimpleDHT11 dht(DHT_PIN);
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);
Ticker weather_ticker;
Ticker time_ticker;

String lines[LCD_ROWS];

volatile bool update_weather = true;
volatile bool update_time = true;

tm epoch2str(uint32_t t);
void updateTime();
void updateWeather();

void setup()
{
    delay(1000);
    Serial.begin(115200);
    Serial.println();

    lcd.init();
    lcd.backlight();
    lcd.print(F("Initializing"));

    remote::connect();
    lcd.clear();

    time_t t = remote::getTime() + 1; // +1s to make up for network latency
    timeval tv = {t + UTC_8HR, 0};
    settimeofday(&tv, nullptr);

    time_ticker.attach(TIME_INTERVAL, [&]() { update_time = true; });
    weather_ticker.attach(WEATHER_INTERVAL, [&]() { update_weather = true; });
}

void loop()
{
    if (update_time)
    {
        updateTime();
        update_time = false;
    }
    if (update_weather)
    {
        updateWeather();
        update_time = false;
    }

    lcd.clear();
    for (int i = 0; i < 4; i++)
    {
        lcd.setCursor(0, i);
        lcd.print(lines[i]);
    }
}

void updateTime()
{
    time_t t;
    time(&t);
    // dynamic allocated, may cause memory leak!
    tm *lt = localtime(&t);

    char buf[LCD_COLS];
    strftime(buf, LCD_COLS, "%m-%d %R", lt);

    delete lt;

    // tbc
}

void updateWeather()
{
    static const size_t LCD_BUF = 40;
    static char line[LCD_BUF];
    static char indoor[LCD_BUF];

    common::weather_data data = remote::getWeatherData();

    byte temp, humid;
    if ((dht.read(&temp, &humid, nullptr)) == SimpleDHTErrSuccess)
    {
        snprintf_P(indoor, LCD_BUF, PSTR("%dC %d%%H"), temp, humid);
    }
    else
    {
        strncpy_P(indoor, PSTR("Error"), LCD_BUF);
    }

    snprintf_P(line, LCD_BUF, PSTR("Indoor %s; %s %dC"), indoor, data.location.c_str(), data.temperature);

    // tbc
}
