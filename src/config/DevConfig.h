/*
 *  Constants and configuration for Firmware development
 *  that occurs on the custom board design for the 
 *  garage sensor
 * 
 *  Author: Josh Perry <josh.perry245@gmail.com>
 *  Created: 04/15/2019
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

// Misc
// #define SENSOR_UID_LENGTH 36            // E.g: 073f04a4-0b9b-4a00-9b11-89709a3ccd1f

// Time zone constants
const int8_t TIME_ZONE = +2;            // UTC+2 => JHB
// #define USE_SUMMER_TIME_DST

// Networking
const char AWS_IOT_DEVICE_GATEWAY[] = "anwaqu8y2zf77-ats.iot.eu-west-1.amazonaws.com";
const char SERVER_UID[] = "eu-west-1.dev";