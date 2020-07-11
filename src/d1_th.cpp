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

/* Example display:
    [  14:20, Yinchuan   ]
    [I -12C 30%H / O -10C]
    [Thunderstorm with ..]
    [.. Hail       07/11 ]
*/

struct
{
    tm time;
    String location;
    byte in_temp;
    byte in_humid;
    int dht_last_err;
    int online_temp;
    String weather;
} display_data;

char lines[LCD_ROWS][LCD_COLS + 1]; // 1 for line terminator

volatile bool update_weather = true;
volatile bool update_time = true;

void updateTime();
void updateWeather();

void setup()
{
    Serial.begin(115200);
    Serial.println();

    lcd.init();
    lcd.backlight();
    lcd.print(F("Initializing"));

    remote::init();
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
    lcd.printf_P(PSTR("  %02d:%02d %s"),
                 display_data.time.tm_min,
                 display_data.time.tm_sec,
                 display_data.location);

    lcd.setCursor(0, 1);
    lcd.printf_P(PSTR("I %3dC %2d%%H / O %3dC"),
                 display_data.in_temp,
                 display_data.in_humid,
                 display_data.online_temp);

    lcd.setCursor(0, 2);
    // todo
}

void updateTime()
{
    static const size_t BUF_LEN = 6;
    static char buf[BUF_LEN];
    time_t t;
    time(&t);
    // localtime() returns static variable address?
    tm *lt = localtime(&t);

    display_data.time = *lt;
}

void updateWeather()
{
    static const size_t LCD_BUF = 40;
    static char line[LCD_BUF];
    static char indoor[LCD_BUF];

    common::weather_data data = remote::getWeatherData();
    display_data.online_temp = data.temperature;
    display_data.location = data.location;
    display_data.weather = data.weather;

    byte temp, humid;
    display_data.dht_last_err = dht.read(&temp, &humid, nullptr);
    display_data.in_temp = temp;
    display_data.in_humid = humid;
}
