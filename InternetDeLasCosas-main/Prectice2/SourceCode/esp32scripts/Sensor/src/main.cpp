#include "sensor.h"

Sensor sensor(
   "iPhone de Nicky",
    "nicole12345",
    "172.20.10.4",
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
