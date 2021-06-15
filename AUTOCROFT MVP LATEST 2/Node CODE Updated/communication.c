#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "conversions.h"
#include "communication.h"

/**
@brief Each part of the data to be received is an element
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

static uint16_t Node_DecodeRxData(NodeRxDataStructure* pNodeRx, dataIndex_t dataIndex)
{
	uint16_t masterToNodeData = 0;

	if(dataIndex != MIN_IRRIG_TIME && dataIndex != MAX_IRRIG_TIME)
	{
		masterToNodeData = pNodeRx->data[dataIndex];
	}
	else
	{
		uint8_t upperByte = pNodeRx->data[dataIndex];
		uint8_t lowerByte = pNodeRx->data[dataIndex + 1];
		masterToNodeData = (upperByte<<8)|lowerByte;
	}
	return masterToNodeData;
}

void Node_StoreRxData(NodeRxDataStructure* pNodeRx)
{
	pNodeRx->minMoist = Node_DecodeRxData(pNodeRx,MIN_MOISTURE);
	pNodeRx->maxMoist = Node_DecodeRxData(pNodeRx,MAX_MOISTURE);
	
	pNodeRx->humidity= Node_DecodeRxData(pNodeRx,HUMIDITY);
	pNodeRx->minHum = Node_DecodeRxData(pNodeRx,MIN_HUMIDITY);
	pNodeRx->maxHum = Node_DecodeRxData(pNodeRx,MAX_HUMIDITY);
	
	pNodeRx->temperature = Node_DecodeRxData(pNodeRx,TEMPERATURE);
	pNodeRx->minTemp = Node_DecodeRxData(pNodeRx,MIN_TEMPERATURE);
	pNodeRx->maxTemp = Node_DecodeRxData(pNodeRx,MAX_TEMPERATURE);
	
	pNodeRx->nodeID = Node_DecodeRxData(pNodeRx,NODE_ID);
	
	pNodeRx->minIrrigTime = Node_DecodeRxData(pNodeRx,MIN_IRRIG_TIME);
	pNodeRx->maxIrrigTime = Node_DecodeRxData(pNodeRx,MAX_IRRIG_TIME);
}

