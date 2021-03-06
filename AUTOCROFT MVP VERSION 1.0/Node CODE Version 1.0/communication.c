#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "conversions.h"
#include "hc12.h"
#include "communication.h"
#include "stm32f10x.h"                  // Device header

/**
@brief Each part of the data to be received is an element
of an array of bytes. e.g. minimum moisture is the first element  
of the array with an index of 0, maximum humidity is the 4th element  
of the array with an index of 3.
*/
enum
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
	RTC_TIME_MINUTE = 9,
	MIN_IRRIG_TIME = 10, 
	MAX_IRRIG_TIME = 11,
	SYS_AND_MEM = 12 /** system and memory control*/
};

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
	pNodeRx->rtcMinute = pNodeRx->data[RTC_TIME_MINUTE];
	pNodeRx->minIrrigTime = pNodeRx->data[MIN_IRRIG_TIME];
	pNodeRx->maxIrrigTime = pNodeRx->data[MAX_IRRIG_TIME];
	pNodeRx->sysMemControl = pNodeRx->data[SYS_AND_MEM];
}
