# Garage Door Sensor Firmware
---

This project contains the firmware for the Garage Door IoT sensor, currently running on an ESP8266 chip.

## Quick Start

Follow the instructions in the [setup.md file]('docs/setup.md') in order to ensure your machine can communicate with the MCU. There after: 

  1. Clone the repo - development branch
  2. Create the branch you need to work on (see Git workflow below)
  3. Ensure you have the requisite certs from AWS IoT core for the device you're working on. See section on AWS certs.
  4. Compile the code using the Arduino IDE

## AWS Certificates
### Dev

This project uses AWS IoT Core as an MQTT message broker, IoT fleet management console and dashboard. AWS requires each individual device (called a *Thing* in AWS parlance) to have unique SSL certs. For development, these can be acquired by the followin steps: 

  1. Navigate to AWS IoT core on the console - ensure region is **Ireland (eu-west-1)**.
  2. Go to the *Manage* tab and click *Create*, and create a single thing (this is for dev)
  3. The name of the thing will be it's UID, for dev it's ok to use a name with convention: `SensorXXDev` where XX is some identifier.
  4. Add the sensor to the type **GDoorSensor**, and add it to the **Development** group
  5. The 'Next' - no need for searchable attributes at this stage
  6. **Certificates** (the important bit):
      * Choose: One-click certificate creation
      * You will then be taken to a page as displayed below where you will be able to download 3 files:
          * Thing certificate: *.pem.crt file
          * Public key: .pem.key file
          * Private key: .pem.key file
      * A standard AWS Root CA is also required, this can 

### Prod

## Setting the Environment

Development will be carried out on different boards and in different contexts, hence it's imperative to maintain the correct config settings in modular files, as well as set the environment accordingly when you start work. (Most critically when we deploy!)

  1. Open the `Configuration.h` dir --> src/config/
  2. Change the ENV macro appropriately for your work, options:
      * PROD - production-ready firmware
      * DEV - development board with associated dev schematic [Chip = Wroom02]
      * NODE_DEV - Firmware being written on NodeMCU board (pinouts different from above)

## Git Workflow

