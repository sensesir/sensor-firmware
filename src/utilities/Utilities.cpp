/*
*	Utility functions to be used across program
*
*	Author: Josh Perry <josh.perry245@gmail.com> 
*	Copyright 2019
*/

#include "Utilities.h"

void splitCharByDelimiter(char* input, const char delimiter[1], std::vector<std::string>* output) {
    std::string inputStr(input); // Deep copy
    size_t pos = 0;
    std::string content;

    do {
        pos = inputStr.find(delimiter);
        content = inputStr.substr(0, pos);
        output->push_back(content);         // Ensure deep copy (awaiting test)
        inputStr.erase(0, pos + 1);         
    } while (pos != std::string::npos);
}

void byteToCharArray(unsigned char *input, unsigned int inputLength, char *output) {
    for (int i=0; i < inputLength; i++) {
        output[i] = (char)input[i];
    }
}