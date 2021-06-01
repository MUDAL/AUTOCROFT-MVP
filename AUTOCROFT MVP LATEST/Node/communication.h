#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H

/*This module is responsible for encoding and decoding data
transfer between master and node.
*/

#define NODE_TX_MSG_SIZE						15

typedef struct
{
	uint8_t colonIndex;
	uint8_t commaIndex;
}message_t;

typedef struct
{
	char data[HC12_RX_BUFFER_SIZE];
	uint16_t nodeID;
	uint32_t dataRxTimeMs;
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
}NodeRxDataStructure;

typedef struct
{
	char data[NODE_TX_MSG_SIZE];
	message_t moistStruct;
}NodeTxDataStructure;

extern void Node_RxInit(NodeRxDataStructure* pNodeRx);
extern void Node_TxInit(NodeTxDataStructure* pNodeTx);
extern void Node_EncodeTxMessage(char* nodeToMasterData, message_t* pMsgStruct, uint16_t intData);
extern uint16_t Node_DecodeRxMessage(message_t* pMsgStruct, char* masterToNodeData);
extern bool Node_RxDataError(char* masterToNodeData);

#endif //_COMMUNICATION_H
