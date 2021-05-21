#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "conversions.h"
#include "hc12.h"
#include "message.h"

//Private defines
#define NO_OF_MSG_STRUCT				11

const char masterMsgTemplate[MASTER_TO_NODE_MSG_SIZE] =
	"mMin:0000,mMax:0000,hMin:0000,hMax:0000,tMin:0000,tMax:0000,iMin:0000,iMax:0000,h:0000,t:0000,nID:0000,";

const char nodeMsgTemplate[HC12_RX_BUFFER_SIZE] = "moisture:0000,";

static void Message_Struct_Init(message_t* pMsgStruct, uint8_t colonIndx, uint8_t commaIndx)
{
	pMsgStruct->colonIndex = colonIndx;
	pMsgStruct->commaIndex = commaIndx;
}

void Master_MessageInit(MasterMessageDataStructure* pMessage)
{
	uint8_t i = 0;
	uint8_t j = 0;
	uint8_t k = 0;
	
	pMessage->nodeID = 0;
	strcpy(pMessage->data,masterMsgTemplate);
	
	message_t* pMsgStruct[NO_OF_MSG_STRUCT] = 
	{
		&pMessage->minMoistStruct,
		&pMessage->maxMoistStruct,
		&pMessage->minHumStruct,
		&pMessage->maxHumStruct,
		&pMessage->minTempStruct,
		&pMessage->maxTempStruct,
		&pMessage->minTimeStruct,
		&pMessage->maxTimeStruct,
		&pMessage->humStruct,
		&pMessage->tempStruct,
		&pMessage->nodeID_Struct
	};
	
	uint8_t colonIndexArr[NO_OF_MSG_STRUCT];
	uint8_t commaIndexArr[NO_OF_MSG_STRUCT];
	
	while(masterMsgTemplate[i] != '\0')
	{
		if (masterMsgTemplate[i] == ':')
		{
			colonIndexArr[j] = i;
			j++;
		}
		if (masterMsgTemplate[i] == ',')
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
}

void Node_MessageInit(NodeMessageDataStructure* pMessage)
{
	uint8_t i = 0;
	uint8_t nodeColonIndex;
	uint8_t nodeCommaIndex;
	
	while(nodeMsgTemplate[i] != '\0')
	{
		if (nodeMsgTemplate[i] == ':')
		{
			nodeColonIndex = i;
		}
		if (nodeMsgTemplate[i] == ',')
		{
			nodeCommaIndex = i;
		}
		i++;
	}
		
	Message_Struct_Init(&pMessage->moistStruct, nodeColonIndex , nodeCommaIndex);
}

void Master_MessageEncode(char* masterToNodeData, message_t* pMsgStruct, uint16_t intData)
{
	/*strData is the string equivalent of 'intData'. The strData
	array must be cleared on entry into this function to ensure
	integer data is encoded without garbage. Garbage results
	from multiple calls to this function when the string equivalent
	of 'intData' fails to clear previous data.*/
	char strData[5] = {'\0','\0','\0','\0','\0'}; 
	
	Conv_IntegerToString(intData, strData);
	uint8_t dataLength = strlen(strData);
	
	switch (dataLength)
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

uint16_t Node_MessageDecode(message_t* pMsgStruct, char* nodeToMasterData)
{
	char strData[5] = "0000";
	
	for (uint8_t i = pMsgStruct->colonIndex + 1; i < pMsgStruct->commaIndex; i++)
	{
		strData[i - (pMsgStruct->colonIndex + 1)] = nodeToMasterData[i];
	}
	
	uint16_t intData = Conv_StringToInteger(strData);
	return intData;
}

