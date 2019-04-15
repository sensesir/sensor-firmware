/*
 * Garage Door IoT sensor main file using Arduino framework
 * 
 * Author: Josh Perry
 * Created: 04/13/2019
 */

#include "./src/config/Configuration.h"
#include "./src/networking/WifiInterface.hpp"
#include "./src/digital-io/GDoorIO.hpp"

// Globals
SensorWifi wifiInterface;
GDoorIO doorIO;

void setup() {
  Serial.begin(BAUD_RATE);
  Serial.println("Firing up Esp!!");
  doorIO.initializeGPIOPins();
  
  // Setup Wifi
  bool connected = wifiInterface.connectToWifi("library", "");   // Aspen library has no password
}

void loop() {
  // put your main code here, to run repeatedly:

}
