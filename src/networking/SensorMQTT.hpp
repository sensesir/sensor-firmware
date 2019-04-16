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
#include "../lib/PubSubClient/PubSubClient.h"
#include "../config/Configuration.h"
#include "../../secrets/AWSIoTSecrets.h"

typedef void (*mqttMsgRecCallback)(char*, byte*, unsigned int);

class SensorMQTT{
    public:
        SensorMQTT(mqttMsgRecCallback callback);
        bool connected = false;
        void connectDeviceGateway();
        void subscribeToTopics();
        int printState(); 
        PubSubClient *mqttClient;

    private:
        void pubSubError(int8_t MQTTErr);
        void ntpConnect();
        SensorMQTT& setMQTTClient(PubSubClient& client); 
};

#endif