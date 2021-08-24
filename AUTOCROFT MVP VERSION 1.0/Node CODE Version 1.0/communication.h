#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H

/*This module is responsible for encoding and decoding data
transfer between master and node.
*/

/**
An idle character frame is always sent whenever the the USART  
transmitter is initialized for the first time. This 8-bit data  
is unwanted and must be eliminated/ignored once received.  
*/
#define IDLE_CHARACTER							255
#define NODE_RX_DATA_SIZE						13

typedef struct
{
	uint8_t data[NODE_RX_DATA_SIZE];
	uint8_t minMoist;
	uint8_t maxMoist;
	uint8_t minHum;
	uint8_t maxHum;
	uint8_t minTemp;
	uint8_t maxTemp;
	uint8_t humidity;
	uint8_t temperature;
	uint8_t nodeID;
	uint8_t rtcMinute;
	uint8_t minIrrigTime;
	uint8_t maxIrrigTime;
	uint8_t clrMemory;
}NodeRxDataStructure;

extern void Node_StoreRxData(NodeRxDataStructure* pNodeRx);

#endif //_COMMUNICATION_H
