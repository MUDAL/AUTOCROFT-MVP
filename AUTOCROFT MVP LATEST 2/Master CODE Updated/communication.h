#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H

/*This module is responsible for encoding and decoding data
transfer between master and node.
*/

#define MASTER_TX_DATA_SIZE		13
#define NO_OF_NODES						101

/**
@brief Each part of the data to be transmitted is an element
of an array of bytes. e.g. minimum moisture is the first element  
of the array with an index of 0, maximum humidity is the 4th element  
of the array with an index of 3.
*/
typedef enum
{
	MIN_MOISTURE = 0,
	MAX_MOISTURE = 1,
	MIN_HUMIDITY = 2,
	MAX_HUMIDITY = 3,
	MIN_TEMPERATURE = 4,
	MAX_TEMPERATURE = 5,
	HUMIDITY = 6,
	TEMPERATURE = 7,
	NODE_ID = 8,
	MIN_IRRIG_TIME = 9, /**Index 9 - 10 for minimum irrigation time*/
	MAX_IRRIG_TIME = 11 /**Index 11 - 12 for maximum irrigation time*/
}dataIndex_t;

extern void Master_EncodeTxData(uint8_t* pMasterTx, uint16_t data, dataIndex_t dataIndex);
extern void Master_SetNodeID(uint8_t id);
extern uint8_t Master_GetNodeID(void);
extern void Master_StoreNodeData(uint8_t id, uint8_t nodeData);
extern uint8_t Master_GetNodeData(uint8_t id);

#endif //_COMMUNICATION_H
