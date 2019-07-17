/*
*	Extension of open-source ESP8266httpUpdater 
*   class (in Arduino core for ESP8266)
*
*	Author: Josh Perry <josh.perry245@gmail.com>
*	Created: 07/16/2019
*   Copyright 2019
*/

#include "OTAUpdater.hpp"

// Also used by external lib needed for Spiff size compute
extern "C" uint32_t _FS_start;
extern "C" uint32_t _FS_end;

/*
 *    Wrapper for the high-level function: update()
 *    Will simply invoke that function with the log handling on the output
 */

void OTAUpdater::updateFirmware(char* updateInfo) {
    // Give some feedback
    Serial.println("OTA UPDATE: Updating sensor firmware");
    GDoorIO::getInstance().networkLEDSetWhite();
    unsigned long startTime = millis();

    // Extract update info
    std::vector<const char*> *firmwareDistInfo = this->deserializeOTAPayload(updateInfo);    
    const char* build = (*firmwareDistInfo)[0];
    const char* version = (*firmwareDistInfo)[1];
    if (!validDistributonRequest(build, version)) {
        Serial.printf("OTA UPDATE: Error, invalid firmware build: %s version: %s \n", build, version);
        return;
    }

    // Create reference to TCP client (poorly named as WiFiClient)
    Serial.printf("OTA UPDATE: Updating to firmware build: %s version: %s\n", build, version);
    WiFiClient client;
    HTTPClient http;
    http.begin(client, OTA_UPDATE_ENDPOINT);

    t_httpUpdate_return result = handleUpdate(http, build, version);

    // To use base class method (doesn't allow customization): 
    // t_httpUpdate_return result = ESPhttpUpdate.update(client, OTA_UPDATE_ENDPOINT);

    switch (result) {
      case HTTP_UPDATE_FAILED:
        USE_SERIAL.printf("OTA UPDATE: Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        USE_SERIAL.println("OTA UPDATE: No updates required");
        break;

      case HTTP_UPDATE_OK:
        double elapsedTime = (millis() - startTime)/1000.0;
        USE_SERIAL.printf("OTA UPDATE: Successful update in %4.2f s, rebooting...", elapsedTime);
        ESP.restart();
        break;
    }
}

HTTPUpdateResult OTAUpdater::handleUpdate(HTTPClient& http, const char* build, const char* version) {
    // Default should be to fail
    HTTPUpdateResult ret = HTTP_UPDATE_FAILED;

    // use HTTP/1.0 for update since the update handler not support any transfer Encoding
    http.useHTTP10(true);
    http.setTimeout(8000);                          // Default - 8000 ? | Could need to change
    // http.setFollowRedirects(false);                 // Default - changed
    http.setUserAgent(F("ESP8266-http-Update"));
    http.addHeader(F("x-ESP8266-STA-MAC"), WiFi.macAddress());
    http.addHeader(F("x-ESP8266-AP-MAC"), WiFi.softAPmacAddress());
    http.addHeader(F("x-ESP8266-free-space"), String(ESP.getFreeSketchSpace()));
    http.addHeader(F("x-ESP8266-sketch-size"), String(ESP.getSketchSize()));
    http.addHeader(F("x-ESP8266-sketch-md5"), String(ESP.getSketchMD5()));
    http.addHeader(F("x-ESP8266-mode"), F("sketch"));

    // Custom headers: Auth 
    http.addHeader("x-api-key", OTA_API_KEY);
    http.addHeader("sensor-uid", SENSOR_UID);           // Goes against our naming convention, but applies to header style
    http.addHeader("firmware-build", build);
    http.addHeader("firmware-version", version);

    const char * headerkeys[] = { "x-MD5" };
    size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);

    // track these headers
    http.collectHeaders(headerkeys, headerkeyssize);

    int code = http.GET();
    int len = http.getSize();

    if(code <= 0) {
        DEBUG_HTTP_UPDATE("[httpUpdate] HTTP error: %s\n", http.errorToString(code).c_str());
        _lastError = code;
        http.end();
        return HTTP_UPDATE_FAILED;
    }

    DEBUG_HTTP_UPDATE("[httpUpdate] Header read fin.\n");
    DEBUG_HTTP_UPDATE("[httpUpdate] Server header:\n");
    DEBUG_HTTP_UPDATE("[httpUpdate]  - code: %d\n", code);
    DEBUG_HTTP_UPDATE("[httpUpdate]  - len: %d\n", len);

    if(http.hasHeader("x-MD5")) {
        DEBUG_HTTP_UPDATE("[httpUpdate]  - MD5: %s\n", http.header("x-MD5").c_str());
    }

    if(http.hasHeader("x-MD5")) {
        DEBUG_HTTP_UPDATE("[httpUpdate]  - MD5: %s\n", http.header("x-MD5").c_str());
    }

    DEBUG_HTTP_UPDATE("[httpUpdate] ESP8266 info:\n");
    DEBUG_HTTP_UPDATE("[httpUpdate]  - free Space: %d\n", ESP.getFreeSketchSpace());
    DEBUG_HTTP_UPDATE("[httpUpdate]  - current Sketch Size: %d\n", ESP.getSketchSize());

    switch(code) {
    case HTTP_CODE_OK:  ///< OK (Start Update)
        
        if(len > 0) {    
            bool startUpdate = true;
            if (len > (int) ESP.getFreeSketchSpace()) {
                DEBUG_HTTP_UPDATE("[httpUpdate] FreeSketchSpace to low (%d) needed: %d\n", ESP.getFreeSketchSpace(), len);
                startUpdate = false;
            }
            
            if (!startUpdate) {
                _lastError = HTTP_UE_TOO_LESS_SPACE;
                ret = HTTP_UPDATE_FAILED;
            } 
            
            else {
                // Serial.println("OTA UPDATE: Starting bin file stream");
                WiFiClient *tcp = http.getStreamPtr();

                WiFiUDP::stopAll();
                WiFiClient::stopAllExcept(tcp);

                delay(100);

                int command;
                command = U_FLASH;
                DEBUG_HTTP_UPDATE("[httpUpdate] runUpdate flash...\n");

                uint8_t buf[4];
                if (tcp->peekBytes(&buf[0], 4) != 4) {
                    DEBUG_HTTP_UPDATE("[httpUpdate] peekBytes magic header failed\n");
                    _lastError = HTTP_UE_BIN_VERIFY_HEADER_FAILED;
                    http.end();
                    return HTTP_UPDATE_FAILED;
                }

                // check for valid first magic byte
                if (buf[0] != 0xE9) {
                    DEBUG_HTTP_UPDATE("[httpUpdate] Magic header does not start with 0xE9\n");
                    _lastError = HTTP_UE_BIN_VERIFY_HEADER_FAILED;
                    http.end();
                    return HTTP_UPDATE_FAILED;
                }

                uint32_t bin_flash_size = ESP.magicFlashChipSize((buf[3] & 0xf0) >> 4);

                // check if new bin fits to SPI flash
                if (bin_flash_size > ESP.getFlashChipRealSize()) {
                    DEBUG_HTTP_UPDATE("[httpUpdate] New binary does not fit SPI Flash size\n");
                    _lastError = HTTP_UE_BIN_FOR_WRONG_FLASH;
                    http.end();
                    return HTTP_UPDATE_FAILED;
                }
                
                // All checks pass - starting stream into flash mem
                Serial.println("OTA UPDATE: Starting byte stream into flash memory");
                if (streamToFlash(*tcp, len, http.header("x-MD5"), command)) {
                    Serial.println("OTA UPDATE: Completed firmware acquisition and stream to flash");
                    ret = HTTP_UPDATE_OK;
                    http.end();
                    return ret;
                } 
                
                else {
                    ret = HTTP_UPDATE_FAILED;
                    DEBUG_HTTP_UPDATE("OTA UPDATE: Update failed, likely causes (1) Invalid boot mode (reset ESP) (2) Corrupt bin file");
                }
            }
        } else {
            _lastError = HTTP_UE_SERVER_NOT_REPORT_SIZE;
            ret = HTTP_UPDATE_FAILED;
            DEBUG_HTTP_UPDATE("[httpUpdate] Content-Length was 0 or wasn't set by Server?!\n");
        }
        break;
    
    case HTTP_CODE_NOT_MODIFIED:
        ///< Not Modified (No updates)
        ret = HTTP_UPDATE_NO_UPDATES;
        break;
    case HTTP_CODE_NOT_FOUND:
        _lastError = HTTP_UE_SERVER_FILE_NOT_FOUND;
        ret = HTTP_UPDATE_FAILED;
        break;
    case HTTP_CODE_FORBIDDEN:
        _lastError = HTTP_UE_SERVER_FORBIDDEN;
        ret = HTTP_UPDATE_FAILED;
        break;
    default:
        _lastError = HTTP_UE_SERVER_WRONG_HTTP_CODE;
        ret = HTTP_UPDATE_FAILED;
        DEBUG_HTTP_UPDATE("[httpUpdate] HTTP Code is (%d)\n", code);
        //http.writeToStream(&Serial1);
        break;
    }

    http.end();
    return ret;
}

