#ifndef _HC12_H
#define _HC12_H

extern void HC12_Init(void);
extern void HC12_RxBufferInit(uint8_t* pBuffer, uint8_t bufferSize);
extern void HC12_TransmitByte(uint8_t data);
extern void HC12_TransmitBytes(uint8_t* bytes, uint8_t len);
extern bool HC12_RxBufferFull(void);
extern void HC12_SetPinControl(bool pinState);
extern uint8_t HC12_NumberOfBytes(void);
extern bool HC12_IncompleteRxData(void);

#endif //_HC12_H
