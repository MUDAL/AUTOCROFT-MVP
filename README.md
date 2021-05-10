# AUTOCROFT-MVP

Autocroft is an IoT-based automatic irrigation system designed and developed by IoTeam.

This codebase is an improved Master-Node Architecture based on one master and several base nodes.  
The base nodes collect data and forward them to the master.  
The master processes data from multiple base nodes and sends to the cloud.

# CHANGES TO MAKE  
1.) HC12 to use non-blocking polling for DMA  
2.) HC12 software timer code should be deprecated since message size is known and fixed.  


