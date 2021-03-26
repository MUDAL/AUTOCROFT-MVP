#include <stdint.h>
#include <string.h>
#include "conversions.h"
#include "message.h"

//The commented blocks of code aren't needed yet for the node.
//The node to master message structure is yet to be developed.

//const char MsgTemplate[NODE_TO_MASTER_MSG_SIZE];

//void Message_Init(char* msgArray)
//{
//	strcpy(msgArray, MsgTemplate);
//}

void Message_Struct_Init(message_t* pMsgStruct, uint8_t colonIndx, uint8_t commaIndx)
{
	pMsgStruct->colonIndex = colonIndx;
	pMsgStruct->commaIndex = commaIndx;
}

//void Message_Encode(message_t* pMsgStruct,uint16_t intData,char* msgArray)
//{
//	
//	char strData[5] = {'\0','\0','\0','\0','\0'}; //string version of integer data
//	Conv_Integer_To_String(intData, strData);
//	uint8_t dataLength = strlen(strData);
//	
//	switch (dataLength)
//	{
//		case 1:
//			msgArray[pMsgStruct->colonIndex + 1] = '0';
//			msgArray[pMsgStruct->colonIndex + 2] = '0';
//			msgArray[pMsgStruct->colonIndex + 3] = '0';
//			msgArray[pMsgStruct->colonIndex + 4] = strData[0];
//			break;
//		
//		case 2:
//			msgArray[pMsgStruct->colonIndex + 1] = '0';
//			msgArray[pMsgStruct->colonIndex + 2] = '0';
//			msgArray[pMsgStruct->colonIndex + 3] = strData[0];
//			msgArray[pMsgStruct->colonIndex + 4] = strData[1];
//			break;
//		
//		case 3:
//			msgArray[pMsgStruct->colonIndex + 1] = '0';
//			msgArray[pMsgStruct->colonIndex + 2] = strData[0];
//			msgArray[pMsgStruct->colonIndex + 3] = strData[1];
//			msgArray[pMsgStruct->colonIndex + 4] = strData[2];
//			break;
//		
//		case 4:
//			msgArray[pMsgStruct->colonIndex + 1] = strData[0];
//			msgArray[pMsgStruct->colonIndex + 2] = strData[1];
//			msgArray[pMsgStruct->colonIndex + 3] = strData[2];
//			msgArray[pMsgStruct->colonIndex + 4] = strData[3];
//			break;
//	}
//}

uint16_t Message_Decode(message_t* pMsgStruct, char* msgArray)
{
	char strData[5] = {'\0','\0','\0','\0','\0'};
	
	for (uint8_t i = pMsgStruct->colonIndex; i < pMsgStruct->commaIndex; i++)
	{
		strData[i - pMsgStruct->colonIndex] = msgArray[i];
	}
	
	uint16_t intData = Conv_String_To_Integer(strData);
	return intData;
}

