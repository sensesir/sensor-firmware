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

    PubSubClient client(wifiClient);
    client.setServer(AWS_IOT_DEVICE_GATEWAY, 8883);
    client.setCallback(callback);

    // Get reference to client
    this->mqttClient = &client;
}

bool SensorMQTT::connectDeviceGateway() {
    Serial.print("MQTT: Attempting to connect to AWS IoT Cloud -> ");
    Serial.println(AWS_IOT_DEVICE_GATEWAY);
    connected = this->mqttClient->connect(DEVICE_ID);

    if (connected) {
        Serial.println("MQTT: Successfully connected to AWS IoT Cloud");
        this->connected = true;
    } else {
        Serial.println("MQTT: Failed to connect to AWS IoT Cloud");
        this->pubSubError(this->mqttClient->state());
    }

    // Topic subscriptions
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

    Serial.println("MQTT: Time set");
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    Serial.print("MQTT: Current time: ");
    Serial.print(asctime(&timeinfo));
}

void SensorMQTT::pubSubError(int8_t MQTTErr) {
  if (MQTTErr == MQTT_CONNECTION_TIMEOUT)
    Serial.print("Connection tiemout");
  else if (MQTTErr == MQTT_CONNECTION_LOST)
    Serial.print("Connection lost");
  else if (MQTTErr == MQTT_CONNECT_FAILED)
    Serial.print("Connect failed");
  else if (MQTTErr == MQTT_DISCONNECTED)
    Serial.print("Disconnected");
  else if (MQTTErr == MQTT_CONNECTED)
    Serial.print("Connected");
  else if (MQTTErr == MQTT_CONNECT_BAD_PROTOCOL)
    Serial.print("Connect bad protocol");
  else if (MQTTErr == MQTT_CONNECT_BAD_CLIENT_ID)
    Serial.print("Connect bad Client-ID");
  else if (MQTTErr == MQTT_CONNECT_UNAVAILABLE)
    Serial.print("Connect unavailable");
  else if (MQTTErr == MQTT_CONNECT_BAD_CREDENTIALS)
    Serial.print("Connect bad credentials");
  else if (MQTTErr == MQTT_CONNECT_UNAUTHORIZED)
    Serial.print("Connect unauthorized");
}




