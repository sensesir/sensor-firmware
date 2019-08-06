/*
*	  Wrapper class to work with the OSS MQTT library for an ESP8266  
*   that uses the Arduino SDK
*
*	  Collaborator(s): Josh Perry <josh.perry245@gmail.com> 
*	  Copyright 2019
*/

#include "SensorMQTT.hpp"

void resetSensor();

SensorMQTT::SensorMQTT() {
  // Just to init super class
}

bool SensorMQTT::initializeMQTT(mqttMsgRecCallback callback) {
    Serial.println("MQTT: Instantiating MQTT client");

    // Set SSL certs
    BearSSL::X509List cert(caCert);
    BearSSL::X509List client_crt(clientCert);
    BearSSL::PrivateKey key(privateKey);
    this->wifiClient = new WiFiClientSecure();
    this->wifiClient->setTrustAnchors(&cert);
    this->wifiClient->setClientRSACert(&client_crt, &key);
    setClient(*wifiClient);

    // Set indicator LED (solid colour because of blocking functions in external lib)
    GDoorIO::getInstance().networkLEDSetCyan();

    // Set time zone - necessary for AWS MQTT [todo: investigate why]
    this->ntpConnect();
    this->setServer(AWS_IOT_DEVICE_GATEWAY, 8883);
    this->setCallback(callback);

    /*
     * Todo: Work out what resource on this function stack is needed by 
     * the connect method. When called outside of this scope cases a crash.
     */
    if(this->connectDeviceGateway()) {
      return true;
    } else {
      return this->reconnectClientSync();
    }
}

bool SensorMQTT::connectDeviceGateway() {  
    this->connTimer.once(MQTT_CONN_TIMEOUT, resetSensor);
    bool success = this->connect(SENSOR_UID);
    this->connTimer.detach();

    if (success) {
      Serial.println("MQTT: Successfully connected to AWS IoT Cloud");
      GDoorIO::getInstance().networkLEDSetBlue();
      return true;
    } else {
      Serial.println("MQTT: Failed to connect to AWS IoT Cloud");
      this->pubSubError(this->state());
      postMQTTConnError();
      return false;
    }
}

bool SensorMQTT::reconnectClientSync() {
  GDoorIO::getInstance().networkLEDSetCyan();

  // Set SSL certs
  BearSSL::X509List cert(caCert);
  BearSSL::X509List client_crt(clientCert);
  BearSSL::PrivateKey key(privateKey);
  this->wifiClient->setTrustAnchors(&cert);
  this->wifiClient->setClientRSACert(&client_crt, &key);
  setClient(*wifiClient); // Should be able to remove

  // Not explicitly required - but to be sure after wifi dropout
  this->ntpConnect();
  
  for (int reconAttempt = 0; reconAttempt < MQTT_RECON_MAX; reconAttempt++) {
    Serial.print("MQTT: Attempting synchronous reconnection #");
    Serial.println(reconAttempt);
    if (this->connectDeviceGateway()) {
      return true;
    } else {
      delay(RECONNECT_DELAY_MQTT);
    }
  }

  return false;
}

bool SensorMQTT::subscribeToTopics() {
  Serial.println("MQTT: Subscribing sensor client to topics");
  std::vector<const char*> SUBSCRIBE_TOPICS = {SUB_ACTUATE, SUB_HEALTH_PING, SUB_FIRMWARE_UPDATE};

  for (int i=0; i < SUBSCRIBE_TOPICS.size(); i++) {
    char topic[256];
    this->generateTopic(topic, SENSOR, SENSOR_UID, COMMAND, SUBSCRIBE_TOPICS[i]);
    bool subscribed = this->subscribe(topic);

    if (subscribed) {
      Serial.print("MQTT: Subscribed to ");
      Serial.println(topic);
    } else {
      Serial.print("MQTT: Failed to subscribe to ");
      Serial.println(topic);
      return false;
      
      // TODO: throw error here - sensor shouldn't initialize with failed subscription
      // Write to EEPROM / some flash mem
    }
  }

  return true;
}

