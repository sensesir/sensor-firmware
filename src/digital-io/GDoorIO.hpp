/*
*	Class to handle digital IO for the sensor
*
*	Author: Josh Perry <josh.perry245@gmail.com>
*	Copyright 2019
*/

#ifndef GDoorIO_h
#define GDoorIO_h

// Includes
#include <Arduino.h>
#include <vector>
#include <string>
#include "../config/Configuration.h"

class GDoorIO {
    public:
        static GDoorIO& getInstance();

        // GPIO Pins 
        const int networkLEDRed   = NETWORK_LED_RED;
        const int networkLEDGreen = NETWORK_LED_GREEN;
        const int networkLEDBlue  = NETWORK_LED_BLUE;
        const int doorSensorPin   = DOOR_SENSOR;
        const int relayDriverPin  = RELAY_OUT;	

        // State
        struct Door GDoor;

        // Unused exposed pins
        std::vector<char> unusedPins = {0, 2, 3, 16};
	
        // Methods
        void initialize();
        void actuateDoor();

        // LED states
        void networkLEDOff();
        void networkLEDSetWhite();
        void networkLEDToggleWhite();
        void networkLEDFSetPurple();
        void networkLEDTogglePurple();
        void networkLEDSetCyan();
        void networkLEDToggleCyan();
        void networkLEDSetBlue();
        void networkLEDToggleBlue();
        void networkLEDSetRed();

    private:   
        // Singleton handling
        GDoorIO();
        GDoorIO(GDoorIO const&);              // Don't Implement
        void operator=(GDoorIO const&);       // Don't implement

        void initializeState();
        void initializeGPIOPins();
        void updateDoorState();         
};

#endif