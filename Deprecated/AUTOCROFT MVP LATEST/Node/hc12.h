#ifndef _HC12_H
#define _HC12_H

#define HC12_RX_BUFFER_SIZE				104

extern void HC12_TxInit(void);
extern void HC12_RxBufferInit(char* pBuffer, uint32_t bufferSize);
extern void HC12_Transmit(char* pData);
extern bool HC12_Rx_BufferFull(void);
extern uint32_t HC12_BaudRate(void);
extern uint16_t HC12_NumberOfBytesToReceive(void);

#endif //_HC12_H
