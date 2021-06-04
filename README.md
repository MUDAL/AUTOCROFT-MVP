# AUTOCROFT-MVP

Autocroft is an IoT-based automatic irrigation system designed and developed by **IoTeam**.

This codebase is an improved Master-Node Architecture based on one master and several base nodes.  
The base nodes collect data and forward them to the master.  
The master processes data from multiple base nodes and sends to the cloud.    

#Observations 
1. Keil debugger interferes with peripheral registers.    
2. LCD consumes considerable power.  
3. Faulty HC12 modules performed better when closer to each other.  

#Design Plans / Improvements  
1. Replacing buttons and potentiometer with keypad for improved longevity.  
2. Separate design based on WiFi and bluetooth (no user interface on the device).  
3. Use of IPEX/SMA antenna for HC12 modules.  

  


