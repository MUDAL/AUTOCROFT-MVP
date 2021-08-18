#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "conversions.h"
#include "hc12.h"
#include "communication.h"

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
	MIN_IRRIG_TIME = 10, /**Index 10 - 11 for minimum irrigation time*/
	MAX_IRRIG_TIME = 12 /**Index 12 - 13 for maximum irrigation time*/
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
	
	pNodeRx->minIrrigTime = (pNodeRx->data[MIN_IRRIG_TIME] << 8) | pNodeRx->data[MIN_IRRIG_TIME + 1];
	pNodeRx->maxIrrigTime = (pNodeRx->data[MAX_IRRIG_TIME] << 8) | pNodeRx->data[MAX_IRRIG_TIME + 1];
}

/**
@brief Error detector and corrector.  
The most common type of error encountered during wireless communication between  
master and node is the IDLE_CHARACTER_ERROR. Its due to the nature of the UART.  
This function detects this unwanted character and eliminates it immediately before  
it corrupts the actual data.
@param pNodeRx: pointer to the data structure containing data received from the master  
by the node.  
@return None.  
*/
void Node_RxErrorHandler(NodeRxDataStructure* pNodeRx)
{
	bool hc12DataIncomplete = HC12_IncompleteRxData();
	if((pNodeRx->data[0] == IDLE_CHARACTER)||hc12DataIncomplete)
	{
		HC12_RxBufferInit(pNodeRx->data, NODE_RX_DATA_SIZE);
	}			
}

/**
@brief Transmits moisture data to the master if the node ID received  
matches the assigned node ID (unique to each node).  
@param pNodeRx: pointer to the data structure containing data received from the master  
by the node.  
@param moisture: moisture data to be sent to the master once the node IDs match.  
@return None  
*/
void Node_TransmitData(NodeRxDataStructure* pNodeRx, uint8_t moisture)
{
	if(pNodeRx->nodeID == BASE_NODE_ID)
	{
		HC12_TransmitByte(moisture);
	}	
}
