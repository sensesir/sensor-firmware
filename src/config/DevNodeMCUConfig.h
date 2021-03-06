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
#define NETWORK_LED_RED 12
#define NETWORK_LED_GREEN 13
#define NETWORK_LED_BLUE 15
#define DOOR_SENSOR 5
#define RELAY_OUT 4
#define NETWORK_MODE 14

// Time constants
#define ACTUATION_PULSE_LENGTH 1500     // miliseconds
#define MQTT_CONN_TIMEOUT 180           // seconds

// Misc
#define SENSOR_UID_LENGTH 36            // E.g: 073f04a4-0b9b-4a00-9b11-89709a3ccd1f

// Time zone constants
const int8_t TIME_ZONE = 1;            // -6: Colorado | Cali: -7 | Ireland +1
// #define USE_SUMMER_TIME_DST

// Networking
const char SERVER_UID[] = "eu-west-1.dev";
const char AWS_IOT_DEVICE_GATEWAY[] = "anwaqu8y2zf77-ats.iot.eu-west-1.amazonaws.com";
const char MQTT_CONN_ERR_ENDPOINT[] = "http://dm071t6ydt31y.cloudfront.net";
const char API_KEY[] = "8sBkjUhVQ9lZP3uigrpB3e2C66dmtaSaJwC3Mmy7";
const char OTA_UPDATE_ENDPOINT[] = "http://ec2-34-245-168-70.eu-west-1.compute.amazonaws.com:3000/otaUpdate";
const char OTA_API_KEY[] = "878ed9d6-8729-4883-a93f-2ceb10b643ce";
const int serverPort = 80;

// Conditional WiFi setting
// #define USE_DEV_MODEL_DATA

#ifdef USE_DEV_MODEL_DATA
const char WIFI_SSID[] = "BRIGNETI";
const char WIFI_PASSWORD[] = "12345678";
#endif

#endif