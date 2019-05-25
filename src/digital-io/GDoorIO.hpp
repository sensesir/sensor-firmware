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

// Door state
typedef enum doorState {
    DOOR_STATE_UNKNOWN = -1,
    DOOR_STATE_OPEN = 0,
    DOOR_STATE_CLOSED,
} DoorState;

struct Door {
    DoorState state;
    char stateString[8];
};

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
        struct Door gdoor;

        // Unused exposed pins
        // Reserved pins: 1, 3 [Tx, Rx]
        std::vector<char> unusedPins = {0, 16};
	
        // Methods
        void initialize();
        void actuateDoor();
        bool doorStateChanged();

        // LED states
        void networkLEDOff();
        void networkLEDSetYellow();
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

        void initializeGPIOPins();
        void updateDoorState();    
        void setStateToOpen();
        void setStateToClosed();
        void setStateToUnknown();
};

#endif