bool OTAUpdater::streamToFlash(Stream& in, uint32_t size, String md5, int command) {

    StreamString error;

    if(!Update.begin(size, command, 12, HIGH)) {
        _lastError = Update.getError();
        Update.printError(error);
        error.trim(); // remove line ending
        DEBUG_HTTP_UPDATE("[httpUpdate] Update.begin failed! (%s)\n", error.c_str());
        return false;
    }

    if(md5.length()) {
        if(!Update.setMD5(md5.c_str())) {
            _lastError = HTTP_UE_SERVER_FAULTY_MD5;
            DEBUG_HTTP_UPDATE("[httpUpdate] Update.setMD5 failed! (%s)\n", md5.c_str());
            return false;
        }
    }

    if(Update.writeStream(in) != size) {
        _lastError = Update.getError();
        Update.printError(error);
        error.trim(); // remove line ending
        DEBUG_HTTP_UPDATE("[httpUpdate] Update.writeStream failed! (%s)\n", error.c_str());
        return false;
    }

    if(!Update.end()) {
        _lastError = Update.getError();
        Update.printError(error);
        error.trim(); // remove line ending
        DEBUG_HTTP_UPDATE("[httpUpdate] Update.end failed! (%s)\n", error.c_str());
        return false;
    }

    return true;
}

