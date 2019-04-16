/*
 * Garage Door IoT sensor main file using Arduino framework
 * 
 * Author: Josh Perry
 * Created: 04/13/2019
 */

#include "./src/config/Configuration.h"
#include "./src/networking/WifiInterface.hpp"
#include "./src/digital-io/GDoorIO.hpp"
#include "./src/networking/SensorMQTT.hpp"

// Function prototypes
void messageReceived(char *topic, byte *payload, unsigned int length);

// Globals
SensorWifi wifiInterface;
GDoorIO doorIO;
SensorMQTT *sensorMQTT;

void setup() {
  Serial.begin(BAUD_RATE);
  Serial.println("Firing up Esp!!");
  doorIO.initializeGPIOPins();
  
  // Setup Wifi
  bool connected = wifiInterface.connectToWifi("library", "");   // Aspen library has no password

  // Setup MQTT
  sensorMQTT = new SensorMQTT(messageReceived);      // Todo: Create static constructor that returns object instance
  // sensorMQTT->connectDeviceGateway();
  
}

void loop() {
  // put your main code here, to run repeatedly:

}

void messageReceived(char *topic, byte *payload, unsigned int length) {
    // Handle new message here
}




