#include "sensor.h"

Sensor::Sensor(const char* ssid, const char* password, const char* ip, int port, int trig, int echo) {
    this->ssid = ssid;
    this->password = password;
    this->serverIP = ip;
    this->serverPort = port;
    this->trigPin = trig;
    this->echoPin = echo;
}

void Sensor::begin() {
    Serial.begin(115200);

    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);

    connectWiFi();
    connectServer();
}

void Sensor::loop() {
    if (!client.connected()) {
        Serial.println("Reconnecting to server...");
        connectServer();
    }

    float distance = measureDistance();

    StaticJsonDocument<384> doc;
    doc["message_type"] = "sensor_data";
    doc["id"] = "ESP32_SENSOR_01";
    doc["distance"] = distance;
    // Legacy fields keep compatibility with older server builds.
    doc["tipo"] = "sensor_data";
    doc["distancia"] = distance;

    String json;
    serializeJson(doc, json);
    client.println(json);

    Serial.println(distance);

    delay(1000);
}

void Sensor::connectWiFi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
}

void Sensor::connectServer() {
    if (client.connect(serverIP, serverPort)) {
        StaticJsonDocument<384> doc;
        doc["message_type"] = "register";
        doc["id"] = "ESP32_SENSOR_01";
        doc["device_type"] = "sensor";
        // Legacy fields keep compatibility with older server builds.
        doc["tipo"] = "registro";
        doc["dispositivo_tipo"] = "sensor";

        String json;
        serializeJson(doc, json);
        client.println(json);
    }
}

float Sensor::measureDistance() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH);
    return duration * 0.034 / 2;
}
