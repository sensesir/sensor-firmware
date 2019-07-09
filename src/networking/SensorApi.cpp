/*
*	  Wrapper class to work with the OSS HTTP library for an ESP8266  
*     that uses the Arduino SDK
*
*	  Collaborator(s): Josh Perry <josh.perry245@gmail.com> 
*	  Copyright 2019
*/

#include "SensorApi.hpp"

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