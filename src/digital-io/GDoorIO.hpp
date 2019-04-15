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
#include "../config/Configuration.h"

class GDoorIO{
    public: 
        int doorSensorPin = DOOR_SENSOR;
        int relayPin 	  = RELAY_OUT;
        int wifiLEDPin 	  = WIFI_LED;
        int hardResetPin  = HARD_RESET;
        int pulseLength   = ACTUATION_PULSE_LENGTH;		

        // Unused exposed pins
        std::vector<char> unusedPins = {0, 2, 3, 13, 14, 15, 16};
	
        void initializeGPIOPins();
};

#endif