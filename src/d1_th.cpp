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

SimpleDHT11 dht(DHT_PIN);
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);
Ticker weather_ticker;
Ticker time_ticker;

int scroll_counter = 0;

bool update_weather = false;
bool update_time = false;

String epoch2str(uint32_t t);
void updateTime();
void updateWeather();

void setup()
{
    delay(1000);
    Serial.begin(115200);
    Serial.println();

    lcd.init();
    lcd.backlight();
    lcd.print("Initializing");

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
    // .75s * 40 + 30s = 60s
    // 300s / 60s = 5
    // delay(750);
    // if ((scroll_counter++ % 40) == 0)
    //     delay(30000);
    // lcd.scrollDisplayLeft();
}

String epoch2str(uint32_t t)
{
    static const uint32_t SECONDS_FROM_1970_TO_2000 = 946684800UL;
    static const byte DAYS_IN_MONTH[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    static char buf[20];

    t -= SECONDS_FROM_1970_TO_2000;
    byte sec = t % 60;
    t /= 60;
    byte min = t % 60;
    t /= 60;
    byte hr = t % 24;
    unsigned int day = t / 24;
    byte month;
    byte year;
    bool leap;
    for (year = 0;; year++)
    {
        leap = year % 4 == 0;
        if (day < 365 + leap)
            break;
        day -= 365 + leap;
    }

    for (month = 1;; month++)
    {
        uint8_t daysPerMonth = daysPerMonth + month - 1;
        if (leap && month == 2)
            daysPerMonth++;
        if (day < daysPerMonth)
            break;
        day -= daysPerMonth;
    }
    day++;

    snprintf_P(buf, 20, PSTR("%04d/%02d/%02d %02d:%02d:%02d"),
               year, month, day, hr, min, sec);

    return String(buf);
}

void updateTime()
{
    uint32_t epoch = remote::getTime();
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

    scroll_counter = 0;
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print(line);
    lcd.setCursor(0, 1);
    lcd.print(data.weather);
}
