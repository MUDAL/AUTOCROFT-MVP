#include <stdint.h>
#include <stdbool.h>
#include "system.h"
#include "hc12.h"
#include "ds3231.h"
#include "communication.h"

#define AT_CMD_LEN		9
/**
AT Command lookup table for selecting a channel of the HC12 module in  
order to communicate with a particular node.
*/
static uint8_t AT_Cmd[NO_OF_NODES][AT_CMD_LEN] = {
	"AT+C001\r\n",
	"AT+C002\r\n",
	"AT+C003\r\n",
	"AT+C004\r\n",
	"AT+C005\r\n",
};

void Master_EncodeTxData(uint8_t* pMasterTx, uint16_t data, dataIndex_t dataIndex)
{
	if(dataIndex > (MASTER_TX_DATA_SIZE - 1))
	{//Invalid input
		return;
	}
	else
	{
		pMasterTx[dataIndex] = data;
	}
}

/**
@brief Sends data to all nodes, receives and stores  
their responses.  
@param pMasterTx: pointer to master-to-node data
@param txLen: number of bytes of data the master transmits to the nodes
@param pMasterRxArray: array containing data received from all nodes
@param noOfNodes: number of nodes the master communicates with
@return None
*/
void Master_TransmitReceive(uint8_t* pMasterTx,
														uint8_t txLen,
														uint8_t* pMasterRx,
														uint8_t* pMasterRxArray,
														uint8_t noOfNodes)
{
	uint8_t nodeID = 0;
	ds3231_t rtc;
	
	while(nodeID < noOfNodes)
	{
		HC12_SetPinControl(false);
		System_TimerDelayMs(40);
		HC12_TransmitBytes(AT_Cmd[nodeID], AT_CMD_LEN);
		HC12_SetPinControl(true);
		System_TimerDelayMs(80);
		if(*pMasterRx != 0)
		{//Remnants of AT commands can corrupt rx data
			//if corrupted data is detected, reinitialize Rx buffer
			*pMasterRx = 0;
			HC12_RxBufferInit(pMasterRx,MASTER_RX_DATA_SIZE);
		}
		
		DS3231_GetTime(&rtc);
		Master_EncodeTxData(pMasterTx,rtc.minutes,RTC_TIME_MINUTE);
		Master_EncodeTxData(pMasterTx, nodeID, NODE_ID);
		HC12_TransmitBytes(pMasterTx, txLen);//send data to node
		System_TimerDelayMs(100); //wait for feedback from node
		if(*pMasterRx != IDLE_CHARACTER)
		{
			pMasterRxArray[nodeID] = *pMasterRx;
		}
		nodeID++;
	}
}

