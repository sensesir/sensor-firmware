/*
 * Garage Door IoT sensor main file using Arduino framework
 * 
 * Collaborator(s): Josh Perry <josh.perry245@gmail.com>
 * Created: 04/13/2019
 */

// Local imports
#include "./src/config/Configuration.h"
#include "./src/config/ErrorCodes.h"
#include "./src/networking/WifiInterface.hpp"
#include "./src/digital-io/GDoorIO.hpp"
#include "./src/networking/SensorMQTT.hpp"
#include "./src/networking/GDoorServer.hpp"
#include "./src/models/SensorModel.hpp"
#include "./src/utilities/Utilities.h"
#include "./src/networking/SensorApi.hpp"
#include "./src/utilities/OTAUpdater.hpp"

// Function prototypes
void messageReceived(char *topic, byte *payload, unsigned int length);

// Globals
SensorWifi wifiInterface;
SensorMQTT sensorMQTT;

void setup() {
  Serial.begin(BAUD_RATE);
  Serial.print("Starting GDoor sensor version => ");
  Serial.println(FIRMWARE_VERSION);
  GDoorIO::getInstance().initialize();
  bool sensorInitialised = SensorModel::getInstance().sensorDataLoaded;

  if (!sensorInitialised) {
    initialiseSensorLocally();
  }
  
  wifiInterface.connectToWifi();
  setupMQTT(!sensorInitialised);
}

void loop() {
  bool clientConnected = sensorMQTT.loop();
  checkRSSI();
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
  postMQTTConnError();

  // Force restart after cool down - try start clean
  delay(20000);
  ESP.restart();
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
    char payloadUIDCheck[512];
    byteToCharArray(payload, length, payloadUIDCheck);  // Required because ArduinoJSON deserialization can't do byte* - only char*
    bool correctSensorUID = sensorMQTT.verifyTargetUID(payloadUIDCheck, &sensorUID);  

    if (!correctSensorUID) {
      std::string prefix("Incorrect sensorUID  => ");
      std::string errorMessage = prefix + sensorUID;
      sensorMQTT.publishError(ERROR_SENSOR_INCORRECT_TARGET, errorMessage.c_str());
      return;
    }
    
    else if (category == COMMAND) {
      char payloadCharArr[512];
      byteToCharArray(payload, length, payloadCharArr);
      handleCommand(descriptor, payloadCharArr);
    }

    else {
      sensorMQTT.publishUnknownTypeError(category, std::string("category"));
      return;
    }
}

void handleCommand(std::string command, char* payload) {
  // String conversion of constants
  std::string actuateStr(SUB_ACTUATE);    // May need to improve this
  std::string healthStr(SUB_HEALTH_PING);
  std::string firmwareUpdateStr(SUB_FIRMWARE_UPDATE);
  std::string rssiRequestStr(SUB_RSSI_REQUEST);

  if (command == actuateStr) { GDoorIO::getInstance().actuateDoor(); } 
  else if (command == healthStr) { sensorMQTT.publishHealth(); }
  else if (command == firmwareUpdateStr) { handleOTAUpate(payload); }
  else if (command == rssiRequestStr) { sensorMQTT.publishRssi(); }
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

void handleOTAUpate(char* payload) {
  sensorMQTT.disconnect();
  OTAUpdater *otaUpdate = new OTAUpdater();
  otaUpdate->updateFirmware(payload); 

  // If the function returns without restarting, it means it failed - publish MQTT message after re-connection
  reconnectMQTTClient();
  sensorMQTT.publishError(ERROR_SENSOR_OTA_FAILURE, "OTA update failed");  // TODO: Add more info
}

void checkRSSI() {
  float rssiRD; 
  rssiRD = WiFi.RSSI();
  if (rssiRD < -99)
  {
    rssiRD = -99;
  }
  if (rssiRD < -80)
  {
    sensorMQTT.publishError(ERROR_SENSOR_SIGNAL_STRENGTH_FAILURE, "Inadequate WiFi Signal Strength (<-80dBm)");
  }
  
}



