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

const char FIRMWARE_VERSION[] = "0.1.0";

// MQTT Topic Pubsub
// General topic use
const char SERVER[]  = "server";
const char SENSOR[]  = "gdoor";
const char EVENT[]   = "event";
const char COMMAND[] = "command";

// Subscribe
const char SUB_ACTUATE[]     = "actuate";
const char SUB_HEALTH_PING[] = "health";

// Publish
const char PUB_FIRST_BOOT[] = "firstBoot";
const char PUB_BOOT[]       = "boot";
const char PUB_OPEN[]       = "open";
const char PUB_CLOSE[]      = "close";
const char PUB_RECONNECT[]  = "reconnect";
const char PUB_HEALTH[]     = "health";
const char PUB_ERROR[]      = "error";

// Payload keys
const char KEY_SENSOR_UID[]       = "sensorUID";
const char KEY_EVENT[]            = "event";
const char KEY_FIRMWARE_VERSION[] = "firmwareVersion";

#endif