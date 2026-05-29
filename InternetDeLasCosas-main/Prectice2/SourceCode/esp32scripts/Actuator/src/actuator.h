#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <WiFi.h>
#include <ArduinoJson.h>

class Actuator {
private:
    const char* ssid;
    const char* password;
    const char* serverIP;
    int serverPort;

    WiFiClient client;

    int pinR, pinG, pinB;

public:
    Actuator(const char* ssid, const char* password, const char* ip, int port,
             int r, int g, int b);

    void begin();
    void loop();

private:
    void connectWiFi();
    void connectServer();
    void processCommand(String message);
    void sendConfirmation(String command);
};

#endif
