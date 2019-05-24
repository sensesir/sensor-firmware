/**
*	Singleton class to represent the sensor data 
*   structures and manage data persistance
*
*	Author: Josh Perry <josh.perry245@gmail.com>
*	Copyright 2019
*/

#include "SensorModel.hpp"

// Singleton handling
SensorModel& SensorModel::getInstance() {
    static SensorModel instance;
    return instance;
}

SensorModel::SensorModel() {}

void SensorModel::initializeModel() {
    std::vector< std::vector<std::string> > modelData;
    this->readModelDataFromDisk(&modelData);

    // Check if there is any data initialized
    if (modelData.empty()) {
        this->sensorDataLoaded = false;
        return;
    }

    // Sort KV pairs according to expectation
    for (std::vector<std::string> dataPair : modelData) {
        std::string key = dataPair[0];
        if (key == KEY_SENSOR_UID) {
            strcpy(this->sensorUID, dataPair[1].c_str());
        } 

        else if (key == KEY_SSID) {
            strcpy(this->wifiSSID, dataPair[1].c_str());
        } 
        
        else if (key == KEY_WIFI_PASSWORD) {
            strcpy(this->wifiPassword, dataPair[1].c_str());
        }

        else {
            Serial.print("MODEL: Error unknown persisted key => ");
            Serial.println(key.c_str());
        }
    }

    this->sensorDataLoaded = true;
}

void SensorModel::readModelDataFromDisk(std::vector< std::vector<std::string> > *modelData) {
    Serial.println("MODEL: Attempting to read sensor model data from disk");
	EEPROM.begin(512);

    // Check if there's any data written in our convention
    char startByte = EEPROM.read(0);
    char startCR   = EEPROM.read(1);
    char startLF   = EEPROM.read(2);

    if (!(startByte == DATA_START && startCR == CR && startLF == LF)) {
        Serial.println("MODEL: No data to read");
        return;
    }

    // Data exists, read it in until end sequence is encountered
    std::string key;
    std::string value;
    uint16_t memAddress = 3;

    while (memAddress < 513) {
        this->readKVPair(&memAddress, &key, &value);
        std::vector<std::string> dataPair = {key, value};
        modelData->push_back(dataPair);

        if (this->endRead(&memAddress)) { break; }
    }    
}

void SensorModel::readKVPair(uint16_t *addressPtr, std::string *key, std::string *value) {
    bool isKey = true;
    uint16_t maxLoop = 512 - *addressPtr;
    
    for (int i = 0; i <  maxLoop; i++) {
        char byte = EEPROM.read(*addressPtr);

        if (byte == CR) {
            *addressPtr += 1;
            byte = EEPROM.read(*addressPtr);
            if (byte == LF) {
                *addressPtr += 1;
                return;
            } else {
                Serial.println("MODEL: Error in persistance interface protocol");
                // Todo: Persist error (ironic)
                return;
            }
        }

        if (byte == JSON_SEPARATOR) {
            *addressPtr += 1;
            isKey = false;
            continue;
        }

        if (isKey) { key->push_back(byte); }
        else { value->push_back(byte); }
    }
}

bool SensorModel::endRead(uint16_t *addressPtr) {
    char terminatorOne = EEPROM.read(*addressPtr);
    char terminatorTwo = EEPROM.read(*addressPtr + 1);

    if (terminatorOne == DATA_END && terminatorTwo == DATA_END) {
        return true;
    } else {
        return false;
    }
}

