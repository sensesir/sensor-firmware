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

    SensorModel::getInstance().writeModelDataToDisk();
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

}

void postWiFiCredsEndpoint() {

}

void confirmSensorUIDSent() {

}