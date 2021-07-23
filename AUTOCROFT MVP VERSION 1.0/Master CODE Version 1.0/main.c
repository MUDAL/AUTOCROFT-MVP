#include <stdint.h>
#include <stdbool.h>
#include "conversions.h"
#include "system.h"
#include "bme280.h"
#include "eeprom24c16.h"
#include "hc12.h"
#include "keypad.h"
#include "lcd.h"
#include "ds3231.h"
#include "communication.h"
#include "wifi.h"
#include "bluetooth.h"
#include "hmi.h"


/**
EEPROM MEMORY ALLOCATION
* PAGE 1 to PAGE 6: Bluetooth data
* PAGE 10: Number of bytes received from bluetooth module
* PAGE 128: Master-to-node data
*/

/**
@brief Copies data from source buffer to target buffer. The buffer is an array  
of 8-bit integers.  
@param target: target buffer (recipient of data)  
@param source: source buffer (source of data)  
@param len: number of bytes in the buffers. The buffers must be of same length.  
@return None
*/
static void CopyData(uint8_t* target, uint8_t* source, uint8_t len)
{
	for(uint8_t i = 0; i < len; i++)
	{
		target[i] = source[i];
	}
}

int main(void)
{
	//STEPS
	//0.)Power external modules 
	//1.)Initializations
	//2.)Clear standby flag (after system wakeup)
	//3.)Read configuration data from EEPROM memory....
	//i.e. settings(min moisture, humidity etc) and WiFi ssid and password.
	//4.)Request for all node data and wait for data to be received from... 
	//all nodes.
	
	//Local variables
	static uint8_t nodeToMasterData;
	static uint8_t masterToNodeData[MASTER_TX_DATA_SIZE];
	static uint8_t bluetoothRxBuffer[BLUETOOTH_RX_MAX_LEN];
	static uint8_t nodeToMasterDataArray[NO_OF_NODES];
	static uint8_t prevMasterToNodeData[MASTER_TX_DATA_SIZE];
	static uint8_t prevBluetoothBuffer[BLUETOOTH_RX_MAX_LEN];
	//static uint8_t prevNoOfBluetoothBytes;
	//static sysTimer_t rtcTimer;
	//static ds3231_t rtc;
	
	//Initializations
	System_Init();
	Keypad_Init();
	LCD_Init();
	HC12_Init();
	HC12_RxBufferInit(&nodeToMasterData,MASTER_RX_DATA_SIZE);
	DS3231_Init();
	//DS3231_SetAlarm2(0);
	//System_TimerInit(&rtcTimer,60000); //check time every 60 seconds
	EEPROM_Init();
	//EEPROM_GetData(prevMasterToNodeData,MASTER_TX_DATA_SIZE,PAGE128);
	//EEPROM_GetData(&prevNoOfBluetoothBytes,1,PAGE10);
	//EEPROM_GetData(prevBluetoothBuffer,prevNoOfBluetoothBytes,PAGE1);
	WiFi_Init();
	BME280_Init();
	//Bluetooth_Init();
	//Bluetooth_RxBufferInit(bluetoothRxBuffer,BLUETOOTH_RX_MAX_LEN);
	//Load data from EEPROM memory
	CopyData(masterToNodeData,prevMasterToNodeData,MASTER_TX_DATA_SIZE);
	CopyData(bluetoothRxBuffer,prevBluetoothBuffer,BLUETOOTH_RX_MAX_LEN);
	HMI_Init(masterToNodeData,
					 &nodeToMasterData,
					 nodeToMasterDataArray);
	//System_ClearStandbyFlag();
	LCD_WriteString("AUTOCROFT");
	LCD_SetCursor(1,0);
	LCD_WriteString("FW: VER 1.0");
	System_TimerDelayMs(800);
	LCD_Clear();
	LCD_WriteString("Initializing....");
	LCD_SetCursor(1,0);
	LCD_WriteString("Please wait");
	Master_TransmitReceive(masterToNodeData,
											   MASTER_TX_DATA_SIZE,
												 &nodeToMasterData,
											   nodeToMasterDataArray,
											   NO_OF_NODES,
											   150);
	while(1)
	{
		/*
		HMI
		*/
		
		HMI_Execute();
		/*
		BLUETOOTH AND WIFI
		*/
		
//		if(Bluetooth_DataIsReady(bluetoothRxBuffer))
//		{
//			//1.)get number of bytes in the data received after allocated rx time elapses.
//			//2.)send the bluetooth rx data to the wifi module.
//			//3.)store bluetooth rx data in the EEPROM
//			//4.)reinitialize bluetooth rx buffer for another data reception.
//			uint8_t bluetoothRxBytes = Bluetooth_NumberOfBytesReceived();
//			WiFi_TransmitBytes(bluetoothRxBuffer,bluetoothRxBytes);
//			//EEPROM_StoreData(bluetoothRxBuffer,bluetoothRxBytes,PAGE1);
//			//EEPROM_StoreData(&bluetoothRxBytes,1,PAGE10);
//			Bluetooth_RxBufferInit(bluetoothRxBuffer,BLUETOOTH_RX_MAX_LEN);
//		}
		
		/*
		RTC AND SLEEP
		*/

//		if(System_TimerDoneCounting(&rtcTimer))
//		{
//			DS3231_GetTime(&rtc);
//			if(rtc.minutes >= 20)
//			{
//				//1.)store configuration data in EEPROM
					//2.)store data from nodes in EEPROM
//				//3.)put system to sleep
					//EEPROM_StoreData(masterToNodeData,MASTER_TX_DATA_SIZE,PAGE128);
//				//System_GoToStandbyMode();
//			}
//		}
		
	}
}


