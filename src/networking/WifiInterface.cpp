/*
*	Wrapper class to work with the ESP8266 Wifi 
*   library to establish a connection to a local wifi network
*	Source file	
*
*	Author: Josh Perry
*	Copyright 2019
*
*/

#include "WifiInterface.hpp"

bool SensorWifi::connectToWifi(const char* ssid, const char* password){
	WiFi.hostname(SENSOR_UID);
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);

	// Wait for a connection
	Serial.print("WIFI INTERFACE: Attempting to connect to WiFi => ");
	Serial.println(ssid);
	GDoorIO *doorIO = &GDoorIO::getInstance();
	doorIO->networkLEDOff();
	while (WiFi.status() != WL_CONNECTED){
		delay(LED_FLASH_DELAY);
		doorIO->networkLEDToggleWhite();
		Serial.print(".");
	}

	Serial.println("");
	Serial.print("WIFI INTERFACE: Connected to ");
	Serial.println(ssid);
	
	// State all info
	delay(1000);
	printNetworkAllocations();

	doorIO->networkLEDSetWhite();
	connected = true;
	return true;
}

void SensorWifi::printNetworkAllocations() {
	IPAddress ipAddress = WiFi.localIP();
	Serial.print("WIFI INTERFACE: Assigned IP address = ");
	Serial.println(ipAddress);
	Serial.print("WIFI INTERFACE: Gateway IP = ");
	Serial.println(WiFi.gatewayIP());
	Serial.print("WIFI INTERFACE: Subnet mask = ");
	Serial.println(WiFi.subnetMask());
}

// Todo: Method needs testing
bool SensorWifi::setWiFiReconnectingState(){
	Serial.println("WIFI INTERFACE: WiFi connection dropped, attempting to reconnect");
	connected = false;
	
	GDoorIO *doorIO = &GDoorIO::getInstance();
	doorIO->networkLEDOff();
	while (WiFi.status() != WL_CONNECTED){
		delay(LED_FLASH_DELAY);
		doorIO->networkLEDToggleWhite();
		Serial.print(".");
	}

	// Reconnected
	Serial.println("");
	Serial.println("WIFI INTERFACE: Reconnected to network");
	
	// State all info
	delay(1000);
	printNetworkAllocations();

	// Indicate connected state on LED
	doorIO->networkLEDSetWhite();
	connected = true;
	return true;
}





