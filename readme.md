# Garage Door Sensor Firmware
---

This project contains the firmware for the Garage Door IoT sensor, currently running on an ESP8266 chip.

## Quick Start

Follow the instructions in the [setup.md file]('./docs/setup.md')

## Setting the Environment

Development will be carried out on different boards and in different contexts, hence it's imperative to maintain the correct config settings in modular files, as well as set the environment accordingly when you start work. (Most critically when we deploy!)

  1. Open the `Configuration.h` dir --> src/config/
  2. Change the ENV macro appropriately for your work, options:
      * PROD - production-ready firmware
      * DEV - development board with associated dev schematic
      * NODE_DEV - Firmware being written on NodeMCU board (pinouts different from above)
