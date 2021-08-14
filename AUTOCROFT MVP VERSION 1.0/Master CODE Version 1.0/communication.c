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
	"AT+C006\r\n",
	"AT+C007\r\n",
	"AT+C008\r\n",
	"AT+C009\r\n",
	"AT+C010\r\n",
	"AT+C011\r\n",
	"AT+C012\r\n",
	"AT+C013\r\n",
	"AT+C014\r\n",
	"AT+C015\r\n",
	"AT+C016\r\n",
	"AT+C017\r\n",
	"AT+C018\r\n",
	"AT+C019\r\n",
	"AT+C020\r\n",
	"AT+C021\r\n",
	"AT+C022\r\n",
	"AT+C023\r\n",
	"AT+C024\r\n",
	"AT+C025\r\n",
	"AT+C026\r\n",
	"AT+C027\r\n",
	"AT+C028\r\n",
	"AT+C029\r\n",
	"AT+C030\r\n",
};

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
@param pMasterTx: pointer to master-to-node data
@param txLen: number of bytes of data the master transmits to the nodes
@param pMasterRxArray: array containing data received from all nodes
@param noOfNodes: number of nodes the master communicates with
@param noOfTimes: number of times the master communicates with each node
@return None
*/
void Master_TransmitReceive(uint8_t* pMasterTx,
														uint8_t txLen,
														uint8_t* pMasterRx,
														uint8_t* pMasterRxArray,
														uint8_t noOfNodes,
														uint8_t noOfTimes)
{
	uint8_t i = 0;
	ds3231_t rtc;

	while(i < noOfTimes)
	{
		uint8_t nodeID = 0;
		while(nodeID < noOfNodes)
		{
			//The master changes its own channel to match that of the node it wants..  
			//to communicate with.
			HC12_SetPinControl(false);
			System_TimerDelayMs(40);
			HC12_TransmitBytes(AT_Cmd[nodeID], AT_CMD_LEN);
			HC12_SetPinControl(true);
			System_TimerDelayMs(80);
			
			//Communication with node whose channel matches that of the master.  
			DS3231_GetTime(&rtc);
			Master_EncodeTxData(pMasterTx,rtc.hours,RTC_TIME_HOUR);
			Master_EncodeTxData(pMasterTx,rtc.minutes,RTC_TIME_MINUTE);
			Master_EncodeTxData(pMasterTx,nodeID,NODE_ID);
			HC12_TransmitBytes(pMasterTx,txLen);//send data to node
			
			while(!HC12_Rx_BufferFull()){};//wait for node to send its data
			if(*pMasterRx != IDLE_CHARACTER_ERROR)
			{
				pMasterRxArray[nodeID] = *pMasterRx;
			}	
			nodeID++;
		}
	}
}

