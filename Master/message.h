#ifndef _MESSAGE_H
#define _MESSAGE_H

/*This module is responsible for encoding and decoding data
transfer between master and node.
*/

#define MASTER_TO_NODE_MSG_SIZE						95
#define NODE_TO_MASTER_MSG_SIZE						95

enum ColonIndex
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
	TEMPERATURE_COLON_INDEX = 88
};

enum CommaIndex
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
	TEMPERATURE_COMMA_INDEX = 93
};

typedef struct
{
	uint8_t colonIndex;
	uint8_t commaIndex;
}message_t;

extern void Message_Init(char* msgArray);
extern void Message_Struct_Init(message_t* pMsgStruct, uint8_t colonIndx, uint8_t commaIndx);
extern void Message_Encode(message_t* pMsgStruct,uint16_t data,char* msgArray);
extern uint16_t Message_Decode(message_t* pMsgStruct, char* msgArray);

#endif //_MESSAGE_H
