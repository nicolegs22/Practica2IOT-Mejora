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
    this->currentR = -1;
    this->currentG = -1;
    this->currentB = -1;
}

void Actuator::begin() {
    Serial.begin(115200);
    delay(1000);
    Serial.println();
    Serial.println("[ACTUADOR] Iniciando...");

    pinMode(pinR, OUTPUT);
    pinMode(pinG, OUTPUT);
    pinMode(pinB, OUTPUT);
    applyLedColor(0, 0, 0);
    
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
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);

    Serial.print("[WiFi] Conectando a ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.print("\n[WiFi] Conectado. IP del ESP32: ");
    Serial.println(WiFi.localIP());
    Serial.print("[WiFi] Gateway: ");
    Serial.println(WiFi.gatewayIP());
}

void Actuator::connectServer() {
    client.stop();

    Serial.print("[ACTUADOR] Conectando a servidor ");
    Serial.print(serverIP);
    Serial.print(":");
    Serial.println(serverPort);

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
        Serial.println("[ACTUADOR] Registrado con el servidor");
    } else {
        Serial.println("[ACTUADOR] Error de conexion");
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

        applyLedColor(r, g, b);

        sendConfirmation("leds");
    }
}

void Actuator::applyLedColor(int r, int g, int b) {
    if (r == currentR && g == currentG && b == currentB) {
        return;
    }

    digitalWrite(pinR, r > 0 ? HIGH : LOW);
    digitalWrite(pinG, g > 0 ? HIGH : LOW);
    digitalWrite(pinB, b > 0 ? HIGH : LOW);

    currentR = r;
    currentG = g;
    currentB = b;

    Serial.print("[ACTUADOR] LED fijo RGB: ");
    Serial.print(r);
    Serial.print(",");
    Serial.print(g);
    Serial.print(",");
    Serial.println(b);
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
