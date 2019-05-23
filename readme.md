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

This project uses AWS IoT Core as an MQTT message broker, IoT fleet management console and dashboard. AWS requires each individual device (called a *Thing* in AWS parlance) to have unique SSL certs. For development, these can be acquired by the following steps: 

  1. Navigate to AWS IoT Core on the console - ensure region is **Ireland (eu-west-1)** (dev region).
  2. Go to the *Manage* tab and click *Create*, and create a single thing (this is for dev)
  3. The name of the thing will be it's UID, for dev it's ok to use a name with convention: `SensorXXDev` where XX is some identifier. Eg: `SensorJPDev`.
  4. Add the sensor to the type **GDoorSensor**, and to the **Development** group
  5. Then 'Next' - no need for searchable attributes at this stage
  6. **Certificates** (the important bit):
      * Choose: One-click certificate creation
      * You will then be taken to a page as displayed below where you will be able to download 3 files:
          * Thing certificate: *.pem.crt file
          * Public key:        *.pem.key file
          * Private key:       *.pem.key file
      * A standard AWS Root CA is also required, this can be reused from the secrets dir (need to affirm this though)
      * Copy the template file [AWSIoTSecrets.h](docs/templates/AWSIoTSecrets.h) from `docs/templates/AWSIoTSecrets.h` to `secrets/AWSIoTSecrets.h`. Do not remove the template file from it's place.
      * Follow the instructions in the file, filling in the necessary hashs in the areas provided with the downloaded certs + keys.
      * Ensure your certificate and key files **ARE NOT** committed to the repo on any branch
  7. **Policy**: This is AWS terminology for what permissions this device has. We require the device to be able to interface with AWS IoT Core. There is already a Dev policy setup: *GDoorDev*

![alt text](docs/img/aws-certs-download.png)

### Prod

In production the above 'signing' process needs to be automated to distribute at scale. 
Further research is required on how this can be achieved.

## Setting the Environment

Development will be carried out on different boards and in different contexts, hence it's imperative to maintain the correct config settings in modular files, as well as set the environment accordingly when you start work. (Most critically when we deploy!)

  1. Open the `Configuration.h` dir --> src/config/
  2. Change the ENV macro appropriately for your work, options:
      * PROD - production-ready firmware
      * DEV - development board with associated dev schematic [Chip = Wroom02]
      * NODE_DEV - Firmware being written on NodeMCU board (pinouts different from above)

## Git Workflow
---

todo

## Firmware functionality
---

### WiFi
#### Sequential process

#### Reconnection event

todo

### MQTT
#### Architecture & Abstraction

#### Messaging Structure

-- Format
-- Commands
-- Events

#### Database shadow

### GPIO / Hardware
#### Pin setting by ENV

#### Singleton Pattern

#### Noise filtering

### Error handling

TODO

### OTA updates

TODO

