#include "sensor.h"

Sensor sensor(
    "LIZ SAAVEDRA 5G",
    "melina5186804",
    "192.168.0.4",
    5000,
    5,   // trig
    18   // echo
);

void setup() {
    sensor.begin();
}

void loop() {
    sensor.loop();
}