void SensorMQTT::publishBootEvent(bool firstBoot, int connDur) {
  // Create topic
  char topic[256];
  this->generateTopic(topic, SERVER, SERVER_UID, EVENT, PUB_BOOT);

  // Create payload using JSON lib
  const int capacity = JSON_OBJECT_SIZE(6) + 120;     // 6 KV pairs + 120 bytes spare for const input duplication
  StaticJsonDocument<capacity> payload;
  payload[KEY_SENSOR_UID] = SENSOR_UID;
  payload[KEY_FIRMWARE_VERSION] = FIRMWARE_VERSION; 
  payload[KEY_DURATION] = connDur;
  payload[KEY_STATE] = GDoorIO::getInstance().gdoor.stateString;
  if (firstBoot) {
    payload[KEY_EVENT] = PUB_FIRST_BOOT;
    payload[KEY_USER_UID] = SensorModel::getInstance().userUID;
  } else {
    payload[KEY_EVENT] = PUB_BOOT;
  }
  Serial.print("MQTT: Publishing payload: ");
  serializeJson(payload, Serial);
  Serial.println("");

  // Serialize JSON into char
  int jsonLength = measureJson(payload);
  char serializedPayload[256];
  serializeJson(payload, serializedPayload);

  bool success = this->publish(topic, serializedPayload);        
  if (success) {
    Serial.print("MQTT: Published message to topic: ");
    Serial.println(topic);
    Serial.print("MQTT: Payload => ");
    serializeJson(payload, Serial);
    Serial.println("");
  } else {
    Serial.print("MQTT: Failed to publish message to topic: ");
    Serial.println(topic);
  }
}

void SensorMQTT::publishDoorState() {
  // Create topic
  char topic[256];
  this->generateTopic(topic, SERVER, SERVER_UID, EVENT, PUB_DOOR_STATE);

  // Create payload
  const int capacity = JSON_OBJECT_SIZE(3) + 120;     // 3 KV pairs + 120 bytes spare for const input duplication
  StaticJsonDocument<capacity> payload;
  payload[KEY_SENSOR_UID] = SENSOR_UID;
  payload[KEY_EVENT] = PUB_DOOR_STATE;
  payload[KEY_STATE] = GDoorIO::getInstance().gdoor.stateString;    // normal char

  // Serialize JSON into char
  int jsonLength = measureJson(payload);
  char serializedPayload[256];
  serializeJson(payload, serializedPayload);

  bool success = this->publish(topic, serializedPayload);        
  if (success) {
    Serial.print("MQTT: Published message to topic: ");
    Serial.println(topic);
    Serial.print("MQTT: Payload => ");
    serializeJson(payload, Serial);
    Serial.println("");
  } else {
    Serial.print("MQTT: Failed to publish message to topic: ");
    Serial.println(topic);
  }
}

void SensorMQTT::publishReconnection(int reconnDur) {
  // Create topic
  char topic[256];
  this->generateTopic(topic, SERVER, SERVER_UID, EVENT, PUB_RECONNECT);

  // Create payload
  const int capacity = JSON_OBJECT_SIZE(3) + 120;     // 3 KV pairs + 120 bytes spare for const input duplication
  StaticJsonDocument<capacity> payload;
  payload[KEY_SENSOR_UID] = SENSOR_UID;
  payload[KEY_EVENT] = PUB_RECONNECT;
  payload[KEY_DURATION] = reconnDur;

  // Serialize JSON into char
  int jsonLength = measureJson(payload);
  char serializedPayload[256];
  serializeJson(payload, serializedPayload);

  bool success = this->publish(topic, serializedPayload);        
  if (success) {
    Serial.print("MQTT: Published message to topic: ");
    Serial.println(topic);
    Serial.print("MQTT: Payload => ");
    serializeJson(payload, Serial);
    Serial.println("");
  } else {
    Serial.print("MQTT: Failed to publish message to topic: ");
    Serial.println(topic);
  }
}

