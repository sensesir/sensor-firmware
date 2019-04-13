# IoT Garage Door Sensor setup
---

This document descibes how to set up the V1.0.0 Garage Door IoT Sensor, which is build using the ESP8266 WiFi MCU. 

## Drivers
### Mac

There is a lot of contention on online forums around whether Mac OSx 14.0+ (Mojave) requires serial-usb drivers at all, most opinions seem to suggest not. This was **not** the case for me. I needed the drivers. Following the steps below to install drivers: 

  1. Unload the old drivers: 
    * `sudo kextunload /Library/Extensions/usbserial.kext` 
    * `sudo kextunload /System/Library/Extensions/usb.kext`
  2. Remove old drivers: 
    * `sudo rm -rf /System/Library/Extensions/usb.kext`
    * `sudo rm -rf /Library/Extensions/usbserial.kext`
  3. Download and install [this package](http://www.wch.cn/downloads/file/178.html).
  4. Force quite the installer - no reboot *should* be required.
  5. Load the new driver: `sudo kextload /Library/Extensions/usbserial.kext`
  6. Plug the ESP in, open up terminal and enter `ls /dev/tty.*`, the chip should show up now.

This [Github repo](https://github.com/adrianmihalko/ch340g-ch34g-ch34x-mac-os-x-driver) was super useful in getting the ESP to show up.

If you have trouble, please get in touch with Josh.

### Windows

TBC

## Arduino IDE

Setting up the arduino IDE should be very simple. Open a project (it should generate a .ino file), then follow the steps: 

  1. Select *tools* --> *boards*. If you're working on a NodeMCU board, select `NodeMCU 0.9 (ESP12-module)`, else if you're on a standalone ESP12 chip, select `Generic ESP8266 Module`. [You may have to download the ESP board set for Arduino].
  2. It should select all options correctly, but ensure Baud rate = 115200

## Testing

Write a simple program to flash an LED. In the Arduino framework this should be trivial. 
