/*
*	Class to handle digital IO for the sensor
*
*	Author: Josh Perry <josh.perry245@gmail.com>
*	Copyright 2019
*/

#include "GDoorIO.hpp"

void GDoorIO::initialize() {
    this->initializeGPIOPins();
    this->initializeState();
}

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

void GDoorIO::initializeState() {
    strcpy(this->GDoor.state, DOOR_STATE_UNKNOWN);
}

void GDoorIO::updateDoorState() {
    // Filter signal for full second to remove noisy readings
    int count = 0;
	for (int i = 0; i < 10; i++) {
		if (digitalRead(this->doorSensorPin) == HIGH){
			strcpy(this->GDoor.state, DOOR_STATE_OPEN);
            return;
		}

		delay(100);
	}

    strcpy(this->GDoor.state, DOOR_STATE_CLOSED);
}