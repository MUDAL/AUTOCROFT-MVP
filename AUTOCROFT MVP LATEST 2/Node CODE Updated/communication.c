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

void Node_StoreRxData(NodeRxDataStructure* pNodeRx)
{
	pNodeRx->minMoist = pNodeRx->data[MIN_MOISTURE];
	pNodeRx->maxMoist = pNodeRx->data[MAX_MOISTURE];
	
	pNodeRx->humidity= pNodeRx->data[HUMIDITY];
	pNodeRx->minHum = pNodeRx->data[MIN_HUMIDITY];
	pNodeRx->maxHum = pNodeRx->data[MAX_HUMIDITY];
	
	pNodeRx->temperature = pNodeRx->data[TEMPERATURE];
	pNodeRx->minTemp = pNodeRx->data[MIN_TEMPERATURE];
	pNodeRx->maxTemp = pNodeRx->data[MAX_TEMPERATURE];
	
	pNodeRx->nodeID = pNodeRx->data[NODE_ID];
	
	pNodeRx->minIrrigTime = (pNodeRx->data[MIN_IRRIG_TIME] << 8) | pNodeRx->data[MIN_IRRIG_TIME + 1];
	pNodeRx->maxIrrigTime = (pNodeRx->data[MAX_IRRIG_TIME] << 8) | pNodeRx->data[MAX_IRRIG_TIME + 1];
}

