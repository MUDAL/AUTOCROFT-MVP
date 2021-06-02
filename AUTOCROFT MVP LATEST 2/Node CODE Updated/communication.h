#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H

/*This module is responsible for encoding and decoding data
transfer between master and node.
*/

typedef enum
{
	MIN_MOISTURE = 0,
	MAX_MOISTURE = 1,
	MIN_HUMIDITY = 2,
	MAX_HUMIDITY = 3,
	MIN_TEMPERATURE = 4,
	MAX_TEMPERATURE = 5,
	HUMIDITY = 6,
	TEMPERATURE = 7,
	NODE_ID = 8,
	MIN_IRRIG_TIME = 9, /**Index 9 - 10 for minimum irrigation time*/
	MAX_IRRIG_TIME = 11 /**Index 11 - 12 for maximum irrigation time*/
}dataIndex_t;

typedef struct
{
	uint8_t TxData; /** Node Tx data member*/
	/**
	@brief Node Rx data members
	*/
	uint8_t RxData[HC12_RX_BUFFER_SIZE];
	uint8_t minMoist;
	uint8_t maxMoist;
	uint8_t minHum;
	uint8_t maxHum;
	uint8_t minTemp;
	uint8_t maxTemp;
	uint8_t humidity;
	uint8_t temperature;
	uint8_t nodeID;
	uint16_t minIrrigTime;
	uint16_t maxIrrigTime;
	uint32_t dataRxTimeMs;
}NodeDataStructure;


extern void Node_Init(NodeDataStructure* pNode);
extern uint16_t Node_DecodeRxMessage(NodeDataStructure* pNode, dataIndex_t dataIndex);

#endif //_COMMUNICATION_H
