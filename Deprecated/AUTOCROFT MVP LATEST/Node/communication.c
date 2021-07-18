#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "conversions.h"
#include "hc12.h"
#include "communication.h"

#define NO_OF_MSG_STRUCT		11
/**
@brief Data from master to node and vice versa takes some time in air.  
This constant serves as a compensating factor for such a situation. It ensures  
the HC12 receiver doesn't expect data reception too early. This time is in milliseconds.  
*/
#define TIME_IN_AIR_MS			30 

//Private variables
/**
@brief template for data the node expects from the master.  
*/
const char nodeRxDataTemplate[HC12_RX_BUFFER_SIZE] =
"mMin:0000,mMax:0000,hMin:0000,hMax:0000,tMin:0000,tMax:0000,iMin:0000,iMax:0000,h:0000,t:0000,nID:0000,";
/**
@brief template for data the node will transmit to the master.
*/
const char nodeTxDataTemplate[NODE_TX_MSG_SIZE] = "moisture:0000,";
/**
@brief number of digit characters contained in the data the node  
receives from the master.
*/
static uint8_t nodeRxNumberOfDigits;

//Private functions
static bool IsDigit(char ch)
{
	if(ch >= '0' && ch <= '9')
	{
		return true;
	}
	return false;
}

static void Message_Struct_Init(message_t* pMsgStruct, uint8_t colonIndx, uint8_t commaIndx)
{
	pMsgStruct->colonIndex = colonIndx;
	pMsgStruct->commaIndex = commaIndx;
}

void Node_RxInit(NodeRxDataStructure* pNodeRx)
{
	uint8_t i = 0;
	uint8_t j = 0;
	uint8_t k = 0;
	uint32_t baudRate = HC12_BaudRate();
	/*Data reception time formula:
	(Message_length * 8 * 1000 / baudrate)
	- X8 (8 bits in a byte)
	- X1000 (seconds to milliseconds)
	values are approximate and will be compensated
	*/
	if (HC12_RX_BUFFER_SIZE > 2)
	{
		pNodeRx->dataRxTimeMs = ((HC12_RX_BUFFER_SIZE - 1) * 8 * 1000 / baudRate);
		pNodeRx->dataRxTimeMs += TIME_IN_AIR_MS;
	}
	else if (HC12_RX_BUFFER_SIZE == 2)
	{
		//No need for compensating the time in air
		pNodeRx->dataRxTimeMs = 1; //1ms
	}
	else
	{
		//Invalid
		pNodeRx->dataRxTimeMs = 0;
	}
	
	/*Initialize node's receive buffer with the node Rx template
	i.e. master-to-node message(or data) template
	*/
	strcpy(pNodeRx->data,nodeRxDataTemplate);
	
	message_t* pMsgStruct[NO_OF_MSG_STRUCT] = 
	{
		&pNodeRx->minMoistStruct,
		&pNodeRx->maxMoistStruct,
		&pNodeRx->minHumStruct,
		&pNodeRx->maxHumStruct,
		&pNodeRx->minTempStruct,
		&pNodeRx->maxTempStruct,
		&pNodeRx->minTimeStruct,
		&pNodeRx->maxTimeStruct,
		&pNodeRx->humStruct,
		&pNodeRx->tempStruct,
		&pNodeRx->nodeID_Struct
	};
	
	uint8_t colonIndexArr[NO_OF_MSG_STRUCT];
	uint8_t commaIndexArr[NO_OF_MSG_STRUCT];
	
	while(nodeRxDataTemplate[i] != '\0')
	{
		if (nodeRxDataTemplate[i] == ':')
		{
			colonIndexArr[j] = i;
			j++;
		}
		if (nodeRxDataTemplate[i] == ',')
		{
			commaIndexArr[k] = i;
			k++;
		}
		i++;
	}
	
	for (uint8_t i = 0; i < NO_OF_MSG_STRUCT; i++)
	{
		Message_Struct_Init(pMsgStruct[i], colonIndexArr[i], commaIndexArr[i]);
	}
	//Get number of digits in the master's message
	for(uint8_t j = 0; j < HC12_RX_BUFFER_SIZE; j++)
	{
		if(IsDigit(nodeRxDataTemplate[j]))
		{
			nodeRxNumberOfDigits++;
		}
	}
}

