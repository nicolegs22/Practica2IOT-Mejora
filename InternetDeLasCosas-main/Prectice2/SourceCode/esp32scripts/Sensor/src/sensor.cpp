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
    delay(1000);
    Serial.println();
    Serial.println("[SENSOR] Iniciando...");

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
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);

    Serial.print("[WiFi] Conectando a ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }

    Serial.print("\n[WiFi] Conectado. IP del ESP32: ");
    Serial.println(WiFi.localIP());
    Serial.print("[WiFi] Gateway: ");
    Serial.println(WiFi.gatewayIP());
}

void Sensor::connectServer() {
    client.stop();

    Serial.print("[SENSOR] Conectando a servidor ");
    Serial.print(serverIP);
    Serial.print(":");
    Serial.println(serverPort);

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
        Serial.println("[SENSOR] Registrado con el servidor");
    } else {
        Serial.println("[SENSOR] Error de conexion");
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
