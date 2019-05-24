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

SensorModel::SensorModel() {
    // Private constructor
    Serial.println("MODEL: Initialising data structures");
    
    // Allows for hardcoding of sensorUID, wifi SSID, and wifi password for dev purposes
    #ifndef USE_DEV_MODEL_DATA
    this->initializeModel();
    #else 
    strcpy(this->wifiSSID, WIFI_SSID);
    strcpy(this->wifiPassword, WIFI_PASSWORD);
    this->sensorDataLoaded = true;
    #endif
}

void SensorModel::setModelSSID(std::string ssid) {
    strcpy(this->wifiSSID, ssid.c_str());
}

void SensorModel::setModelPassword(std::string password) {
    strcpy(this->wifiPassword, password.c_str());
}

void SensorModel::initializeModel() {
    std::vector< std::vector<std::string> > modelData;
    this->readModelDataFromDisk(&modelData);

    // Check if there is any data initialized
    if (modelData.empty()) {                        // Could be returning false
        Serial.println("MODEL: Data structures not init - left empty");
        this->sensorDataLoaded = false;
        return;
    }

    // Sort KV pairs according to expectation
    for (std::vector<std::string> dataPair : modelData) {
        std::string key = dataPair[0];
        
        if (key == KEY_SSID) {
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
	EEPROM.begin(512);

    // Check if there's any data written in our convention
    char startByte = EEPROM.read(0);
    char startCR   = EEPROM.read(1);
    char startLF   = EEPROM.read(2);

    bool startProtocol = (startByte == DATA_START) && (startCR == CR) && (startLF == LF);
    if (!startProtocol) {
        Serial.println("MODEL: No data to read");
        return;
    }

    // Data exists, read it in until end sequence is encountered
    Serial.println("MODEL: Found start bytes, continuing to read flash");
    std::string key;
    std::string value;
    uint16_t memAddress = 3;

    while (memAddress < 512) {
        this->readKVPair(&memAddress, &key, &value);
        std::vector<std::string> dataPair = {key, value};
        modelData->push_back(dataPair);
        
        if (this->endRead(&memAddress)) { break; }
        key.clear();
        value.clear();
    }

    EEPROM.end();    
}

void SensorModel::readKVPair(uint16_t *addressPtr, std::string *key, std::string *value) {
    // Serial.println("MODEL: Reading KV pair");
    uint16_t maxLoop = 512 - *addressPtr;
    
    // Serial.print("MODEL: Key => ");
    for (uint16_t i = 0; i < maxLoop; i++) {
        char byte = EEPROM.read(*addressPtr);

        if (byte == JSON_SEPARATOR) {
            *addressPtr += 1;
            break;
        }

        key->push_back(byte);
        // Serial.print(byte);
        *addressPtr += 1;
    }

    // Serial.println("");
    // Serial.print("MODEL: Value => ");
    maxLoop = 512 - *addressPtr;
    
    for (uint16_t j = 0; j < maxLoop; j++) {
        char byte = EEPROM.read(*addressPtr);

        if (byte == CR) {
            *addressPtr += 1;
            byte = EEPROM.read(*addressPtr);
            if (byte == LF) {
                *addressPtr += 1;
                // Serial.println("");
                return;
            } else {
                Serial.println("MODEL: Error in persistance interface protocol");
                // Todo: Persist error (ironic)
                return;
            }
        }

        value->push_back(byte);
        // Serial.print(byte);
        *addressPtr += 1;
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

void SensorModel::writeModelDataToDisk() {
    Serial.println("MODEL: Writing sensor model data to disk");
	EEPROM.begin(512);

    // Start protocol convention (custom)
    EEPROM.write(0, DATA_START);
    EEPROM.write(1, CR);
    EEPROM.write(2, LF);

    uint16_t memAddress = 3;
    this->writeKVPair(&memAddress, KEY_SSID, this->wifiSSID);
    this->writeKVPair(&memAddress, KEY_WIFI_PASSWORD, this->wifiPassword);

    // Terminating convention
    EEPROM.write(memAddress, DATA_END);
    EEPROM.write(memAddress+1, DATA_END);
    EEPROM.end();

    Serial.println("MODEL: Wrote data to disk");
}

void SensorModel::writeKVPair(uint16_t *addressPtr, const char* key, char* value) {
    Serial.println("MODEL: Writing KV pair to disk");
    Serial.print("MODEL: Key => ");

    for (int i = 0; i < strlen(key); i++) {
        EEPROM.write(*addressPtr, key[i]);
        Serial.print(key[i]);
        *addressPtr += 1;
    }

    // Add separator
    EEPROM.write(*addressPtr, JSON_SEPARATOR);
    *addressPtr += 1;

    Serial.println("");
    Serial.print("MODEL: Value => ");
    for (int i = 0; i < strlen(value); i++) {
        EEPROM.write(*addressPtr, value[i]);
        Serial.print(value[i]);
        *addressPtr += 1;
    }
    Serial.println("");

    // Terminator
    EEPROM.write(*addressPtr, CR);
    *addressPtr += 1;
    EEPROM.write(*addressPtr, LF);
    *addressPtr += 1;
}

void SensorModel::clearEEPROM() {
    EEPROM.begin(512);
    for (int i = 0 ; i < EEPROM.length() ; i++) {
        EEPROM.write(i, 0);
    }
    EEPROM.end();
    Serial.println("MODEL: Cleared EEPROM flash mem");
}
