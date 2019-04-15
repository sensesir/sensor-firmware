/*
 *  Constants and configuration for Firmware development
 *  that occurs on the NodeMCU ESP8266 dev board 
 * 
 *  Author: Josh Perry <josh.perry245@gmail.com>
 *  Created: 04/13/2019
 *  Copyright: 2019
 */

// Chip config
#define BAUD_RATE 115200

/* 
 *              GPIO
 * 
 *  Special Pins:
 *      a) 01: TX Serial
 *      b) 03: RX Serial
 */
#define WIFI_LED 5
#define DOOR_SENSOR 14
#define RELAY_OUT 4
#define HARD_RESET 12

// Time constants
#define ACTUATION_PULSE_LENGTH 1500     // miliseconds