/*
 *  Constants and configuration for Firmware development
 *  that occurs on the NodeMCU ESP8266 dev board 
 * 
 *  Author: Josh Perry <josh.perry245@gmail.com>
 *  Created: 04/13/2019
 *  Copyright: 2019
 */

#ifndef DEVNODEMCUCONFIG_H
#define DEVNODEMCUCONFIG_H

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

// Time zone constants
const int8_t TIME_ZONE = 1;            // -6: Colorado | Cali: -7 | Ireland +1
// #define USE_SUMMER_TIME_DST

// Networking
const char AWS_IOT_DEVICE_GATEWAY[] = "anwaqu8y2zf77-ats.iot.eu-west-1.amazonaws.com";
const char SENSOR_UID[] = "SensorJPDev";

// MQTT Topic Pubsub
// Subscribe
const char SUB_ACTUATE[] = "gdoor/SensorJPDev/command/actuate";
const char SUB_HEALTH_PING[] = "gdoor/SensorJPDev/event/health";
// std::vector<const char*> SUBSCRIBE_TOPICS = {SUB_ACTUATE, SUB_HEALTH_PING}; // {SUB_ACTUATE, SUB_HEALTH_PING};

// Publish
const char PUB_OPEN[]      = "server/SensorJPDev/event/open";
const char PUB_CLOSE[]     = "server/SensorJPDev/event/close";
const char PUB_BOOT[]      = "server/SensorJPDev/event/boot";
const char PUB_RECONNECT[] = "server/SensorJPDev/event/reconnect";
const char PUB_HEALTH[]    = "server/SensorJPDev/event/health";
const char PUB_ERROR[]     = "server/SensorJPDev/event/error";

#endif