#ifndef _MESSAGE_H
#define _MESSAGE_H

/*This module is responsible for encoding and decoding data
transfer between master and node.
*/

#define NODE_TO_MASTER_MSG_SIZE						15

typedef struct
{
	uint8_t colonIndex;
	uint8_t commaIndex;
}message_t;

typedef struct
{
	char data[HC12_RX_BUFFER_SIZE];
	message_t minMoistStruct;
	message_t maxMoistStruct;
	message_t minHumStruct;
	message_t maxHumStruct;
	message_t minTempStruct;
	message_t maxTempStruct;
	message_t minTimeStruct;
	message_t maxTimeStruct;
	message_t humStruct;
	message_t tempStruct;
	message_t nodeID_Struct;
}MasterMessageDataStructure;

typedef struct
{
	char data[NODE_TO_MASTER_MSG_SIZE];
	message_t moistStruct;
}NodeMessageDataStructure;

extern void Master_MessageInit(MasterMessageDataStructure* pMessage);
extern void Node_MessageInit(NodeMessageDataStructure* pMessage);
extern void Node_MessageEncode(char* nodeToMasterData, message_t* pMsgStruct, uint16_t intData);
extern uint16_t Master_MessageDecode(message_t* pMsgStruct, char* masterToNodeData);

#endif //_MESSAGE_H
