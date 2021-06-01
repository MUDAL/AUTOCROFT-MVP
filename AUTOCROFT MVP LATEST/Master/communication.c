#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "conversions.h"
#include "hc12.h"
#include "communication.h"

//Private defines
#define NO_OF_MSG_STRUCT		11
/**
@brief Data from master to node and vice versa takes some time in air.  
This constant serves as a compensating factor for such a situation. It ensures  
the HC12 receiver doesn't expect data reception too early. This time is in milliseconds.  
*/
#define TIME_IN_AIR_MS			10

//Private variables
/**
@brief template for data the master will transmit to nodes.
*/
const char masterTxDataTemplate[MASTER_TX_MSG_SIZE] =
"mMin:0000,mMax:0000,hMin:0000,hMax:0000,tMin:0000,tMax:0000,iMin:0000,iMax:0000,h:0000,t:0000,nID:0000,";
/**
@brief template for data the master expects from a node.
*/
const char masterRxDataTemplate[HC12_RX_BUFFER_SIZE] = "moisture:0000,";
/**
@brief number of digit characters contained in the data the master  
receives from the nodes.
*/
static uint8_t masterRxNumberOfDigits;

//Private functions
static bool IsDigit(char ch)
{
	if(ch >= '0' && ch <= '9')
	{
		return true;
	}
	return false;
}

/**
@brief Initialize message structure. The value of each data element of the message  
is preceeded by a colon and has a comma after it.
@param pMsgStruct: pointer to message structure. A message structure  
contains the colon and comma indices of a data element of the master-to-node  
array
@param colonIndx: Index of colon of a data element 
@param commaIndx: Index of comma of a data element
@return None
*/
static void Message_Struct_Init(message_t* pMsgStruct, uint8_t colonIndx, uint8_t commaIndx)
{
	pMsgStruct->colonIndex = colonIndx;
	pMsgStruct->commaIndex = commaIndx;
}

//External functions
void Master_TxInit(MasterTxDataStructure* pMasterTx)
{
	//NB:Ensure HC12 is initialized before calling this function
	//Future modifications might have HC12 setups here
	uint8_t i = 0;
	uint8_t j = 0;
	uint8_t k = 0;
	pMasterTx->nodeID = NEUTRAL_NODE_ID;
	
	strcpy(pMasterTx->data,masterTxDataTemplate);
	
	message_t* pMsgStruct[NO_OF_MSG_STRUCT] = 
	{
		&pMasterTx->minMoistStruct,
		&pMasterTx->maxMoistStruct,
		&pMasterTx->minHumStruct,
		&pMasterTx->maxHumStruct,
		&pMasterTx->minTempStruct,
		&pMasterTx->maxTempStruct,
		&pMasterTx->minTimeStruct,
		&pMasterTx->maxTimeStruct,
		&pMasterTx->humStruct,
		&pMasterTx->tempStruct,
		&pMasterTx->nodeID_Struct
	};
	
	uint8_t colonIndexArr[NO_OF_MSG_STRUCT];
	uint8_t commaIndexArr[NO_OF_MSG_STRUCT];
	
	while(masterTxDataTemplate[i] != '\0')
	{
		if(masterTxDataTemplate[i] == ':')
		{
			colonIndexArr[j] = i;
			j++;
		}
		if(masterTxDataTemplate[i] == ',')
		{
			commaIndexArr[k] = i;
			k++;
		}
		i++;
	}
	
	for(uint8_t i = 0; i < NO_OF_MSG_STRUCT; i++)
	{
		Message_Struct_Init(pMsgStruct[i], colonIndexArr[i], commaIndexArr[i]);
	}
}

