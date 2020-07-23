#include <Arduino.h>
#include <Hash.h>
#include <WiFiUdp.h>
#include "remote.hxx"
#include <Keypad.h>

const byte MAX_PASS_PER_MIN = 8;

WiFiUDP udp;

char keymap[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'},
};
byte rowPins[4] = {2, 3, 4, 5};
byte colPins[4] = {9, 8, 7, 6};
Keypad keypad(makeKeymap(keymap), rowPins, colPins, 4, 4);

String getRandPass(uint32_t seed);
String getRandPass(uint32_t seed, byte nth);
void checkInput();
bool verifyPass(const String &str);
void handleResult(bool success);

void setup()
{
    Serial.begin(BAUD_RATE);
    Serial.println();

    remote::connect();

    udp.begin(UDP_PORT);
}

void loop()
{
    remote::listenTime(udp);

    checkInput();
}

String getRandPass(uint32_t seed)
{
    static uint32_t prev_seed = 0;
    static byte counter = 0;

    if (prev_seed != seed)
    {
        prev_seed = seed;
        counter = 0;
    }

    if (counter >= MAX_PASS_PER_MIN)
        counter = 0;

    return getRandPass(seed, counter++);
}

String getRandPass(uint32_t seed, byte nth)
{
    static const char map[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                               '8', '9', 'A', 'B', 'C', 'D'};

    srand(seed);
    for (int i = 0; i < nth; i++)
        rand();

    char otp[6];
    uint8_t hash[20];
    sha1(String(rand()), hash);

    for (int i = 0; i < 5; i++)
        otp[i] = map[hash[hash[i] % 3 + i * 3 + 5] % 14];
    otp[5] = 0;

    return String(otp);
}

void checkInput()
{
    static String str;
    char c = keypad.getKey();
    if (!c)
        return;
    Serial.print(c);
    switch (c)
    {
    case '#':
        Serial.println();
        if (str.length())
            handleResult(verifyPass(str));
        else
            break;

    case '*':
        Serial.println(F("..Cancel"));
        str.clear();
        break;

    default:
        str += c;
        break;
    }
}

bool verifyPass(const String &str)
{
    if (str.length() != 5)
        return false;

    // time(nullptr) returns time_t, eliminating
    // the need to create a local variable
    uint32_t now_min = time(nullptr) / 60;
    for (int i = 0; i < 3; i++)
        for (byte j = 0; j < MAX_PASS_PER_MIN; j++)
            if (getRandPass(now_min - i, j).equals(str))
                return true;

    return false;
}

void handleResult(bool success)
{
    if (success)
    {
    }
    else
    {
    }
}
