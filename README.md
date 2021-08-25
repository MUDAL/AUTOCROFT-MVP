# AUTOCROFT-MVP

Autocroft is an IoT-based automatic irrigation system designed and developed by **IoTeam**.

This codebase is an improved Master-Node Architecture based on one master and several base nodes.  
The base nodes collect data and forward them to the master.  
The master processes data from multiple base nodes and sends to the cloud.  

Some **keypad** functionalities:  
- 'A' Forward  
- 'B' Backward  
- 'C' Up/Down  
- 'D' Clear memory/Reset system  
- '#' Enter  
- '*' Send data from master to the nodes

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

  