std::vector<const char*>* OTAUpdater::deserializeOTAPayload(char* payload) {
  // Serial.printf("OTA UPDATE: Deserializing payload = %s\n", payload);
  const size_t capacity = JSON_OBJECT_SIZE(3) + 150;
  StaticJsonDocument<capacity> doc;
  DeserializationError error = deserializeJson(doc, payload);
  std::vector<const char*> *updateInfo = new std::vector<const char*>(2);
  
  if (error) {
      std::string errPrefix("MQTT: deserializeJson() failed with code ");
      std::string errCode(error.c_str());
      std::string errorMessage = errPrefix + errCode;
      Serial.println(errorMessage.c_str());
      *updateInfo = {"",""};
      return updateInfo;
  }

  const char* build = doc["build"];
  const char* version = doc["version"];
  *updateInfo = {build, version};
  return updateInfo;
}

/*
 *  Ensures: 
 *      (1) that the build and version are non-null
 *      (2) That they are sensible 
 */
bool OTAUpdater::validDistributonRequest(const char* build, const char* version) {
    std::string buildStr(build);
    std::string versionStr(version);

    if (buildStr.length() && versionStr.length()) {
        int buildNum = atoi(build);
        int majorVersionNum = (int)(versionStr.front()) - 48; // Convert to Ascii val, then reposition 

        if (buildNum > 0 && majorVersionNum > 0) {
            return true;
        }
    } 

    // Catch all else
    return false;
}
