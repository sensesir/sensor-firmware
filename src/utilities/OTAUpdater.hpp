/*
*	Extension of open-source ESP8266httpUpdater 
*   class (in Arduino core for ESP8266)
*
*	Author: Josh Perry <josh.perry245@gmail.com>
*	Created: 07/16/2019
*   Copyright 2019
*/

#ifndef OTAUPDATER_H
#define OTAUPDATER_H

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include "../config/Configuration.h"
#include "../digital-io/GDoorIO.hpp"
#include "../../secrets/AWSIoTSecrets.h"
#include "../lib/ArduinoJSON/ArduinoJson.h"
#include <StreamString.h>
#include <vector>
#include <string>

#define USE_SERIAL Serial  // Seems a bit unnecessary, but library wanted to do this...

#define DEBUG_HTTP_UPDATE Serial.printf

/*
 *  Design note: The class seems to only make use of class-level methods. No 
 */

class OTAUpdater: private ESP8266HTTPUpdate {
    public:
        void updateFirmware(char* updateInfo); 

    protected:
        HTTPUpdateResult handleUpdate(HTTPClient& http, const char* build, const char* version);
        bool streamToFlash(Stream& in, uint32_t size, String md5, int command);
    
    private:
        std::vector<const char*>* deserializeOTAPayload(char* payload);
        bool validDistributonRequest(const char* build, const char* version);
};

#endif
