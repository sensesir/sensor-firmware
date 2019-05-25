/*
*	Script to implement an ephemenral local server. 
*   Intended for the use of communicating with a local 
*   client to acquire wifi credentials and distribute 
*   sensor UID.
*
*	Author: Josh Perry <josh.perry245@gmail.com> 
*	Copyright 2019
*/

#include "GDoorServer.hpp"

// Globals
ESP8266WebServer *server;
bool sentSensorUID = false;
bool sensorUIDSendSuccess = false;

// Function prototypes
void setupDNS();
void registerEndpoints(ESP8266WebServer *server);
bool credentialsAcquired();
void handleRoot();
void getSensorUIDEndpoint();
void postWiFiCredsEndpoint();
void confirmSensorUIDSent();

/**
 *  Method sets up an Access point with DNS such that user 
 *  mobile app can connect to it and pass WiFi credentials
 */

void initialiseSensorLocally() {
	Serial.println("SERVER: Creating access point");
	GDoorIO::getInstance().networkLEDSetYellow();
	IPAddress apIP(10, 10, 10, 1);                      // Todo: research if this will cause any collisions
    IPAddress subnet(255,255,255,0);                    // Todo: refresher in subnet mask function
    WiFi.softAPConfig(apIP, apIP, subnet);
    WiFi.softAP(ACCESS_POINT_NAME);		// No password
    setupDNS();

	// Set up server
	server = new ESP8266WebServer(serverPort);
	registerEndpoints(server);
	server->begin();

    Serial.print("SERVER: Local IP => ");
    Serial.println(WiFi.softAPIP());

	// Loop untill we have what we need! 
    do {
        server->handleClient();
        MDNS.update();  // If the local IP is changed
    } while (!credentialsAcquired());

    delay(2500);    // Complete last http response
    server->stop();
    MDNS.end();
    Serial.println("SERVER: Completed credential acquisition");
}

bool credentialsAcquired() {
    bool wifiCreds = SensorModel::getInstance().wifiCredsAcquired();
    return wifiCreds && sentSensorUID && sensorUIDSendSuccess;
}

void setupDNS() {
    Serial.print("SERVER: Setting up DNS => http://");
    Serial.print(DNS_ADDRESS);
    Serial.println(".local");
    while(!MDNS.begin(DNS_ADDRESS)) {
        Serial.print(".");
        delay(1000);
    }
}

void registerEndpoints(ESP8266WebServer *server) {
	server->on("/", handleRoot);
    server->on(GET_SENSOR_UID_ENDPOINT, getSensorUIDEndpoint);
    server->on(POST_WIFI_CREDS_ENDPOINT, postWiFiCredsEndpoint);
    server->on(POST_SENSOR_UID_CONFIRM, confirmSensorUIDSent);
}

// Define endpoints
void handleRoot() {
    // Todo: make more useful
	Serial.println("SERVER: Received root request");
    server->send(200, "text/plain", "No content");
}

void getSensorUIDEndpoint() {
    Serial.println("SERVER: Sending sensor UID to client");

    // Create payload
    const int capacity = JSON_OBJECT_SIZE(1) + 120;     // 1 KV pair + 120 bytes spare for const input duplication
    StaticJsonDocument<capacity> payload;
    payload[KEY_SENSOR_UID] = SENSOR_UID;

    // Serialize JSON into char
    int jsonLength = measureJson(payload);
    char serializedPayload[256];
    serializeJson(payload, serializedPayload);

    server->send(200, "application/json", serializedPayload);
    sentSensorUID = true;
}

void postWiFiCredsEndpoint() {
    if (server->hasArg("plain") == false) {
        Serial.println("SERVER: Error - did not receive WiFi creds");
        server->send(400,"text/plain", "No credentials received");
        return;
    }

    Serial.println("SERVER: Received wifi creds, deserializing");
    const size_t capacity = JSON_OBJECT_SIZE(2) + 120; // Only 1 KV pair for now + buffer (100 = ~2x safety) for duplication of const
    StaticJsonDocument<capacity> doc;
    DeserializationError error = deserializeJson(doc, server->arg("plain"));

    // Unpack - being wary of null pointers
    const char* rawSSID = doc[KEY_SSID];
    if (!rawSSID) { 
        server->send(400, "text/plain", "No SSID received");
        return;
    }

    const char* rawPassword = doc[KEY_WIFI_PASSWORD];
    if (!rawPassword) { 
        server->send(400, "text/plain", "No password received");
        return;
    }

    Serial.println("SERVER: Received SSID & password, setting model");
    SensorModel *sensorModel = &SensorModel::getInstance();
    sensorModel->setModelSSID(rawSSID);
    sensorModel->setModelPassword(rawPassword);
    sensorModel->writeModelDataToDisk();
    server->send(200, "text/plain", "successfully set SSID & password");
}

void confirmSensorUIDSent() {
    Serial.println("SERVER: Received confirmation of sensorUID upload");
    server->send(200,"text/plain", "OK");
    sensorUIDSendSuccess = true;
}