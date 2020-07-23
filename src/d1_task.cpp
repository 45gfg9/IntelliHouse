#include <Arduino.h>
#include <queue>
#include <LittleFS.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
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
    DateTime(uint16_t y, uint8_t m, uint8_t d, uint8_t hh, uint8_t mm, uint8_t ss);
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

WiFiUDP udp;
ESP8266WebServer server; // default port 80
std::priority_queue<Task, std::vector<Task>, std::greater<Task>> pq;

void setup()
{
    Serial.begin(BAUD_RATE);
    Serial.println();
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

    udp.begin(UDP_PORT);

    server.begin();
}

void loop()
{
    server.handleClient();
}

time_t DateTime::ut() const
{
    static const uint8_t DAYS_IN_MONTH[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    static const time_t SECONDS_FROM_1970_TO_2000 = 946684800L;

    uint8_t y = this->y;
    uint16_t d = this->d;
    for (int i = 0; i < m - 1; i++)
        d += DAYS_IN_MONTH[i];
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

DateTime::DateTime(uint16_t y, uint8_t m, uint8_t d,
                   uint8_t hh, uint8_t mm, uint8_t ss) : y(y < 2000 ? y : y - 2000), m(m), d(d),
                                                         hh(hh), mm(mm), ss(ss) {}

DateTime::DateTime(const DateTime &copy) : y(copy.y), m(copy.m), d(copy.d),
                                           hh(copy.hh), mm(copy.mm), ss(copy.ss) {}

String DateTime::toStr() const
{
    static char buf[20];
    snprintf_P(buf, 20, PSTR("20%02d/%02d/%02d %02d:%02d:%02d"),
               y, m, d, hh, mm, ss);
    return String(buf);
}
