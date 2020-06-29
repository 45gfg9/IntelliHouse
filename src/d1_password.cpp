#include <Arduino.h>
#include <Hash.h>
#include "remote.hxx"
#include <Keypad.h>

char keymap[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'},
};

byte rowPins[4] = {2, 3, 4, 5};
byte colPins[4] = {9, 8, 7, 6};

Keypad keypad(makeKeymap(keymap), rowPins, colPins, 4, 4);

String getRandPass(uint32_t n);
void checkInput();
bool verifyPass(const String &str);
void handleResult(bool success);

void setup()
{
    remote::connect();
}

void loop()
{
    checkInput();
}

String getPass(uint32_t n)
{
    static const char map[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                               '8', '9', 'A', 'B', 'C', 'D'};

    char otp[6];
    uint8_t hash[20];
    sha1(String(n), hash);

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
        Serial.println();
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

    uint32_t this_min = remote::getTime() / 60,
             prev_min = this_min - 1,
             prev_prev_min = prev_min - 1;

    return (str == getPass(this_min) ||
            str == getPass(prev_min) ||
            str == getPass(prev_prev_min));
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
