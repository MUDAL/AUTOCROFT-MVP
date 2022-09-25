# AUTOCROFT-MVP

Autocroft is an automatic irrigation system designed and developed by **IoTeam**.

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

# Tested versions  
Version 1.0 and Version 1.1 
Each node will be assigned a subchannel of the 433MHz 
communication system. The master will switch from channel to channel in order to send and receive 
data from the nodes.

# How it works  
The master sends data to the nodes in order for them to make irrigation decisions. Irrigation decisions are made  
once every hour (under automatic control). The master and nodes have a runtime of 26 minutes and they sleep for  
34 minutes every hour.  

# Version 1.1  
Has similar features with version 1.0 but with the following changes:  
1. No Bluetooth and WiFi in the master controller.  
2. No EEPROM in the Nodes.  
3. Use of LCD with I2C GPIO expander for the master controller.  

# Link to documentation  
https://www.researchgate.net/publication/359510238_DESIGN_OF_A_LOW_POWER_AUTOMATIC_IRRIGATION_SYSTEM  

# Prototype  
![master_controller](https://user-images.githubusercontent.com/46250887/192158418-07bae23d-95b9-4a5f-b960-efba648698b8.jpg)
![node](https://user-images.githubusercontent.com/46250887/192158438-4ecc0b63-ca7b-4488-a67e-22b89585a235.jpg)

# 3D models of prototype PCB  
![Screenshot (229)](https://user-images.githubusercontent.com/46250887/192159274-96f814f6-ca0c-4d74-9928-ac080ac255e3.png)
![Screenshot (232)](https://user-images.githubusercontent.com/46250887/192159281-3b18d93f-7986-400c-9d23-10f9b267f04b.png)


  
