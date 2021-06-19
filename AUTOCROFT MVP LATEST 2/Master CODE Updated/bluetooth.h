#ifndef _BLUETOOTH_H
#define _BLUETOOTH_H

#define BLUETOOTH_RX_MAX_LEN				96
#define BLUETOOTH_RX_MAX_TIME_MS		100

extern void Bluetooth_Init(void);
extern void Bluetooth_RxBufferInit(uint8_t* pBuffer, uint8_t bufferSize);
extern uint8_t Bluetooth_NumberOfBytesReceived(void);
extern void Bluetooth_RxReInit(void);

#endif //_BLUETOOTH_H
