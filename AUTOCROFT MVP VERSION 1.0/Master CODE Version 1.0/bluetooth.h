#ifndef _BLUETOOTH_H
#define _BLUETOOTH_H

#define BLUETOOTH_RX_MAX_LEN				96

enum BluetoothRxStatus
{
	NO_DATA_RECEIVED = 0,
	COMPLETE_RX_DATA,
	INCOMPLETE_RX_DATA
};

typedef struct
{
	uint8_t rxBuffer[BLUETOOTH_RX_MAX_LEN];
	uint8_t noOfRxBytes;
	uint8_t rxStatus;
}bluetooth_t;

extern void Bluetooth_Init(void);
extern void Bluetooth_RxBufferInit(uint8_t* pBuffer, uint8_t bufferSize);
extern uint8_t Bluetooth_RxStatus(void);
extern uint8_t Bluetooth_NumberOfBytesReceived(void);

#endif //_BLUETOOTH_H
