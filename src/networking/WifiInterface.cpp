/*
*	Wrapper class to work with the ESP8266 Wifi 
*   library to establish a connection to a local wifi network
*	Source file	
*
*	Author: Josh Perry
*	Copyright 2018
*
*/

#include "WifiInterface.hpp"

bool SensorWifi::connectToWifi(const char* ssid, const char* password){
	// Tempt - inspect **
	WiFi.hostname("SensorJPDev");
	
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

IPAddress SensorWifi::setWiFiReconnectingState(){
	// Start a ticker at interval 500ms
	Serial.println("WIFI INTERFACE: WiFi connection dropped, attempting to reconnect");
	while (WiFi.status() != WL_CONNECTED){
		delay(500);
		Serial.print(".");
		toggleLED();
	}

	// Reconnected
	Serial.println("WIFI INTERFACE: Successfully reconnected");
	digitalWrite(WIFI_LED, HIGH);

	// Print and return the assigned local IP on reconnection
	delay(1000);
	IPAddress ipAddress = WiFi.localIP();
	IPAddress gatewayIP = WiFi.gatewayIP();
	Serial.print("WIFI INTERFACE: Assigned LAN IP address = ");
	Serial.println(ipAddress);

	// Check if statuc IP is correctly assigned
	bool correctStaticIP = (ipAddress[0] == gatewayIP[0]) && (ipAddress[1] == gatewayIP[1]) && (ipAddress[2] == gatewayIP[2]) && (ipAddress[3] == currentStaticOctet);
	if (!correctStaticIP){
		WiFi.disconnect();
		// TODO: Add connection method here
	}

	return ipAddress;
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




