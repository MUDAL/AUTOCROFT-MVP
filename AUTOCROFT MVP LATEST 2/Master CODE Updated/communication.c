#include <stdint.h>
#include "communication.h"

static uint8_t nodeID;
static uint8_t masterRxArr[NO_OF_NODES];

void Master_EncodeTxData(uint8_t* pMasterTx, uint16_t data, dataIndex_t dataIndex)
{
	if(dataIndex > MAX_IRRIG_TIME)
	{//Invalid input
		return;
	}
	if(dataIndex != MIN_IRRIG_TIME && dataIndex != MAX_IRRIG_TIME)
	{
		pMasterTx[dataIndex] = data;
	}
	else
	{
		pMasterTx[dataIndex] = (data&0xFF00)>>8; //high byte
		pMasterTx[dataIndex + 1] = (data&0xFF); //low byte
	}
}

void Master_SetNodeID(uint8_t id)
{
	nodeID = id;
}

uint8_t Master_GetNodeID(void)
{
	return nodeID;
}

/**
@brief Stores moisture data received from all nodes in  
an array.
*/
void Master_StoreNodeData(uint8_t id, uint8_t nodeData)
{
	masterRxArr[id] = nodeData;
}

/**
@brief Gets data for a particular node by using the ID of  
that node.
*/
uint8_t Master_GetNodeData(uint8_t id)
{
	return masterRxArr[id];
}
