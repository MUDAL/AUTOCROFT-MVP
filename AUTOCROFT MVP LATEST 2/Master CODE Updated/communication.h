#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H

/*This module is responsible for encoding and decoding data
transfer between master and node.
*/

#define MASTER_TX_MSG_SIZE		13
#define NEUTRAL_NODE_ID				0 /*valid node ID starts from 1*/
#define NO_OF_NODES						101

/**
@brief Each part of the data to be transmitted is an element
of an array of bytes. e.g. minimum moisture is the first element  
of the array with an index of 0, maximum humidity is the 4th element  
of the array with an index of 3.
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
	uint8_t data[MASTER_TX_MSG_SIZE];
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
}MasterTxDataStructure;

typedef struct
{
	uint8_t data;
	uint8_t moistureArr[NO_OF_NODES];
}MasterRxDataStructure;

extern void Master_EncodeTxData(MasterTxDataStructure* pMasterTx, uint16_t data, dataIndex_t dataIndex);

#endif //_COMMUNICATION_H
