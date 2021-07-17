#include <stdint.h>
#include <stdbool.h>
#include "system.h"
#include "hc12.h"
#include "communication.h"

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

/**
@brief Sends data to all nodes, receives and stores  
their responses.  
@param pMasterTx:
@param txLen:
@param pMasterRxArray:
@param rxLen:
@param rxTimeoutMs:
@return None
*/
void Master_TransmitReceive(uint8_t* pMasterTx,
														uint8_t txLen,
														uint8_t* pMasterRx,
														uint8_t* pMasterRxArray,
														uint8_t rxLen,
														uint16_t rxTimeoutMs)
{
	uint8_t nodeID = 0;

	while(nodeID < rxLen)
	{
		Master_EncodeTxData(pMasterTx, nodeID, NODE_ID);
		HC12_TransmitBytes(pMasterTx, txLen);//send data to node
		System_TimerDelayMs(rxTimeoutMs);
		
		if(HC12_Rx_BufferFull())
		{
			if(*pMasterRx != IDLE_CHARACTER_ERROR)
			{
				pMasterRxArray[nodeID] = *pMasterRx;
			}	
		}
		nodeID++;
	}
}

