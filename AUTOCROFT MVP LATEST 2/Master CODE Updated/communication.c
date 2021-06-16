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
@param pMasterRx:
@param rxLen:
@param rxTimeoutMs:
@return None
*/
void Master_TransmitReceive(uint8_t* pMasterTx,
														uint8_t txLen,
														uint8_t* pMasterRx,
														uint8_t rxLen,
														uint16_t rxTimeoutMs)
{
	uint8_t nodeID = 0;
	uint8_t state = 0;
	sysTimer_t masterRxTimer;
	
	System_TimerInit(&masterRxTimer, rxTimeoutMs);
	
	while(nodeID < rxLen)
	{
		switch(state)
		{
			case 0:
				Master_EncodeTxData(pMasterTx, nodeID, NODE_ID);
				HC12_TransmitBytes(pMasterTx, txLen);//send data to node
				state = 1;
				break;
			case 1:
				if(System_TimerDoneCounting(&masterRxTimer))
				{
					if (HC12_Rx_BufferFull())
					{
						pMasterRx[nodeID] = HC12_ReadByte();
					}
					else
					{
						pMasterRx[nodeID] = 200; //error value
					}
					nodeID++;
					state = 0;
				}
				break;
		}
	}
}

