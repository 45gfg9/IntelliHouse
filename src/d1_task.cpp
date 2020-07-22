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
    uint8_t y, m, d, hh, mm, ss;
    time_t ut() const;

public:
    DateTime(const String &date, const String &time);
    DateTime(const DateTime &copy);

    String toStr() const;

    bool operator<(const DateTime &rhs) const { return ut() < rhs.ut(); }
    bool operator>(const DateTime &rhs) const { return ut() > rhs.ut(); }
    bool operator<=(const DateTime &rhs) const { return ut() <= rhs.ut(); }
    bool operator>=(const DateTime &rhs) const { return ut() >= rhs.ut(); }
};

struct Task
{
    TaskType task;
    DateTime time;
    String remark;

    bool operator<(const Task &rhs) const { return time < rhs.time; }
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

time_t DateTime::ut() const
{
    // need validation
    static const uint8_t daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    static const time_t SECONDS_FROM_1970_TO_2000 = 946684800L;

    uint8_t y = this->y - 2000;
    uint16_t d = this->d;
    for (int i = 0; i < m - 1; i++)
        d += daysInMonth[i];
    if (m > 2 && y % 4 == 0)
        d++;
    d += 365 * y + (y + 3) / 4 - 1;

    return SECONDS_FROM_1970_TO_2000 + ((d * 24L + hh) * 60 + mm) * 60 + ss;
}

DateTime::DateTime(const String &date, const String &time)
{
    // Example: 2020/07/22 16:55:22

    y = date.toInt();
    m = date.substring(5).toInt();
    d = date.substring(8).toInt();
    hh = time.toInt();
    mm = time.substring(3).toInt();
    ss = time.substring(6).toInt();
}

DateTime::DateTime(const DateTime &copy)
{
    y = copy.y;
    m = copy.m;
    d = copy.d;
    hh = copy.hh;
    mm = copy.mm;
    ss = copy.ss;
}

String DateTime::toStr() const
{
    static char buf[20];
    snprintf_P(buf, 20, PSTR("%04d/%02d/%02d %02d:%02d:%02d"),
               y, m, d, hh, mm, ss);
    return String(buf);
}
