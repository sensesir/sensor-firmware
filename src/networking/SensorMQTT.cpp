/*
*	  Wrapper class to work with the OSS MQTT library for an ESP8266  
*   that uses the Arduino SDK
*
*	  Collaborator(s): Josh Perry <josh.perry245@gmail.com> 
*	  Copyright 2019
*/

#include "SensorMQTT.hpp"

SensorMQTT::SensorMQTT() {
  // Just to init super class
}

void SensorMQTT::initializeMQTT(mqttMsgRecCallback callback) {
    Serial.println("MQTT: Instantiating MQTT client");

    // Set SSL certs
    BearSSL::X509List cert(caCert);
    BearSSL::X509List client_crt(clientCert);
    BearSSL::PrivateKey key(privateKey);
    wifiClient.setTrustAnchors(&cert);
    wifiClient.setClientRSACert(&client_crt, &key);
    setClient(wifiClient);

    // Set time zone - necessary for AWS MQTT [todo: investigate why]
    this->ntpConnect();
    this->setServer(AWS_IOT_DEVICE_GATEWAY, 8883);
    this->setCallback(callback);

    /*
     * Todo: Work out what resource on this function stack is needed by 
     * the connect method. When called outside of this scope cases a crash.
     */
    this->connectDeviceGateway();
}

void SensorMQTT::connectDeviceGateway() {
    Serial.print("MQTT: Attempting to connect to AWS IoT Cloud -> ");
    Serial.println(AWS_IOT_DEVICE_GATEWAY);    
    bool success = this->connect(DEVICE_ID);

    if (success) {
      Serial.println("MQTT: Successfully connected to AWS IoT Cloud");
    } else {
      Serial.println("MQTT: Failed to connect to AWS IoT Cloud");
      this->pubSubError(this->state());
    }
}

void SensorMQTT::subscribeToTopics() {
  Serial.println("MQTT: Subscribing sensor client to topics");
  std::vector<const char*> SUBSCRIBE_TOPICS = {SUB_ACTUATE, SUB_HEALTH_PING};

  for (int i=0; i < SUBSCRIBE_TOPICS.size(); i++) {
    bool subscribed = this->subscribe(SUBSCRIBE_TOPICS[i]);

    if (subscribed) {
      Serial.print("MQTT: Subscribed to ");
      Serial.println(SUBSCRIBE_TOPICS[i]);
    } else {
      Serial.print("MQTT: Failed to subscribe to ");
      Serial.println(SUBSCRIBE_TOPICS[i]);
      
      // TODO: throw error here - sensor shouldn't initialize with failed subscription
      // Write to EEPROM
      // Assert false for crash
      // OR on test bed have an error LED that indicates error state [can then go investigate logs]
    }
  }
}

void SensorMQTT::publishBootEvent(bool firstBoot) {
  const char* topic = this->generateTopic(SERVER, AWS_IOT_DEVICE_GATEWAY, EVENT, PUB_FIRST_BOOT);
  
  // Create payload using JSON lib
  const int capacity = JSON_OBJECT_SIZE(3) + 120;     // 3 KV pairs + 120 bytes spare for const input duplication
  StaticJsonDocument<capacity> payload;
  payload[KEY_SENSOR_UID] = SENSOR_UID;
  payload[KEY_FIRMWARE_VERSION] = FIRMWARE_VERSION; 
  if (firstBoot) {
    payload[KEY_EVENT] = PUB_FIRST_BOOT;
  } else {
    payload[KEY_EVENT] = PUB_BOOT;
  }
  serializeJson(payload, Serial);

  // Serialize JSON into char
  int jsonLength = measureJson(payload);
  char serializedPayload[256];
  serializeJson(payload, serializedPayload);

  bool success = this->publish(topic, serializedPayload);
  if(success) {
    Serial.println("MQTT: Published boot message");
  } else {
    Serial.println("MQTT: Failed to publish boot message");
  }
}

char* SensorMQTT::generateTopic(const char* target, const char* targetUID, const char* msgCategory, const char* descriptor) {
  char topic[256];
  strcpy(topic, target);
  strcpy(topic, "/");         // Not computationally optimal, but traded for constants coherence
  strcpy(topic, targetUID);
  strcpy(topic, "/");
  strcpy(topic, msgCategory);
  strcpy(topic, "/");
  strcpy(topic, descriptor);
  return topic;               // Ensure memory for topic doesn't get deallocated ** Suspect it will
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
  WiFiClientSecure wifiClient;
  Serial.print("SSL Error Code: ");
  Serial.println(wifiClient.getLastSSLError());
}




