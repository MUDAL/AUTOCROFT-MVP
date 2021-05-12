#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "conversions.h"
#include "hc12.h"
#include "message.h"

#define NO_OF_MSG_STRUCT				11

enum MasterColonIndex
{
	MIN_MOISTURE_COLON_INDEX = 4,
	MAX_MOISTURE_COLON_INDEX = 14,
	MIN_HUMIDITY_COLON_INDEX = 24,
	MAX_HUMIDITY_COLON_INDEX = 34,
	MIN_TEMPERATURE_COLON_INDEX = 44,
	MAX_TEMPERATURE_COLON_INDEX = 54,
	MIN_IRRIG_TIME_COLON_INDEX = 64,
	MAX_IRRIG_TIME_COLON_INDEX = 74,
	HUMIDITY_COLON_INDEX = 81,
	TEMPERATURE_COLON_INDEX = 88,
	MST_TO_NODE_ID_COLON_INDEX = 97
};

const uint8_t colonIndexArr[NO_OF_MSG_STRUCT] = 
{
	MIN_MOISTURE_COLON_INDEX,
	MAX_MOISTURE_COLON_INDEX,
	MIN_HUMIDITY_COLON_INDEX,
	MAX_HUMIDITY_COLON_INDEX,
	MIN_TEMPERATURE_COLON_INDEX,
	MAX_TEMPERATURE_COLON_INDEX,
	MIN_IRRIG_TIME_COLON_INDEX,
	MAX_IRRIG_TIME_COLON_INDEX,
	HUMIDITY_COLON_INDEX,
	TEMPERATURE_COLON_INDEX,
	MST_TO_NODE_ID_COLON_INDEX
};

enum MasterCommaIndex
{
	MIN_MOISTURE_COMMA_INDEX = 9,
	MAX_MOISTURE_COMMA_INDEX = 19,
	MIN_HUMIDITY_COMMA_INDEX = 29,
	MAX_HUMIDITY_COMMA_INDEX = 39,
	MIN_TEMPERATURE_COMMA_INDEX = 49,
	MAX_TEMPERATURE_COMMA_INDEX = 59,
	MIN_IRRIG_TIME_COMMA_INDEX = 69,
	MAX_IRRIG_TIME_COMMA_INDEX = 79,
	HUMIDITY_COMMA_INDEX = 86,
	TEMPERATURE_COMMA_INDEX = 93,
	MST_TO_NODE_ID_COMMA_INDEX = 102
};

const uint8_t commaIndexArr[NO_OF_MSG_STRUCT] = 
{
	MIN_MOISTURE_COMMA_INDEX,
	MAX_MOISTURE_COMMA_INDEX,
	MIN_HUMIDITY_COMMA_INDEX,
	MAX_HUMIDITY_COMMA_INDEX,
	MIN_TEMPERATURE_COMMA_INDEX,
	MAX_TEMPERATURE_COMMA_INDEX,
	MIN_IRRIG_TIME_COMMA_INDEX,
	MAX_IRRIG_TIME_COMMA_INDEX,
	HUMIDITY_COMMA_INDEX,
	TEMPERATURE_COMMA_INDEX,
	MST_TO_NODE_ID_COMMA_INDEX
};


enum NodeIndex
{
	NODE_MOISTURE_COLON_INDEX = 8,
	NODE_MOISTURE_COMMA_INDEX = 13
};

const char nodeMsgTemplate[NODE_TO_MASTER_MSG_SIZE] = "moisture:0000,";

static void Message_Struct_Init(message_t* pMsgStruct, uint8_t colonIndx, uint8_t commaIndx)
{
	pMsgStruct->colonIndex = colonIndx;
	pMsgStruct->commaIndex = commaIndx;
}

void Master_Message_Init(MasterMessageDataStructure* pMessage)
{
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
	
	for (uint8_t i = 0; i < NO_OF_MSG_STRUCT; i++)
	{
		Message_Struct_Init(pMsgStruct[i], colonIndexArr[i], commaIndexArr[i]);
	}
}

void Node_Message_Init(NodeMessageDataStructure* pMessage)
{
	strcpy(pMessage->dataToSend,nodeMsgTemplate);
	Message_Struct_Init(&pMessage->moisture,NODE_MOISTURE_COLON_INDEX, NODE_MOISTURE_COMMA_INDEX);
}

void Node_Message_Encode(char* nodeToMasterData, message_t* pMsgStruct, uint16_t intData)
{
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

