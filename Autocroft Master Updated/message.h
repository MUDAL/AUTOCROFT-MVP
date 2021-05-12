#ifndef _MESSAGE_H
#define _MESSAGE_H

/*This module is responsible for encoding and decoding data
transfer between master and node.
*/

#define MASTER_TO_NODE_MSG_SIZE						104
#define NO_OF_NODES												100

typedef struct
{
	uint8_t colonIndex;
	uint8_t commaIndex;
}message_t;

typedef struct
{
	char dataToSend[MASTER_TO_NODE_MSG_SIZE];
	uint8_t nodeIDvalue;
	message_t minMoisture;
	message_t maxMoisture;
	message_t minHumidity;
	message_t maxHumidity;
	message_t minTemperature;
	message_t maxTemperature;
	message_t minIrrigTime;
	message_t maxIrrigTime;
	message_t humidity;
	message_t temperature;
	message_t nodeID;
}MasterMessageDataStructure;

typedef struct
{
	uint8_t moistureArr[NO_OF_NODES + 1];
	char data[HC12_RX_BUFFER_SIZE];
	message_t moisture;
}NodeMessageDataStructure;

extern void Master_Message_Init(MasterMessageDataStructure* pMessage);
extern void Node_Message_Init(NodeMessageDataStructure* pMessage);
extern void Master_Message_Encode(char* masterToNodeData, message_t* pMsgStruct, uint16_t intData);
extern uint16_t Node_Message_Decode(message_t* pMsgStruct, char* nodeToMasterData);

#endif //_MESSAGE_H
