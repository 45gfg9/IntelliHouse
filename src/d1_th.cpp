#include <Arduino.h>
#include <Ticker.h>
#include <SimpleDHT.h>
#include <LiquidCrystal_I2C.h>
#include "remote.hxx"

static const int DHT_PIN = D5;

static const byte LCD_ADDR = 0x27;
static const byte LCD_COLS = 20;
static const byte LCD_ROWS = 4;

static const int UPDATE_S = 30;

SimpleDHT11 dht(DHT_PIN);
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);
Ticker update;

int scroll_counter = 0;

String epoch2str(uint32_t t);
void updateFun();

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

    // update.attach(UPDATE_S, updateFun); // FIXME CULPRIT
}

void loop()
{
    // TODO rewrite

    // .75s * 40 + 30s = 60s
    // 300s / 60s = 5
    // delay(750);
    // if ((scroll_counter++ % 40) == 0)
    //     delay(30000);
    // lcd.scrollDisplayLeft();
    // updateFun();
    // delay(30000);
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

    snprintf_P(buf, 20, PSTR("%d/%d/%d %d:%d:%d"),
               year, month, day, hr, min, sec);

    return String(buf);
}

void updateFun()
{
    static const size_t LCD_BUF = 40;
    static char line[LCD_BUF];
    static char indoor[LCD_BUF];

    uint32_t epoch = remote::getTime();
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
