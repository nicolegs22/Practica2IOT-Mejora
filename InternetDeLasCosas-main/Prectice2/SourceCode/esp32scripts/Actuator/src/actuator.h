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
    int currentR, currentG, currentB;

public:
    Actuator(const char* ssid, const char* password, const char* ip, int port,
             int r, int g, int b);

    void begin();
    void loop();

private:
    void connectWiFi();
    void connectServer();
    void processCommand(String message);
    void applyLedColor(int r, int g, int b);
    void sendConfirmation(String command);
};

#endif
