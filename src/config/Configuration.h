/*
*	High-level wrapper to include correct environment config
*
*	Author: Josh Perry <josh.perry245@gmail.com>
*	Created: 04/13/2019
*   Copyright 2019
*/

#ifndef Constants
#define Constants

#define PROD 0
#define DEV 1
#define NODE_DEV 2

// Set ENV here
#define ENV NODE_DEV

#if ENV == PROD
    #include "./ProdConfig.h"
#elif ENV == DEV
    #include "./DevConfig.h"
#elif ENV == NODE_DEV
    #include "./DevNodeMCUConfig.h"
#else
    #error No ENV defined
#endif

/** 
 *                    Generic constants here
 *    All constants that are env-agnostic should be defined below
 */

const char FIRMWARE_VERSION[] = "0.2.0";

// Networking
const char KEY_SSID[] = "wifiSSID";
const char KEY_WIFI_PASSWORD[] = "wifiPassword";

// MQTT Topic Pubsub
// General topic use
const char SERVER[]  = "server";
const char SENSOR[]  = "gdoor";
const char EVENT[]   = "event";
const char COMMAND[] = "command";
const char TOPIC_DELIMITER[] = "/";

// Subscribe
const char SUB_ACTUATE[]     = "actuate";
const char SUB_HEALTH_PING[] = "health";

// Publish
const char PUB_FIRST_BOOT[] = "firstBoot";
const char PUB_BOOT[]       = "boot";
const char PUB_DOOR_STATE[] = "doorStateChange";
const char PUB_RECONNECT[]  = "reconnect";
const char PUB_HEALTH[]     = "health";
const char PUB_ERROR[]      = "error";

// Other events
const char EVENT_MQTT_CONN_ERROR[] = "mqttConFailure";

// Payload keys
const char KEY_SENSOR_UID[]       = "sensorUID";
const char KEY_EVENT[]            = "event";
const char KEY_FIRMWARE_VERSION[] = "firmwareVersion";
const char KEY_STATE[]            = "state";
const char KEY_MESSAGE[]          = "message";
const char KEY_DURATION[]         = "duration";

const char DOOR_STATE_OPEN_STR[]    = "open";
const char DOOR_STATE_CLOSED_STR[]  = "closed";
const char DOOR_STATE_UNKNOWN_STR[] = "unknown";

// Time constants
#define LED_FLASH_DELAY 500         // ms
#define SENSOR_FILTER_DELAY 50      // ms  [check average noise frequency and compare]
#define RECONNECT_DELAY_MQTT 2500
#define SETUP_COOL_DOWN 2000

// Number constants
#define MQTT_RECON_MAX 20

#endif