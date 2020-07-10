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

tm epoch2str(uint32_t t)
{
    static const uint32_t SECONDS_FROM_1970_TO_2020 = 1577750400UL;
    static const byte DAYS_IN_MONTH[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    tm ret;

    t -= SECONDS_FROM_1970_TO_2020;
    ret.tm_sec = t % 60;
    t /= 60;
    ret.tm_min = t % 60;
    t /= 60;
    ret.tm_hour = t % 24;
    int day = t / 24;
    bool leap;

    // TODO return struct tm so we can use strftime()

    // for (ret.tm_year = 2020; day > 365 + leap; ret.tm_year++)
    // {
    //     leap = year % 4 == 0;
    //     if (day < 365 + leap)
    //         break;
    //     day -= 365 + leap;
    // }

    // for (month = 1;; month++)
    // {
    //     uint8_t daysPerMonth = DAYS_IN_MONTH[month - 1];
    //     if (leap && month == 2)
    //         daysPerMonth++;
    //     if (day < daysPerMonth)
    //         break;
    //     day -= daysPerMonth;
    // }

    return ret;
}

void updateTime()
{
    // TODO
    uint32_t epoch = remote::getTime();

    tm time = epoch2str(epoch);

    char buf[41];
    strftime(buf, 40, "%R", &time);
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
}
