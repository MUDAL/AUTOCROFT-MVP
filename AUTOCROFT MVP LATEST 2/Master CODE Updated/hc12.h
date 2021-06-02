#ifndef _HC12_H
#define _HC12_H

#define HC12_RX_BUFFER_SIZE				1

extern void HC12_TxInit(void);
extern void HC12_RxBufferInit(uint8_t* pBuffer, uint8_t bufferSize);
extern void HC12_TransmitBytes(uint8_t* bytes, uint8_t len);
extern bool HC12_Rx_BufferFull(void);
extern uint32_t HC12_BaudRate(void);
extern uint16_t HC12_NumberOfBytesToReceive(void);

#endif //_HC12_H
