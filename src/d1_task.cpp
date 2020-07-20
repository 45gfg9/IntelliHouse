#include <Arduino.h>
#include <list>
#include <ESP8266WebServer.h>
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
};

ESP8266WebServer server; // default port 80
std::list<Task> task_list;

void setup()
{
    remote::connect();

    server.on("/addTask", [&]() {
        // TODO

        server.send(200);
    });
}

void loop()
{
}
