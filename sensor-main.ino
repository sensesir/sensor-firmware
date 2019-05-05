/*
 * Garage Door IoT sensor main file using Arduino framework
 * 
 * Collaborator(s): Josh Perry <josh.perry245@gmail.com>
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
SensorMQTT sensorMQTT;

void setup() {
  Serial.begin(BAUD_RATE);
  Serial.println("Firing up Esp!!");
  doorIO.initializeGPIOPins();
  
  // Setup Wifi
  bool connected = wifiInterface.connectToWifi("library", "");   // Aspen library has no password

  // Setup MQTT
  sensorMQTT.initializeMQTT(messageReceived);     
  sensorMQTT.subscribeToTopics();

  // Setup successful: post boot message
  sensorMQTT.publishBootEvent(false);         // Will be based on presence of a persisted sensorUID
}

void loop() {
  sensorMQTT.loop();
  delay(10);
}

void messageReceived(char *topic, byte *payload, unsigned int length) {
    // Handle new message here
    Serial.println("Received new message from broker");
}




