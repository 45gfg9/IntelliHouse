#include <Arduino.h>
#include <queue>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include "remote.hxx"

enum TaskType
{
    TV,
    BATH,
};

class DateTime
{
    time_t t;

public:
    DateTime(const String &date, const String &time);
    DateTime(const DateTime &copy);
    String toStr();

    bool operator<(const DateTime &rhs) const { return t < rhs.t; }
    bool operator>(const DateTime &rhs) const { return t > rhs.t; }
    bool operator<=(const DateTime &rhs) const { return t <= rhs.t; }
    bool operator>=(const DateTime &rhs) const { return t >= rhs.t; }
};

struct Task
{
    TaskType task;
    DateTime time;
    String remark;

    bool operator>(const Task &rhs) const { return time > rhs.time; }
};

ESP8266WebServer server; // default port 80
std::priority_queue<Task, std::vector<Task>, std::greater<Task>> pq;

void setup()
{
    remote::connect();

    Serial.println(LittleFS.begin() ? F("LittleFS Begin") : F("LittleFS Fail"));

    server.on("/addTask", [&]() {
        if (server.method() != HTTP_POST)
        {
            server.send(405);
            return;
        }

        String arg_type = server.arg("task");
        String arg_date = server.arg("date");
        String arg_time = server.arg("time");
        String arg_remark = server.arg("remark");

        Task task = {(TaskType)arg_type.toInt(), DateTime(arg_date, arg_time), arg_remark};
        pq.push(task);

        server.send(201);
    });
}

void loop()
{
    server.handleClient();
}

// number of days since 2000/01/01, valid for 2001..2099
static uint16_t date2days(uint16_t y, uint8_t m, uint8_t d)
{
    static const uint8_t daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (y >= 2000)
        y -= 2000;
    uint16_t days = d;
    for (uint8_t i = 1; i < m; i++)
        days += daysInMonth[i - 1];
    if (m > 2 && y % 4 == 0)
        days++;
    return days + 365 * y + (y + 3) / 4 - 1;
}

static long time2long(uint16_t days, uint8_t h, uint8_t m, uint8_t s)
{
    return ((days * 24L + h) * 60 + m) * 60 + s;
}

DateTime::DateTime(const String &date, const String &time)
{
    // Example: 2020/07/22 16:55:2
    // TODO change store method
    const static time_t SECONDS_FROM_1970_TO_2000 = 946684800L;

    uint8_t y = date.toInt();
    uint8_t m = date.substring(5).toInt();
    uint8_t d = date.substring(8).toInt();
    uint8_t hh = time.toInt();
    uint8_t mm = time.substring(3).toInt();
    uint8_t ss = time.substring(6).toInt();

    t = SECONDS_FROM_1970_TO_2000;
    t += time2long(date2days(y, m, d), hh, mm, ss);
}

DateTime::DateTime(const DateTime &copy)
{
    // TODO change store method
    t = copy.t;
}

String DateTime::toStr()
{
    static char buf[20];
    // TODO

    return String(buf);
}
