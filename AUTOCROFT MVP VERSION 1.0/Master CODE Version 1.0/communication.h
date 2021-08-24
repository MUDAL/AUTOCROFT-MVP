#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H

/*This module is responsible for encoding and decoding data
transfer between master and node.
*/

/**
An idle character frame is always sent whenever the the USART  
transmitter is initialized for the first time. This 8-bit data  
is unwanted and must be eliminated/ignored once received.  
*/
#define IDLE_CHARACTER				255
#define MASTER_TX_DATA_SIZE		13
#define MASTER_RX_DATA_SIZE		1
#define NO_OF_NODES						5

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
	RTC_TIME_MINUTE = 9,
	MIN_IRRIG_TIME = 10, 
	MAX_IRRIG_TIME = 11,
	SYS_AND_MEM = 12 /** system and memory control*/
}dataIndex_t;

extern void Master_EncodeTxData(uint8_t* pMasterTx, uint16_t data, dataIndex_t dataIndex);
//This function blocks other operations until the query of all nodes is complete.
extern void Master_TransmitReceive(uint8_t* pMasterTx,
																	 uint8_t txLen,
																	 uint8_t* pMasterRx,
																	 uint8_t* pMasterRxArray,
																	 uint8_t noOfNodes);

#endif //_COMMUNICATION_H
