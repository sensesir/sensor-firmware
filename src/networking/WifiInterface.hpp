/*
*	Wrapper class to work with the ESP8266 Wifi 
*   library to establish a connection to a local wifi network
*	Header file
*
*	Author: Josh Perry <josh.perry245@gmail.com> 
*	Copyright 2019
*
*/

#ifndef WIFIINTERFACE_H
#define WIFIINTERFACE_H

#include <ESP8266WiFi.h>
#include <Arduino.h>
#include "../models/SensorModel.hpp"
#include "../digital-io/GDoorIO.hpp"
#include "../config/Configuration.h"

// Static IP 
class SensorWifi{
	public:
		bool connectToWifi();
		bool connectToWifi(const char* ssid, const char* password);
		bool setWiFiReconnectingState();
		bool connected = false;

	private:
		const char* currentSsid;
		const char* currentPassword;
		const int* currentGatewayIPArr;
		const int* currentSubnetIPArr;
		int currentStaticOctet;
		void printNetworkAllocations();
};




#endif