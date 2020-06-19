#include <Arduino.h>
#include <Hash.h>
#include "remote.hxx"
#include <Keypad.h>
#include "task.hxx"

char keymap[4][4] = {
    {'7', '8', '9', 'A'},
    {'4', '5', '6', 'B'},
    {'1', '2', '3', 'C'},
    {'*', '0', '#', 'D'},
};

byte rowPins[4] = {2, 3, 4, 5};
byte colPins[4] = {9, 8, 7, 6};

Keypad keypad(makeKeymap(keymap), rowPins, colPins, 4, 4);

void getRandPass(char out[5]);

void setup()
{
    remote::connect();
}

void loop()
{
}

void getRandPass(char out[6])
{
    static const char map[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                               '8', '9', 'A', 'B', 'C', 'D'};

    uint32_t time = task::getTime().unixtime();
    char tchr[10];
    uint8_t hash[20];
    snprintf(tchr, 10, "%u", time);
    sha1(tchr, strlen(tchr), hash);

    for (int i = 0; i < 5; i++)
        out[i] = map[hash[hash[i] % 3 + i * 3 + 5] % 14];
    out[5] = 0;
}
