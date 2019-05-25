/*
*	Singleton class to represent the sensor data 
*   structures and manage data persistance
*
*	Author: Josh Perry <josh.perry245@gmail.com>
*	Copyright 2019
*/

#ifndef SENSORMODEL_H
#define SENSORMODEL_H

#include <Arduino.h>
#include <EEPROM.h>
// #include <hardwareSerial.h>
#include <vector>
#include <string>
#include "../config/Configuration.h"

// Specific to current version of data persistance
const char DATA_START = 2;
const char DATA_END = 0;
const char CR = 13;
const char LF = 10;
const char JSON_SEPARATOR = 58;

class SensorModel {
    public:
        static SensorModel& getInstance();

        bool sensorDataLoaded = false;
        char wifiSSID[50];          // Arbitrary, but surely no one has an SSID/password longer than this?
        char wifiPassword[50];   

        void setModelSSID(const char* ssid);
        void setModelPassword(const char* password); 
        void writeModelDataToDisk();
        void clearEEPROM();
        bool wifiCredsAcquired();

    private: 
        // Singleton handling
        SensorModel();
        SensorModel(SensorModel const&);              // Don't Implement
        void operator=(SensorModel const&);           // Don't implement

        void initializeModel();
        void readModelDataFromDisk(std::vector< std::vector<std::string> > *modelData);
        void readKVPair(uint16_t *addressPtr, std::string *key, std::string *value);
        bool endRead(uint16_t *addressPtr);
        void writeKVPair(uint16_t *addressPtr, const char* key, char* value);
};

#endif
