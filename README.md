# AUTOCROFT-MVP

Autocroft is an IoT-based automatic irrigation system designed and developed by **IoTeam**.

This codebase is an improved Master-Node Architecture based on one master and several base nodes.  
The base nodes collect data and forward them to the master.  
The master processes data from multiple base nodes and sends to the cloud.  

Some **keypad** functionalities:  
- A:  Forward  
- B:  Backward  
- C:  Up/Down  
- D:  Clear memory/Reset system  
- #:  Select 
- *:  Send data from master to the nodes

# Current version  
Version 1.0  
Each node will be assigned a subchannel of the 433MHz 
communication system. The master will switch from channel to channel in order to send and receive 
data from the nodes.

# How it works  
The master sends data to the nodes in order for them to make irrigation decisions. Irrigation decisions are made  
once every hour (under automatic control). The master and nodes have a runtime of 26 minutes and they sleep for  
34 minutes every hour.  

# Design Plans for next version (Version 1.1)  
1. MOSFET driver using LM358N: The MOSFET will be used to power external hardware down in order to improve power consumption.  
2. Replace voltage regulators with buck converters where necessary.  
3. Use an I2C adapter for the LCD to save GPIO pins and modify the LCD library without altering the interface.  
4. Small change to the DS3231 library to allow 'SetTime' function to set hours, minutes and seconds.  
5. Small change to the DS3231 library to remove 'SetMinutes' and use 'SetTime' instead.  
6. The ESP8266 isn't necessarily the only option for WiFi, ESP32 can be used in its place but provisions will be made to adapt it to PCB ver 1.0.  
7. A small veroboard adapter to link I2C connections on the PCB ver 1.0 of the master to the LCD I2C adapter.  
8. Firmware to be developed for the ESP8266/ESP32 for sending data to the cloud.  
9. A common API to configure all I2C devices(I2CBusConfig) instead of repeating the same function calls in their Init functions (e.g. EEPROM_Init,BME280_Init).  
10. Use of any keypad key to turn the LCD on. (LCD is powered down 2 minutes after turning on in order to save power).   
11. AT command multidimensional array (Increase the number of columns by 1 i.e. the number of characters in the AT command for choosing a channel). Make the array a const.  
12. Set maximum number of nodes to 40.  
  
# Design Plans for version 2.0  
1. Master controller redesign  
2. LCD and keypad user interface should be removed  
3. Shifting user interface functionality to a mobile app  
4. The app and master controller would communicate via bluetooth  

  
