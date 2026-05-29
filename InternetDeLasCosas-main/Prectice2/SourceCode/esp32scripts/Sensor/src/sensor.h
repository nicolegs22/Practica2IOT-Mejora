#ifndef SENSOR_H
#define SENSOR_H

#include <WiFi.h>
#include <ArduinoJson.h>

class Sensor {
private:
    const char* ssid;
    const char* password;
    const char* serverIP;
    int serverPort;

    WiFiClient client;

    int trigPin;
    int echoPin;

public:
    Sensor(const char* ssid, const char* password, const char* ip, int port, int trig, int echo);

    void begin();
    void loop();

private:
    void connectWiFi();
    void connectServer();
    float measureDistance();
};

#endif
