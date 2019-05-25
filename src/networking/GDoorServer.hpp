/*
*	Script to implement an ephemenral local server. 
*   Intended for the use of communicating with a local 
*   client to acquire wifi credentials and distribute 
*   sensor UID.
*
*	Author: Josh Perry <josh.perry245@gmail.com> 
*	Copyright 2019
*/

#ifndef GDOORSERVER_H
#define GDOORSERVER_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "ESP8266WebServer.h"
#include <ESP8266mDNS.h>
#include "../../secrets/AWSIoTSecrets.h"
#include "../lib/ArduinoJSON/ArduinoJson.h"
#include "../config/Configuration.h"
#include "../digital-io/GDoorIO.hpp"
#include "../models/SensorModel.hpp"

void initialiseSensorLocally();

#endif

