#include "actuator.h"

Actuator actuator(
    "LIZ SAAVEDRA 5G",
    "melina5186804",
    "192.168.0.4",
    5000,
    32, 25, 26
);

void setup() {
    actuator.begin();
}

void loop() {
    actuator.loop();
}
