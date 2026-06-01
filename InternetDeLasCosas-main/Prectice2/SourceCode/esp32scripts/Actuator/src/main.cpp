#include "actuator.h"

Actuator actuator(
    "iPhone de Nicky",
    "nicole12345",
    "172.20.10.4",
    5000,
    32, 25, 26
);

void setup() {
    actuator.begin();
}

void loop() {
    actuator.loop();
}
