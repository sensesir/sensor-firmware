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
#include "./src/networking/GDoorServer.hpp"
#include "./src/models/SensorModel.hpp"
#include "./src/utilities/Utilities.h"

// Function prototypes
void messageReceived(char *topic, byte *payload, unsigned int length);

// Globals
SensorWifi wifiInterface;
SensorMQTT sensorMQTT;

void setup() {
  Serial.begin(BAUD_RATE);
  Serial.println("Firing up Esp!!");
  GDoorIO::getInstance().initialize();
  bool sensorInitialised = SensorModel::getInstance().sensorDataLoaded;

  if (!sensorInitialised) {
    initialiseSensorLocally();
  }
  
  wifiInterface.connectToWifi();
  setupMQTT(false);
}

void loop() {
  bool clientConnected = sensorMQTT.loop();

  // IO Update
  GDoorIO *doorIO = &GDoorIO::getInstance();
  doorIO->assessModePin();
  bool newDoorState = doorIO->doorStateChanged();
  if(newDoorState) { sensorMQTT.publishDoorState(); }

  // Reconnection checks
  if (WiFi.status() != WL_CONNECTED) { wifiInterface.setWiFiReconnectingState(); }
  if (!clientConnected) { reconnectMQTTClient(); }
  delay(10);
}

void setupMQTT(bool firstBoot) {
  int connStart = millis();
  bool mqttConnected  = sensorMQTT.initializeMQTT(messageReceived);     
  bool mqttSubscribed = sensorMQTT.subscribeToTopics();
  if (!(mqttConnected && mqttSubscribed)) {
    mqttFailureLoop();
  }

  // Setup successful: post boot message
  int connDur = (millis() - connStart) / 1000; // Int div - trunc to whole num is fine
  sensorMQTT.publishBootEvent(firstBoot, connDur); 
}

void mqttFailureLoop() {
  Serial.println("MAIN: Fatal MQTT connection failure - entering error state");
  GDoorIO::getInstance().networkLEDSetRed();
  
  while(true) {
    // May need: Reset WDT  
    // Waiting for user reset
    delay(10);
  }
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
  
  if (command == actuateStr) { GDoorIO::getInstance().actuateDoor(); } 
  else if (command == healthStr) { sensorMQTT.publishHealth(); } 
  else { sensorMQTT.publishUnknownTypeError(command, std::string("command")); }
}

void reconnectMQTTClient() {
  delay(SETUP_COOL_DOWN);
  int reconnStart = millis();
  bool reconnected = sensorMQTT.reconnectClientSync(); 
  bool subscribed = sensorMQTT.subscribeToTopics();
  
  if (reconnected && subscribed) {
    int reconnDur = (millis() - reconnStart) / 1000; // Int div is fine - trunc to whole num
    delay(SETUP_COOL_DOWN);
    sensorMQTT.publishReconnection(reconnDur);
  } else {
    mqttFailureLoop();
  }
}