void SensorMQTT::publishHealth() {
  // Create topic
  char topic[256];
  this->generateTopic(topic, SERVER, SERVER_UID, EVENT, PUB_HEALTH);

  // Create payload
  const int capacity = JSON_OBJECT_SIZE(2) + 120;     // 2 KV pairs + 120 bytes spare for const input duplication
  StaticJsonDocument<capacity> payload;
  payload[KEY_SENSOR_UID] = SENSOR_UID;
  payload[KEY_EVENT] = PUB_HEALTH;

  // Serialize JSON into char
  int jsonLength = measureJson(payload);
  char serializedPayload[256];
  serializeJson(payload, serializedPayload);

  bool success = this->publish(topic, serializedPayload);        
  if (success) {
    Serial.print("MQTT: Published message to topic: ");
    Serial.println(topic);
    Serial.print("MQTT: Payload => ");
    serializeJson(payload, Serial);
    Serial.println("");
  } else {
    Serial.print("MQTT: Failed to publish message to topic: ");
    Serial.println(topic);
  }
}

void SensorMQTT::publishError(const char* errorCode, const char* message) {
  // Create topic
  char topic[256];
  this->generateTopic(topic, SERVER, SERVER_UID, EVENT, PUB_ERROR);

  // Create payload
  const int capacity = JSON_OBJECT_SIZE(4) + 120;     // 3 KV pairs + 120 bytes spare for const input duplication
  StaticJsonDocument<capacity> payload;
  payload[KEY_SENSOR_UID] = SENSOR_UID;
  payload[KEY_EVENT] = PUB_ERROR;
  payload[KEY_ERROR_CODE] = errorCode;
  payload[KEY_MESSAGE] = message;

  // Serialize JSON into char
  int jsonLength = measureJson(payload);
  char serializedPayload[256];
  serializeJson(payload, serializedPayload);

  bool success = this->publish(topic, serializedPayload);        
  if (success) {
    Serial.print("MQTT: Published message to topic: ");
    Serial.println(topic);
    Serial.print("MQTT: Payload => ");
    serializeJson(payload, Serial);
    Serial.println("");
  } else {
    Serial.print("MQTT: Failed to publish message to topic: ");
    Serial.println(topic);
  }
}

// Specific error type
void SensorMQTT::publishUnknownTypeError(std::string unknownType, std::string identifier) {
  std::string errorMessage = "Type: " + unknownType + " | " + "Identifier: " + identifier;

  // Create topic
  char topic[256];
  this->generateTopic(topic, SERVER, SERVER_UID, EVENT, PUB_ERROR);

  // Create payload
  const int capacity = JSON_OBJECT_SIZE(3) + 120;     // 3 KV pairs + 120 bytes spare for const input duplication
  StaticJsonDocument<capacity> payload;
  payload[KEY_SENSOR_UID] = SENSOR_UID;
  payload[KEY_EVENT] = PUB_ERROR;
  payload[KEY_MESSAGE] = errorMessage.c_str();

  // Serialize JSON into char
  int jsonLength = measureJson(payload);
  char serializedPayload[256];
  serializeJson(payload, serializedPayload);

  bool success = this->publish(topic, serializedPayload);        
  if (success) {
    Serial.print("MQTT: Published message to topic: ");
    Serial.println(topic);
    Serial.print("MQTT: Payload => ");
    serializeJson(payload, Serial);
    Serial.println("");
  } else {
    Serial.print("MQTT: Failed to publish message to topic: ");
    Serial.println(topic);
  }
}

/**

  Currently struggling with how to set argument 2 to pass the StaticJsonDocument instance in

  void SensorMQTT::publishMessage(char* topic, const JsonObject& payload) {
    // Serialize JSON into char
    int jsonLength = measureJson(payload);
    char serializedPayload[256];                    // 256 is based on expected largest packet + safety, could need to update
    serializeJson(payload, serializedPayload);

    bool success = this->publish(topic, serializedPayload);        
    if(success) {
      Serial.print("MQTT: Published message to topic: ");
      Serial.println(topic);
    } else {
      Serial.print("MQTT: Failed to publish message to topic: ");
      Serial.println(topic);
    }
  }
*/

