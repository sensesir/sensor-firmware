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
    pinMode(networkLEDRed, OUTPUT);
    pinMode(networkLEDGreen, OUTPUT);
    pinMode(networkLEDBlue, OUTPUT);
    this->networkLEDOff();

    pinMode(doorSensorPin, INPUT);
    pinMode(relayDriverPin, OUTPUT);
    digitalWrite(relayDriverPin, LOW);
    
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

void GDoorIO::actuateDoor() {
    // TODO:
}

// LED management

void GDoorIO::networkLEDOff() {
    digitalWrite(networkLEDRed, LOW);
    digitalWrite(networkLEDGreen, LOW);
    digitalWrite(networkLEDBlue, LOW);    
}

void GDoorIO::networkLEDSetWhite() {
    this->networkLEDOff();
    digitalWrite(networkLEDRed, HIGH);
    digitalWrite(networkLEDGreen, HIGH);
    digitalWrite(networkLEDBlue, HIGH);
} 

void GDoorIO::networkLEDFlashWhite() {
    this->networkLEDSetWhite();

    while(true) {
        delay(LED_FLASH_DELAY);
        int state = digitalRead(networkLEDRed);
        digitalWrite(networkLEDRed, !state);
        digitalWrite(networkLEDGreen, !state);
        digitalWrite(networkLEDBlue, !state);
    }
}

void GDoorIO::networkLEDFSetPurple(){
    this->networkLEDOff();
    digitalWrite(networkLEDRed, HIGH);
    digitalWrite(networkLEDBlue, HIGH);
}

void GDoorIO::networkLEDFlashPurple() {
    this->networkLEDOff();
    this->networkLEDFlashPurple();

    while(true) {
        delay(LED_FLASH_DELAY);
        int state = digitalRead(networkLEDRed);
        digitalWrite(networkLEDRed, !state);
        digitalWrite(networkLEDBlue, !state);
    }
}

void GDoorIO::networkLEDSetBlue() {
    this->networkLEDOff();
    digitalWrite(networkLEDBlue, HIGH);
}

void GDoorIO::networkLEDFlashBlue(){
    this->networkLEDOff();
    this->networkLEDSetBlue();

    while(true) {
        delay(LED_FLASH_DELAY);
        int state = digitalRead(networkLEDBlue);
        digitalWrite(networkLEDBlue, !state);
    }
}

