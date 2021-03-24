#ifndef _MESSAGE_H
#define _MESSAGE_H

/*This module is responsible for encoding and decoding data
transfer between master and node.
*/

typedef struct
{
	uint8_t titleIndex;
	uint8_t colonIndex;
}message_t;

extern void Message_Element_Init(message_t* pMsgStruct, uint8_t titleIndx, uint8_t colonIndx);
extern void Message_Encode(message_t* pMsgStruct,uint16_t data,char* msgArray);


#endif //_MESSAGE_H
