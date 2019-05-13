/*
*	Class to handle digital IO for the sensor
*
*	Author: Josh Perry <josh.perry245@gmail.com>
*	Copyright 2019
*/

#ifndef GDoorIO_h
#define DGoorIO_h

// Includes
#include <Arduino.h>
#include <vector>
#include <string>
#include "../config/Configuration.h"

class GDoorIO{
    public:
        // GPIO Pins 
        const int doorSensorPin = DOOR_SENSOR;
        const int relayPin 	  = RELAY_OUT;
        const int wifiLEDPin 	  = WIFI_LED;
        const int hardResetPin  = HARD_RESET;
        const int pulseLength   = ACTUATION_PULSE_LENGTH;		

        // State
        struct Door GDoor;

        // Unused exposed pins
        std::vector<char> unusedPins = {0, 2, 3, 13, 14, 15, 16};
	
        // Methods
        void initialize();
        void actuateDoor();

    private:
        void initializeState();
        void initializeGPIOPins();
        void updateDoorState(); 
};

#endif