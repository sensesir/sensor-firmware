/*
*	Utility functions to be used across program
*
*	Author: Josh Perry <josh.perry245@gmail.com> 
*	Copyright 2019
*/

#ifndef UTILITIES_H
#define UTILITIES_H

#include <hardwareSerial.h>
#include <vector>
#include <string>
#include "../lib/ArduinoJSON/ArduinoJson.h"
#include "../config/Configuration.h"

void splitCharByDelimiter(char* input, const char delimiter[1], std::vector<std::string>* output);
void byteToCharArray(unsigned char *input, unsigned int inputLength, char *output);

#endif