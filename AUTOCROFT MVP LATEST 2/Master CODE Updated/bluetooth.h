#ifndef _BLUETOOTH_H
#define _BLUETOOTH_H

#define BLUETOOTH_RX_MAX_LEN				96

extern void Bluetooth_Init(void);
extern void Bluetooth_RxBufferInit(uint8_t* pBuffer, uint8_t bufferSize);
extern bool Bluetooth_DataIsReady(uint8_t* pBuffer);
extern uint8_t Bluetooth_NumberOfBytesReceived(void);
extern void Bluetooth_ClearRxBuffer(uint8_t* pBuffer);

#endif //_BLUETOOTH_H
