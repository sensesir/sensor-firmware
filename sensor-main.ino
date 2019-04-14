/*
 * Garage Door IoT sensor main file using Arduino framework
 * 
 * Author: Josh Perry
 * Created: 04/13/2019
 */

#include "./src/config/Configuration.h"
#include "./src/networking/WifiInterface.hpp"

void setup() {
  Serial.begin(BAUD_RATE);
  Serial.println("Firing up Esp!!");
  

}

void loop() {
  // put your main code here, to run repeatedly:

}
