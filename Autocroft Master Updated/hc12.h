#ifndef _HC12_H
#define _HC12_H

#define HC12_RX_BUFFER_SIZE				15

extern void HC12_Tx_Init(void);
extern void HC12_Rx_Buffer_Init(char* pBuffer, uint32_t bufferSize);
extern void HC12_Transmit(char* pData);
extern bool HC12_Rx_Buffer_Full(void);

#endif //_HC12_H
