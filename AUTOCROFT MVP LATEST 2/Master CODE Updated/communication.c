#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "conversions.h"
#include "hc12.h"
#include "communication.h"

//Private variables
/**
@brief template for data the master will transmit to nodes.
*/
static uint8_t masterTxDataTemplate[MASTER_TX_MSG_SIZE];
/**
@brief template for data the master expects from a node.
*/
static uint8_t masterRxDataTemplate;

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
void Master_TxInit(MasterTxDataStructure* pMasterTx)
{
	copyIntArray(pMasterTx->data, masterTxDataTemplate, MASTER_TX_MSG_SIZE);
	pMasterTx->nodeID = NEUTRAL_NODE_ID;
}

void Master_RxInit(MasterRxDataStructure* pMasterRx)
{
	pMasterRx->data = masterRxDataTemplate;
}

void Master_EncodeTxMessage(MasterTxDataStructure* pMasterTx, uint16_t data, dataIndex_t dataIndex)
{
	if(dataIndex > MAX_IRRIG_TIME)
	{//Invalid input
		return;
	}
	if(dataIndex != MIN_IRRIG_TIME && dataIndex != MAX_IRRIG_TIME)
	{
		pMasterTx->data[dataIndex] = data;
	}
	else
	{
		pMasterTx->data[dataIndex] = (data&0xFF00)>>8; //high byte
		pMasterTx->data[dataIndex + 1] = (data&0xFF); //low byte
	}
}

uint8_t Master_DecodeRxMessage(MasterRxDataStructure* pMasterRx)
{
	return pMasterRx->data;
}