void Master_RxInit(MasterRxDataStructure* pMasterRx)
{
	uint8_t i = 0;
	uint8_t nodeColonIndex;
	uint8_t nodeCommaIndex;
	uint32_t baudRate = HC12_BaudRate();
	/*Data reception time formula:
	(Message_length * 8 * 1000 / baudrate)
	- X8 (8 bits in a byte)
	- X1000 (seconds to milliseconds)
	values are approximate and will be compensated
	*/
	if (HC12_RX_BUFFER_SIZE > 2)
	{
		pMasterRx->dataRxTimeMs = ((HC12_RX_BUFFER_SIZE - 1) * 8 * 1000 / baudRate);
		pMasterRx->dataRxTimeMs += TIME_IN_AIR_MS;
	}
	else if (HC12_RX_BUFFER_SIZE == 2)
	{
		//No need for compensating the time in air
		pMasterRx->dataRxTimeMs = 1; //1ms
	}
	else
	{
		//Invalid
		pMasterRx->dataRxTimeMs = 0;
	}
	
	/*Initialize master's receive buffer with the master Rx template
	i.e. node-to-master message(or data) template
	*/
	strcpy(pMasterRx->data,masterRxDataTemplate); 
	
	while(masterRxDataTemplate[i] != '\0')
	{
		if(masterRxDataTemplate[i] == ':')
		{
			nodeColonIndex = i;
		}
		if(masterRxDataTemplate[i] == ',')
		{
			nodeCommaIndex = i;
		}
		i++;
	}
	Message_Struct_Init(&pMasterRx->moistStruct, nodeColonIndex , nodeCommaIndex);
	//Get number of digits in the node's message
	for(uint8_t j = 0; j < HC12_RX_BUFFER_SIZE; j++)
	{
		if(IsDigit(masterRxDataTemplate[j]))
		{
			masterRxNumberOfDigits++;
		}
	}
}

void Master_EncodeTxMessage(char* masterToNodeData, message_t* pMsgStruct, uint16_t intData)
{
	/*strData is the string equivalent of 'intData'. The strData
	array must be cleared on entry into this function to ensure
	integer data is encoded without garbage. Garbage results
	from multiple calls to this function when the string equivalent
	of 'intData' fails to clear previous data.*/
	char strData[5] = {'\0','\0','\0','\0','\0'}; 
	
	Conv_IntegerToString(intData, strData);
	uint8_t dataLength = strlen(strData);
	
	switch(dataLength)
	{
		case 1:
			masterToNodeData[pMsgStruct->colonIndex + 1] = '0';
			masterToNodeData[pMsgStruct->colonIndex + 2] = '0';
			masterToNodeData[pMsgStruct->colonIndex + 3] = '0';
			masterToNodeData[pMsgStruct->colonIndex + 4] = strData[0];
			break;
		
		case 2:
			masterToNodeData[pMsgStruct->colonIndex + 1] = '0';
			masterToNodeData[pMsgStruct->colonIndex + 2] = '0';
			masterToNodeData[pMsgStruct->colonIndex + 3] = strData[0];
			masterToNodeData[pMsgStruct->colonIndex + 4] = strData[1];
			break;
		
		case 3:
			masterToNodeData[pMsgStruct->colonIndex + 1] = '0';
			masterToNodeData[pMsgStruct->colonIndex + 2] = strData[0];
			masterToNodeData[pMsgStruct->colonIndex + 3] = strData[1];
			masterToNodeData[pMsgStruct->colonIndex + 4] = strData[2];
			break;
		
		case 4:
			masterToNodeData[pMsgStruct->colonIndex + 1] = strData[0];
			masterToNodeData[pMsgStruct->colonIndex + 2] = strData[1];
			masterToNodeData[pMsgStruct->colonIndex + 3] = strData[2];
			masterToNodeData[pMsgStruct->colonIndex + 4] = strData[3];
			break;
	}
}

uint16_t Master_DecodeRxMessage(message_t* pMsgStruct, char* nodeToMasterData)
{
	char strData[5] = "0000";
	
	for(uint8_t i = pMsgStruct->colonIndex + 1; i < pMsgStruct->commaIndex; i++)
	{
		strData[i -(pMsgStruct->colonIndex + 1)] = nodeToMasterData[i];
	}
	uint16_t intData = Conv_StringToInteger(strData);
	return intData;
}

bool Master_RxDataError(char* nodeToMasterData)
{
	uint8_t i = 0;
	uint8_t noOfDigits = 0;
	bool commError = false;
	
	while(i < HC12_RX_BUFFER_SIZE)
	{
		if(!IsDigit(nodeToMasterData[i]))
		{
			if(nodeToMasterData[i] != masterRxDataTemplate[i])
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
	if(noOfDigits != masterRxNumberOfDigits)
	{
		commError = true;
	}
	return commError;
}
