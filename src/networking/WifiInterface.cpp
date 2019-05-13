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
	while (WiFi.status() != WL_CONNECTED){
		delay(500);
		Serial.print(".");
		toggleLED();
	}

	Serial.println("");
	Serial.print("WIFI INTERFACE: Connected to ");
	Serial.println(ssid);
	
	// State all info
	delay(1000);
	printNetworkAllocations();

	// Indicate connected state on LED
	digitalWrite(WIFI_LED, HIGH);
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
	// Reconnection should happen automatically
	// Start a ticker at interval 500ms
	Serial.println("WIFI INTERFACE: WiFi connection dropped, attempting to reconnect");
	while (WiFi.status() != WL_CONNECTED){
		delay(500);
		Serial.print(".");
		toggleLED();
	}

	// Reconnected
	Serial.println("");
	Serial.print("WIFI INTERFACE: Reconnected to network");
	
	// State all info
	delay(1000);
	printNetworkAllocations();

	// Indicate connected state on LED
	digitalWrite(WIFI_LED, HIGH);
	connected = true;
	return true;
}

void SensorWifi::flashWifiLED(){
	Serial.println("Initiating WiFi set up");
	while(1){
		toggleLED();
		delay(1000);
		toggleLED();
		delay(1000);
	}
}

void SensorWifi::toggleLED(){
	int state = digitalRead(WIFI_LED);
	digitalWrite(WIFI_LED, !state);
}




