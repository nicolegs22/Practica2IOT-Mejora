#include "actuator.h"

Actuator::Actuator(const char* ssid, const char* password, const char* ip, int port,
                   int r, int g, int b) {
    this->ssid = ssid;
    this->password = password;
    this->serverIP = ip;
    this->serverPort = port;
    this->pinR = r;
    this->pinG = g;
    this->pinB = b;
}

void Actuator::begin() {
    Serial.begin(115200);
    pinMode(pinR, OUTPUT);
    pinMode(pinG, OUTPUT);
    pinMode(pinB, OUTPUT);
    
    connectWiFi();
    connectServer();
}

void Actuator::loop() {
    if (!client.connected()) {
        connectServer();
        delay(1000);
        return;
    }

    if (client.available()) {
        String message = client.readStringUntil('\n');
        processCommand(message);
    }
    
    delay(10);
}

void Actuator::connectWiFi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n[WiFi] Connected");
}

void Actuator::connectServer() {
    if (client.connect(serverIP, serverPort)) {
        StaticJsonDocument<384> doc;
        doc["message_type"] = "register";
        doc["id"] = "ESP32_ACTUADOR_01";
        doc["device_type"] = "actuator";
        // Legacy fields keep compatibility with older server builds.
        doc["tipo"] = "registro";
        doc["dispositivo_tipo"] = "actuador";

        String json;
        serializeJson(doc, json);
        client.println(json);
        Serial.println("[ACTUATOR] Registered with server");
    } else {
        Serial.println("[ACTUATOR] Connection error");
    }
}

void Actuator::processCommand(String message) {
    StaticJsonDocument<512> doc;
    if (deserializeJson(doc, message)) return;

    String messageType = doc["message_type"] | "";
    if (messageType.length() == 0) {
        messageType = doc["tipo"] | "";
    }

    String command = doc["command"] | "";
    if (command.length() == 0) {
        command = doc["comando"] | "";
    }

    // Accept legacy command messages from older server builds.
    if ((messageType == "command" || messageType == "comando") && command == "leds") {
        int r = doc["rgb"][0];
        int g = doc["rgb"][1];
        int b = doc["rgb"][2];
        int duration = doc["duration"].is<int>() ? doc["duration"].as<int>() : doc["duracion"].as<int>();

        analogWrite(pinR, r);
        analogWrite(pinG, g);
        analogWrite(pinB, b);
        
        delay(duration);
        
        analogWrite(pinR, 0);
        analogWrite(pinG, 0);
        analogWrite(pinB, 0);

        sendConfirmation("leds");
    }
}

void Actuator::sendConfirmation(String command) {
    StaticJsonDocument<384> doc;
    doc["message_type"] = "command_response";
    doc["id"] = "ESP32_ACTUADOR_01";
    doc["command"] = command;
    doc["result"] = "ok";
    // Legacy fields keep compatibility with older server builds.
    doc["tipo"] = "comando_respuesta";
    doc["comando"] = command;
    doc["resultado"] = "ok";

    String json;
    serializeJson(doc, json);
    client.println(json);
}
