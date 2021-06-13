#ifndef _HC12_H
#define _HC12_H

extern void HC12_Init(void);
extern void HC12_TransmitBytes(uint8_t* bytes, uint8_t len);
extern bool HC12_Rx_BufferFull(void);
extern uint8_t HC12_ReadByte(void);

#endif //_HC12_H
