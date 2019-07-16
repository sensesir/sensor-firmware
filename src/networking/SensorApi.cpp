/*
*	  Wrapper class to work with the OSS HTTP library for an ESP8266  
*     that uses the Arduino SDK
*
*	  Collaborator(s): Josh Perry <josh.perry245@gmail.com> 
*	  Copyright 2019
*/

#include "SensorApi.hpp"

std::string hexStr(uint8_t *data, int len);

void postMQTTConnError() {
    Serial.print("SENSOR API: Posting MQTT conn error to server");

    // Create JSON payload for post
    const int capacity = JSON_OBJECT_SIZE(2) + 120;     // 4 KV pairs + 120 bytes spare for const input duplication
    StaticJsonDocument<capacity> payload;
    payload[KEY_SENSOR_UID] = SENSOR_UID;
    payload[KEY_EVENT] = EVENT_MQTT_CONN_ERROR;

    Serial.print("SENSOR API: Posting payload => ");
    serializeJson(payload, Serial);

    // Serialize JSON into char
    int jsonLength = measureJson(payload);
    char serializedPayload[256];
    serializeJson(payload, serializedPayload);

    HTTPClient httpReq;
    httpReq.begin(MQTT_CONN_ERR_ENDPOINT);  
    httpReq.addHeader("Content-Type", "application/json");
    httpReq.addHeader("x-api-key", API_KEY);
    int resCode = httpReq.POST(serializedPayload);

    // Look for the response & examine 
    String resString = httpReq.getString();
    Serial.println();
    Serial.print("SENSOR API: Receieved response to HTTP POST => ");
    Serial.println(resCode);
    Serial.println(resString);

    // Close connection
    httpReq.end();
}

void otaUpdate() {
    WiFiClient client;
    HTTPClient httpReq;

    Serial.println("SENSOR API: Creating connection to stream firmware binary");
    httpReq.begin(client, OTA_STREAM_ENDPOINT);
    httpReq.addHeader("x-api-key", OTA_API_KEY);
    httpReq.addHeader("sensor-uid", SENSOR_UID);

    // Hit the endpoint
    int payloadSize = 0;
    int httpCode = httpReq.GET();
    bool updateSuccess = false; 

    if (httpCode > 0) {
        // Server has responded 
        Serial.printf("SENSOR API: Http res code: %d\n", httpCode);

        if (httpCode == 200) {
            Serial.print("SENSOR API: Starting binary stream of size = ");

            // Get length of payload & create a read buffer 
            int len = httpReq.getSize();
            Serial.println(len);
            uint8_t buff[128] = { 0 };      // Verify that this should be unsigned
            
            /*
            while (httpReq.connected() && (len > 0 || len == -1)) {
                // read up to 128 byte
                int c = stream->readBytes(buff, std::min((size_t)len, sizeof(buff)));
                // Serial.printf("readBytes: %d\n", c);
                
                if (!c) {
                    // Corrupt connection? Fail?
                    Serial.println("SENSOR API: Read timeout.");
                    // break;
                }

                // write it to Serial
                // Serial.write(buff, c);
                payloadSize += c;

                if (len > 0) {
                    // Not applicable to us, server doesn't send file size... Unless we make it? 
                    len -= c;
                }

                // Temp (may cause the stream to flood)
                std::string hexString = hexStr(buff, sizeof(buff));
                Serial.println(hexString.c_str());
            }
            */
        }
    } 
    
    else {
      Serial.printf("SENSOR API: [HTTP] GET... failed, error: %s\n", httpReq.errorToString(httpCode).c_str());
    }

    httpReq.end();
    Serial.print("SENSOR API: Completed transaction with payload size = ");
    Serial.println(payloadSize);

    Serial.print("SENSOR API: Successful update = ");
    Serial.println(updateSuccess);
}

void otaUpdateV2() {
    WiFiClient client;
    t_httpUpdate_return ret = ESPhttpUpdate.update(client, "http://ec2-34-245-168-70.eu-west-1.compute.amazonaws.com:3000/otaNonStream");

    switch (ret) {
        case HTTP_UPDATE_FAILED:
            USE_SERIAL.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
            break;

        case HTTP_UPDATE_NO_UPDATES:
            USE_SERIAL.println("HTTP_UPDATE_NO_UPDATES");
            break;

        case HTTP_UPDATE_OK:
            USE_SERIAL.println("HTTP_UPDATE_OK");
            break;
    }
}


std::string hexStr(uint8_t *data, int len) {
    std::stringstream ss;
    ss<<std::hex;

    for(int i(0);i<len;++i){
        ss<<(int)data[i];
    }
        
    return ss.str();
}