bool SensorMQTT::verifyTargetUID(char *payload, std::string *sensorUID) {
  this->deserializeStdPayload(payload, sensorUID);

  if (*sensorUID == std::string(SENSOR_UID)){ return true; }
  else { return false; }
}

void SensorMQTT::deserializeStdPayload(char* payload, std::string *sensorUID) {
  const size_t capacity = JSON_OBJECT_SIZE(1) + 100; // Only 1 KV pair for now + buffer (100 = ~2x safety) for duplication of const
  StaticJsonDocument<capacity> doc;
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
      std::string errPrefix("MQTT: deserializeJson() failed with code ");
      std::string errCode(error.c_str());
      std::string errorMessage = errPrefix + errCode;
      Serial.println(errorMessage.c_str());
      this->publishError(ERROR_SENSOR_JSON, errorMessage.c_str());
      return;
  }

  // Set pointer that was passed in
  const char* rawSensorUID = doc[KEY_SENSOR_UID];
  if (!rawSensorUID) { 
    *sensorUID = "";  // Error case - rawSensorUID = NULL_PTR
  } else {
    *sensorUID = rawSensorUID;
  }
}

void SensorMQTT::generateTopic(char* topic,const char* target, const char* targetUID, const char* msgCategory, const char* descriptor) {
  std::string firmwareMajorVersion("v");
  firmwareMajorVersion += FIRMWARE_VERSION[0];
  std::string topicStr = std::string(target) + "/" + std::string(targetUID) + "/" + firmwareMajorVersion + "/" + std::string(msgCategory) + "/" + std::string(descriptor);
  strcpy(topic, topicStr.c_str());
}

/*
 *  Open source method
 *  Todo: read through and confirm functionality (when time permits)
 */

void SensorMQTT::ntpConnect() {
    Serial.print("MQTT: Setting time using SNTP");
    #ifdef USE_SUMMER_TIME_DST 
    uint8_t DST = 1;
    #else 
    uint8_t DST = 0;
    #endif
    unsigned long lastMillis = 0;
    time_t now;
    time_t nowish = 1510592825;     // Figure out why **
    
    configTime(TIME_ZONE * 3600, DST * 3600, "pool.ntp.org", "time.nist.gov");
    now = time(nullptr);

    while (now < nowish) {
        delay(500);
        Serial.print(".");
        now = time(nullptr);
    }

    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    Serial.println("");
    Serial.print("MQTT: Current time: ");
    Serial.println(asctime(&timeinfo));
}

void SensorMQTT::pubSubError(int8_t MQTTErr) {
  if (MQTTErr == MQTT_CONNECTION_TIMEOUT)
    Serial.println("MQTT: Connection tiemout");
  else if (MQTTErr == MQTT_CONNECTION_LOST)
    Serial.println("MQTT: Connection lost");
  else if (MQTTErr == MQTT_CONNECT_FAILED)
    Serial.println("MQTT: Connect failed");
  else if (MQTTErr == MQTT_DISCONNECTED)
    Serial.println("MQTT: Disconnected");
  else if (MQTTErr == MQTT_CONNECTED)
    Serial.println("MQTT: Connected");
  else if (MQTTErr == MQTT_CONNECT_BAD_PROTOCOL)
    Serial.println("MQTT: Connect bad protocol");
  else if (MQTTErr == MQTT_CONNECT_BAD_CLIENT_ID)
    Serial.println("MQTT: Connect bad Client-ID");
  else if (MQTTErr == MQTT_CONNECT_UNAVAILABLE)
    Serial.println("MQTT: Connect unavailable");
  else if (MQTTErr == MQTT_CONNECT_BAD_CREDENTIALS)
    Serial.println("MQTT: Connect bad credentials");
  else if (MQTTErr == MQTT_CONNECT_UNAUTHORIZED)
    Serial.println("MQTT: Connect unauthorized");
  else 
    Serial.println("MQTT: No error code");

  // SSL Error
  WiFiClientSecure wifiClient;  // Todo: use member pointer
  Serial.print("SSL Error Code: ");
  Serial.println(wifiClient.getLastSSLError());
}

void resetSensor() {
  Serial.println("MQTT: Conn timed out.");
  ESP.restart();
}



