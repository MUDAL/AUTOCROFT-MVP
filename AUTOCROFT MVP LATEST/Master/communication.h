#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H

/*This module is responsible for encoding and decoding data
transfer between master and node.
*/

#define MASTER_TX_MSG_SIZE		104
#define NEUTRAL_NODE_ID				0 /*valid node ID starts from 1*/
#define NO_OF_NODES						101

typedef struct
{
	uint8_t colonIndex;
	uint8_t commaIndex;
}message_t;

typedef struct
{
	char data[MASTER_TX_MSG_SIZE];
	uint16_t nodeID;
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
}MasterTxDataStructure;

typedef struct
{
	uint8_t moistureArr[NO_OF_NODES];
	uint32_t dataRxTimeMs;
	char data[HC12_RX_BUFFER_SIZE];
	message_t moistStruct;
}MasterRxDataStructure;

extern void Master_TxInit(MasterTxDataStructure* pMasterTx);
extern void Master_RxInit(MasterRxDataStructure* pMasterRx);
extern void Master_EncodeTxMessage(char* masterToNodeData, message_t* pMsgStruct, uint16_t intData);
extern uint16_t Master_DecodeRxMessage(message_t* pMsgStruct, char* nodeToMasterData);
extern bool Master_RxDataError(char* nodeToMasterData);

#endif //_COMMUNICATION_H