void Node_TxInit(NodeTxDataStructure* pNodeTx)
{
	uint8_t i = 0;
	uint8_t nodeColonIndex;
	uint8_t nodeCommaIndex;
	
	strcpy(pNodeTx->data,nodeTxDataTemplate);
	
	while(nodeTxDataTemplate[i] != '\0')
	{
		if (nodeTxDataTemplate[i] == ':')
		{
			nodeColonIndex = i;
		}
		if (nodeTxDataTemplate[i] == ',')
		{
			nodeCommaIndex = i;
		}
		i++;
	}
		
	Message_Struct_Init(&pNodeTx->moistStruct, nodeColonIndex , nodeCommaIndex);
}

void Node_EncodeTxMessage(char* nodeToMasterData, message_t* pMsgStruct, uint16_t intData)
{
	/*strData is the string equivalent of 'intData'. The strData
	array must be cleared on entry into this function to ensure
	integer data is encoded without garbage. Garbage results
	from multiple calls to this function when the string equivalent
	of 'intData' fails to clear previous data.*/
	char strData[5] = {'\0','\0','\0','\0','\0'}; //string version of integer data
	
	Conv_IntegerToString(intData, strData);
	uint8_t dataLength = strlen(strData);
	
	switch (dataLength)
	{
		case 1:
			nodeToMasterData[pMsgStruct->colonIndex + 1] = '0';
			nodeToMasterData[pMsgStruct->colonIndex + 2] = '0';
			nodeToMasterData[pMsgStruct->colonIndex + 3] = '0';
			nodeToMasterData[pMsgStruct->colonIndex + 4] = strData[0];
			break;
		
		case 2:
			nodeToMasterData[pMsgStruct->colonIndex + 1] = '0';
			nodeToMasterData[pMsgStruct->colonIndex + 2] = '0';
			nodeToMasterData[pMsgStruct->colonIndex + 3] = strData[0];
			nodeToMasterData[pMsgStruct->colonIndex + 4] = strData[1];
			break;
		
		case 3:
			nodeToMasterData[pMsgStruct->colonIndex + 1] = '0';
			nodeToMasterData[pMsgStruct->colonIndex + 2] = strData[0];
			nodeToMasterData[pMsgStruct->colonIndex + 3] = strData[1];
			nodeToMasterData[pMsgStruct->colonIndex + 4] = strData[2];
			break;
		
		case 4:
			nodeToMasterData[pMsgStruct->colonIndex + 1] = strData[0];
			nodeToMasterData[pMsgStruct->colonIndex + 2] = strData[1];
			nodeToMasterData[pMsgStruct->colonIndex + 3] = strData[2];
			nodeToMasterData[pMsgStruct->colonIndex + 4] = strData[3];
			break;
	}
}

uint16_t Node_DecodeRxMessage(message_t* pMsgStruct, char* masterToNodeData)
{
	char strData[5] = "0000";
	
	for (uint8_t i = pMsgStruct->colonIndex + 1; i < pMsgStruct->commaIndex; i++)
	{
		strData[i - (pMsgStruct->colonIndex + 1)] = masterToNodeData[i];
	}
	uint16_t intData = Conv_StringToInteger(strData);
	return intData;
}

bool Node_RxDataError(char* masterToNodeData)
{
	uint8_t i = 0;
	uint8_t noOfDigits = 0;
	bool commError = false;
	
	while(i < HC12_RX_BUFFER_SIZE)
	{
		if(!IsDigit(masterToNodeData[i]))
		{
			if(masterToNodeData[i] != nodeRxDataTemplate[i])
			{
				commError = true;
				break;
			}
		}
		else
		{
			noOfDigits++;
		}
		i++;
	}
	if(noOfDigits != nodeRxNumberOfDigits)
	{
		commError = true;
	}
	return commError;
}
