/*
*	Wrapper class to work with the OSS MQTT library for an ESP8266  
*   that uses the Arduino SDK
*
*	Author: Josh Perry <josh.perry245@gmail.com> 
*	Copyright 2019
*/

#include "SensorMQTT.hpp"

// Todo: Create static constructor that returns object instance
SensorMQTT::SensorMQTT(mqttMsgRecCallback callback) {
    Serial.println("MQTT: Instantiating MQTT client");

    // Set SSL certs
    WiFiClientSecure wifiClient;
    BearSSL::X509List cert(caCert);
    BearSSL::X509List client_crt(clientCert);
    BearSSL::PrivateKey key(privateKey);

    // Set time zone - necessary for AWS MQTT
    this->ntpConnect();

    wifiClient.setTrustAnchors(&cert);
    wifiClient.setClientRSACert(&client_crt, &key);

    // this->mqttClient = new PubSubClient(wifiClient);
    PubSubClient client(wifiClient);
    setMQTTClient(client);

    this->mqttClient->setServer(AWS_IOT_DEVICE_GATEWAY, 8883);
    this->mqttClient->setCallback(callback);

    // Attempt to connect | TODO: Pull out, needs to be invoked on it's own
    this->connectDeviceGateway();
    this->subscribeToTopics();
}

void SensorMQTT::connectDeviceGateway() {
    Serial.print("MQTT: Attempting to connect to AWS IoT Cloud -> ");
    Serial.println(AWS_IOT_DEVICE_GATEWAY);    
    this->connected = this->mqttClient->connect(DEVICE_ID);

    if (this->connected) {
      this->connected = true;
      Serial.println("MQTT: Successfully connected to AWS IoT Cloud");
    } else {
      this->connected = false;
      Serial.println("MQTT: Failed to connect to AWS IoT Cloud");
      this->pubSubError(mqttClient->state());
    }
}

void SensorMQTT::subscribeToTopics() {
  Serial.println("MQTT: Subscribing sensor client to topics");
  bool subscribed = this->mqttClient->subscribe(HEALTH_PING);

  if (subscribed) {
    Serial.println("MQTT: Subscribed to health ping");
  } else {
    Serial.println("MQTT: Failed to subscribe to health ping");
  }
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

int SensorMQTT::printState() {
  int currentState = this->mqttClient->state();
  this->pubSubError(currentState);
  return currentState;
}

SensorMQTT& SensorMQTT::setMQTTClient(PubSubClient& client) {
  this->mqttClient = &client;
  return *this;
}




