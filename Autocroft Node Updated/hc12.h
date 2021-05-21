#ifndef _HC12_H
#define _HC12_H

#define HC12_RX_BUFFER_SIZE				104

extern void HC12_TxInit(void);
extern void HC12_Rx_BufferInit(char* pBuffer, uint32_t bufferSize);
extern void HC12_Transmit(char* pData);
extern bool HC12_Rx_BufferFull(void);

#endif //_HC12_H
