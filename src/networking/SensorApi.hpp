/*
*	Wrapper class to work with the OSS HTTP library for an ESP8266  
*   that uses the Arduino SDK
*
*	Author: Josh Perry <josh.perry245@gmail.com> 
*	Copyright 2019
*/

#include <Esp.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <hardwareSerial.h>
#include "../lib/ArduinoJSON/ArduinoJson.h"
#include "../../secrets/AWSIoTSecrets.h"

#include <Stream.h>
#include <ESP8266httpUpdate.h>

#include <sstream>
#include <iomanip>

#define USE_SERIAL Serial

void postMQTTConnError();
void otaUpdate();
void otaUpdateV2();

