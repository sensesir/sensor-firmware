/*
*	Class to handle digital IO for the sensor
*
*	Author: Josh Perry <josh.perry245@gmail.com>
*	Copyright 2019
*/

#include "GDoorIO.hpp"

// Singleton handling
GDoorIO& GDoorIO::getInstance() {
    static GDoorIO instance;
    return instance;
}

GDoorIO::GDoorIO() {}

void GDoorIO::initialize() {
    this->initializeGPIOPins();
    this->setStateToUnknown();
}

void GDoorIO::initializeGPIOPins() {
    Serial.println("Setting GPIO Pins to correct IO state");
    pinMode(networkLEDRed, OUTPUT);
    pinMode(networkLEDGreen, OUTPUT);
    pinMode(networkLEDBlue, OUTPUT);
    this->networkLEDOff();

    pinMode(doorSensorPin, INPUT);
    pinMode(networkModePin, INPUT);
    pinMode(relayDriverPin, OUTPUT);
    digitalWrite(relayDriverPin, LOW);
    
    // Set all unused pins to high Z
    for (char pin : unusedPins) {
        pinMode(pin, INPUT);
    }
}

bool GDoorIO::doorStateChanged() {
    DoorState oldState = this->gdoor.state;     // Ensure deep copy
    this->updateDoorState();

    if (this->gdoor.state != oldState) { return true; }
    else { return false; }
}

void GDoorIO::updateDoorState() {
    // Filter signal for full second to remove noisy readings
    int count = 0;
	for (int i = 0; i < 10; i++) {
		if (digitalRead(this->doorSensorPin) == LOW){
			this->setStateToOpen();
            return;
		}

		delay(SENSOR_FILTER_DELAY);
	}

    this->setStateToClosed();
}

void GDoorIO::setStateToOpen() {
    this->gdoor.state = DOOR_STATE_OPEN;
    strcpy(this->gdoor.stateString, DOOR_STATE_OPEN_STR);
}

void GDoorIO::setStateToClosed() {
    this->gdoor.state = DOOR_STATE_CLOSED;
    strcpy(this->gdoor.stateString, DOOR_STATE_CLOSED_STR);
}

void GDoorIO::setStateToUnknown() {
    this->gdoor.state = DOOR_STATE_UNKNOWN;
    strcpy(this->gdoor.stateString, DOOR_STATE_UNKNOWN_STR);
}

void GDoorIO::actuateDoor() {
    Serial.println("DOOR IO: Actuating door");
    digitalWrite(relayDriverPin, HIGH);
    delay(ACTUATION_PULSE_LENGTH);
    digitalWrite(relayDriverPin, LOW);
}

// See readme for motivation as to why an interrupt is not used
void GDoorIO::assessModePin() {
    bool modePinState = digitalRead(this->networkModePin);
    if (modePinState == HIGH) {
        return;
    }
    
    int count = 0;
    int maxLoop = ((MODE_RESET_HOLD + 1)*1000)/SENSOR_FILTER_DELAY;
    for (int i = 0; i < maxLoop; i++) {    
        if (digitalRead(this->networkModePin) == LOW) {
            break;
        }

        count ++;
        delay(SENSOR_FILTER_DELAY);
    }

    if (count > ((MODE_RESET_HOLD * 1000)/SENSOR_FILTER_DELAY)) {
        // Factory reset - [More functionality in future]
        Serial.println("GDOORIO: Mode held for > 30s. Factory reset.");
        SensorModel::getInstance().clearEEPROM();
        ESP.restart();
        return;
    }

    if (count > ((MODE_AP_POINT_HOLD * 1000)/SENSOR_FILTER_DELAY)) {
        Serial.println("GDOORIO: Mode held for ~5s. Restarting.");
        SensorModel::getInstance().clearEEPROM();
        ESP.restart();
        return;
    }
}

// LED management

void GDoorIO::networkLEDOff() {
    digitalWrite(networkLEDRed, LOW);
    digitalWrite(networkLEDGreen, LOW);
    digitalWrite(networkLEDBlue, LOW);    
}

void GDoorIO::networkLEDSetYellow() {
    this->networkLEDOff();
    digitalWrite(networkLEDRed, HIGH);
    digitalWrite(networkLEDGreen, HIGH);
    digitalWrite(networkLEDBlue, LOW);
} 

void GDoorIO::networkLEDSetWhite() {
    this->networkLEDOff();
    digitalWrite(networkLEDRed, HIGH);
    digitalWrite(networkLEDGreen, HIGH);
    digitalWrite(networkLEDBlue, HIGH);
} 

void GDoorIO::networkLEDToggleWhite() {
    int state = digitalRead(networkLEDRed);
    digitalWrite(networkLEDRed, !state);
    digitalWrite(networkLEDGreen, !state);
    digitalWrite(networkLEDBlue, !state);
}

void GDoorIO::networkLEDFSetPurple(){
    this->networkLEDOff();
    digitalWrite(networkLEDRed, HIGH);
    digitalWrite(networkLEDBlue, HIGH);
}

void GDoorIO::networkLEDTogglePurple() {
    int state = digitalRead(networkLEDRed);
    digitalWrite(networkLEDRed, !state);
    digitalWrite(networkLEDBlue, !state);
}

void GDoorIO::networkLEDSetCyan() {
    this->networkLEDOff();
    digitalWrite(networkLEDGreen, HIGH);
    digitalWrite(networkLEDBlue, HIGH);
}

void GDoorIO::networkLEDToggleCyan() {
    int state = digitalRead(networkLEDGreen);
    digitalWrite(networkLEDGreen, !state);
    digitalWrite(networkLEDBlue, !state);
}

void GDoorIO::networkLEDSetBlue() {
    this->networkLEDOff();
    digitalWrite(networkLEDBlue, HIGH);
}

void GDoorIO::networkLEDToggleBlue(){
    int state = digitalRead(networkLEDBlue);
    digitalWrite(networkLEDBlue, !state);
}

void GDoorIO::networkLEDSetRed() {
    this->networkLEDOff();
    digitalWrite(networkLEDRed, HIGH);
}

