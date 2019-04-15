/*
*	Class to handle digital IO for the sensor
*
*	Author: Josh Perry <josh.perry245@gmail.com>
*	Copyright 2019
*/

#include "GDoorIO.hpp"

void GDoorIO::initializeGPIOPins() {
    Serial.println("Setting GPIO Pins to correct IO state");
    pinMode(doorSensorPin, INPUT);
    pinMode(hardResetPin, INPUT);
    pinMode(wifiLEDPin, OUTPUT);
    digitalWrite(wifiLEDPin, LOW);
    // pinMode(relayPin, OUTPUT);
    // digitalWrite(relayPin, LOW);
    
    // Set all unused pins to high Z
    for (char pin : unusedPins) {
        pinMode(pin, INPUT);
    }
}