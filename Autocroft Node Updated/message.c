#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "conversions.h"
#include "hc12.h"
#include "message.h"

#define NO_OF_MSG_STRUCT				11

const char masterMsgTemplate[HC12_RX_BUFFER_SIZE] =
	"mMin:0000,mMax:0000,hMin:0000,hMax:0000,tMin:0000,tMax:0000,iMin:0000,iMax:0000,h:0000,t:0000,nID:0000,";

const char nodeMsgTemplate[NODE_TO_MASTER_MSG_SIZE] = "moisture:0000,";

static void Message_Struct_Init(message_t* pMsgStruct, uint8_t colonIndx, uint8_t commaIndx)
{
	pMsgStruct->colonIndex = colonIndx;
	pMsgStruct->commaIndex = commaIndx;
}

void Master_Message_Init(MasterMessageDataStructure* pMessage)
{
	uint8_t i = 0;
	uint8_t j = 0;
	uint8_t k = 0;
	
	message_t* pMsgStruct[NO_OF_MSG_STRUCT] = 
	{
		&pMessage->minMoisture,
		&pMessage->maxMoisture,
		&pMessage->minHumidity,
		&pMessage->maxHumidity,
		&pMessage->minTemperature,
		&pMessage->maxTemperature,
		&pMessage->minIrrigTime,
		&pMessage->maxIrrigTime,
		&pMessage->humidity,
		&pMessage->temperature,
		&pMessage->nodeID
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

void Node_Message_Init(NodeMessageDataStructure* pMessage)
{
	uint8_t i = 0;
	uint8_t nodeColonIndex;
	uint8_t nodeCommaIndex;
	
	strcpy(pMessage->dataToSend,nodeMsgTemplate);
	
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
		
	Message_Struct_Init(&pMessage->moisture, nodeColonIndex , nodeCommaIndex);
}

void Node_Message_Encode(char* nodeToMasterData, message_t* pMsgStruct, uint16_t intData)
{
	/*strData is the string equivalent of 'intData'. The strData
	array must be cleared on entry into this function to ensure
	integer data is encoded without garbage. Garbage results
	from multiple calls to this function when the string equivalent
	of 'intData' fails to clear previous data.*/
	char strData[5] = {'\0','\0','\0','\0','\0'}; //string version of integer data
	
	Conv_Integer_To_String(intData, strData);
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

uint16_t Master_Message_Decode(message_t* pMsgStruct, char* masterToNodeData)
{
	char strData[5] = "0000";
	
	for (uint8_t i = pMsgStruct->colonIndex + 1; i < pMsgStruct->commaIndex; i++)
	{
		strData[i - (pMsgStruct->colonIndex + 1)] = masterToNodeData[i];
	}
	
	uint16_t intData = Conv_String_To_Integer(strData);
	return intData;
}

