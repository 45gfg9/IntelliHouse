#include <Arduino.h>
#include <queue>
#include <LittleFS.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <DateTime.h>
#include "remote.h"

enum TaskType {
  TV,
  BATH,
};

struct Task {
  TaskType task;
  DateTime time;
  String remark;

  bool operator<(const Task &rhs) const { return time < rhs.time; }
  bool operator>(const Task &rhs) const { return time > rhs.time; }
};

WiFiUDP udp;
ESP8266WebServer server;
std::priority_queue<Task, std::vector<Task>, std::greater<Task>> pq;

void setup() {
  Serial.begin(BAUD_RATE);
  Serial.println();
  remote::connect();
  remote::mDNSsetup("d1_task");

  Serial.println(LittleFS.begin() ? F("LittleFS Begin") : F("LittleFS Fail"));

  server.on("/addTask", HTTP_POST, [&]() {
    String arg_type = server.arg("task");
    String arg_date = server.arg("date");
    String arg_time = server.arg("time");
    String arg_remark = server.arg("remark");

    Task task = {
        (TaskType)arg_type.toInt(), DateTime(arg_date, arg_time), arg_remark};
    pq.push(task);

    server.send(201);
  });

  udp.begin(UDP_PORT);

  server.begin();
}

void loop() {
  server.handleClient();
}
