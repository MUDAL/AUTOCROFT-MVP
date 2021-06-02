#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "conversions.h"
#include "hc12.h"
#include "communication.h"

//Private variables
/**
@brief template for data the node expects from the master.  
*/
static const uint8_t nodeRxDataTemplate[HC12_RX_BUFFER_SIZE];
/**
@brief template for data the node will transmit to the master.
*/
static const uint8_t nodeTxDataTemplate;

//Private functions
static void copyIntArray(uint8_t* pDest, const uint8_t* pSource, uint8_t length)
{
	if (length < 1)
	{//Invalid input
		return;
	}
	for(uint8_t i = 0; i < length; i++)
	{
		pDest[i] = pSource[i];
	}
}

//External functions
void Node_Init(NodeDataStructure* pNode)
{
	//Rx initialization
	copyIntArray(pNode->RxData, nodeRxDataTemplate, HC12_RX_BUFFER_SIZE);
	//Tx initialization
	pNode->TxData = nodeTxDataTemplate;
}

uint16_t Node_DecodeRxMessage(NodeDataStructure* pNode, dataIndex_t dataIndex)
{
	uint16_t masterToNodeData = 0;
	if(dataIndex > MAX_IRRIG_TIME)
	{//Invalid input
		return 0;
	}
	if(dataIndex != MIN_IRRIG_TIME && dataIndex != MAX_IRRIG_TIME)
	{
		masterToNodeData = pNode->RxData[dataIndex];
	}
	else
	{
		uint8_t upperByte = pNode->RxData[dataIndex];
		uint8_t lowerByte = pNode->RxData[dataIndex + 1];
		masterToNodeData = (upperByte<<8)|lowerByte;
	}
	return masterToNodeData;
}
