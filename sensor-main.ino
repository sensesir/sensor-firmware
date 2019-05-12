/*
 * Garage Door IoT sensor main file using Arduino framework
 * 
 * Collaborator(s): Josh Perry <josh.perry245@gmail.com>
 * Created: 04/13/2019
 */

// Local imports
#include "./src/config/Configuration.h"
#include "./src/networking/WifiInterface.hpp"
#include "./src/digital-io/GDoorIO.hpp"
#include "./src/networking/SensorMQTT.hpp"
#include "./src/utilities/Utilities.h"

// Function prototypes
void messageReceived(char *topic, byte *payload, unsigned int length);

// Globals
SensorWifi wifiInterface;
GDoorIO doorIO;
SensorMQTT sensorMQTT;

void setup() {
  Serial.begin(BAUD_RATE);
  Serial.println("Firing up Esp!!");
  doorIO.initialize();
  
  // Setup Wifi
  bool connected = wifiInterface.connectToWifi("The FishBowl", "Glennie123");

  // Setup MQTT
  sensorMQTT.initializeMQTT(messageReceived);     
  sensorMQTT.subscribeToTopics();

  // Setup successful: post boot message
  sensorMQTT.publishBootEvent(true);      
}

void loop() {
  sensorMQTT.loop();
  delay(10);
}

void messageReceived(char *topic, byte *payload, unsigned int length) {
    // Handle new message here
    Serial.print("MAIN: Received new message from topic => ");
    Serial.println(topic);

    // Parse topic to analyze what to do
    std::vector<std::string> topicComponents;
    splitCharByDelimiter(topic, TOPIC_DELIMITER, &topicComponents);
    std::string category   = topicComponents[3];
    std::string descriptor = topicComponents[4];

    // Deserialize sensorUID as redundant check for correct target
    std::string sensorUID;
    char payloadCast[256];
    byteToCharArray(payload, length, payloadCast);  // Seems a bit pointless, but for ease of argument passing
    bool correctSensorUID = sensorMQTT.verifyTargetUID(payloadCast, &sensorUID);  

    if (!correctSensorUID) {
      std::string prefix("ERROR: Incorrect sensorUID received => ");
      std::string errorMessage = prefix + sensorUID;
      sensorMQTT.publishError(errorMessage.c_str());
      return;
    }
    
    else if (category == COMMAND) {
      handleCommand(descriptor);
    }

    else {
      sensorMQTT.publishUnknownTypeError(category, std::string("category"));
      return;
    }
}

void handleCommand(std::string command) {
  // String conversion of constants
  std::string actuateStr(SUB_ACTUATE);    // May need to improve this
  std::string healthStr(SUB_HEALTH_PING);
  
  if (command == actuateStr) { doorIO.actuateDoor(); } 
  else if (command == healthStr) { sensorMQTT.publishHealth(); } 
  else { sensorMQTT.publishUnknownTypeError(command, std::string("command")); }
}




