/*
*	Wrapper class to work with the OSS MQTT library for an ESP8266  
*   that uses the Arduino SDK
*
*	Author: Josh Perry <josh.perry245@gmail.com> 
*	Copyright 2019
*/

/*   NOTE: May migrate away from class because PubSub client message received callback cannot take a member function (method) type
*    Will see how code develops
*/

#ifndef SENSORMQTT_H
#define SENSORMQTT_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <time.h>
#include <string>
#include "../lib/PubSubClient/PubSubClient.h"
#include "../lib/ArduinoJSON/ArduinoJson.h"
#include "../config/Configuration.h"
#include "../../secrets/AWSIoTSecrets.h"
#include "../utilities/Utilities.h"
#include "../digital-io/GDoorIO.hpp"

typedef void (*mqttMsgRecCallback)(char*, byte*, unsigned int);

class SensorMQTT: public PubSubClient{
    public:
        SensorMQTT();
        bool initializeMQTT(mqttMsgRecCallback callback);
        bool connectDeviceGateway();
        bool reconnectClientSync();
        bool subscribeToTopics();
        
        // Publish methods
        void publishBootEvent(bool firstBoot);
        void publishDoorState(); 
        void publishReconnection();
        void publishHealth();
        void publishError(const char* message);
        void publishUnknownTypeError(std::string unknownType, std::string identifier);
        bool verifyTargetUID(char *payload, std::string *sensorUID); 
        void deserializeStdPayload(char* payload, std::string *sensorUID); 

    private:
        WiFiClientSecure wifiClient; // Needs to be persisted
        // void publishMessage(char* topic, const JsonObject& payload); [Re-implement when possible]
        void generateTopic(char* topic,const char* target, const char* targetUID, const char* msgCategory, const char* descriptor);
        void pubSubError(int8_t MQTTErr);
        void ntpConnect();
};

#endif