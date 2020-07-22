#include <Arduino.h>
#include <queue>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
// Temporary workaround
// TODO replace
#include <DateTime.h>
#include "remote.hxx"

enum TaskType
{
    TV,
    BATH,
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
        // TODO

        server.send(200);
    });
}

void loop()
{
    server.handleClient();
}
