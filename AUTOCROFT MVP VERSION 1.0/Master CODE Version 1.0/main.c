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
@brief Source code for Autocroft Master  
*/

/**
EEPROM MEMORY ALLOCATION
* PAGE 1 to PAGE 6: Bluetooth data
* PAGE 10: Number of bytes received from bluetooth module
* PAGE 128: Master-to-node data
*/

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
	static uint8_t nodeToMasterDataArray[NO_OF_NODES];
	static sysTimer_t rtcTimer;
	static ds3231_t rtc;
	static bluetooth_t bluetooth;

	//Initializations
	System_Init();
	EEPROM_Init();
	EEPROM_GetData(masterToNodeData,MASTER_TX_DATA_SIZE,PAGE128);
	EEPROM_GetData(&bluetooth.noOfRxBytes,1,PAGE10);
	EEPROM_GetData(bluetooth.rxBuffer,bluetooth.noOfRxBytes,PAGE1);
	Keypad_Init();
	LCD_Init();
	HC12_Init();
	HC12_RxBufferInit(&nodeToMasterData,MASTER_RX_DATA_SIZE);
	DS3231_Init();
	DS3231_24HourFormat(); 
	//Set minutes to 0 by default
	DS3231_SetMinutes(0); 
	//Alarm to wake the system up every time the system is at 0 minutes. e.g. 9:00, 11:00
	DS3231_SetAlarm2(0);
	//60 seconds periodic software timer
	System_TimerInit(&rtcTimer,60000); 
	WiFi_Init();
	BME280_Init();
	Bluetooth_Init();
	Bluetooth_RxBufferInit(bluetooth.rxBuffer,BLUETOOTH_RX_MAX_LEN);
	HMI_Init(masterToNodeData,&nodeToMasterData,nodeToMasterDataArray);
	System_ClearStandbyFlag();
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
											   NO_OF_NODES);
	while(1)
	{
		/*
		HMI
		*/
		HMI_Execute();
		/*
		BLUETOOTH AND WIFI
		*/
		bluetooth.rxStatus = Bluetooth_RxStatus();
		if(bluetooth.rxStatus != NO_DATA_RECEIVED)
		{
			LCD_Clear();
			LCD_WriteString("Credentials");
			LCD_SetCursor(1,0);
			LCD_WriteString("Received");
			System_TimerDelayMs(1000);
			LCD_Clear();
			switch(bluetooth.rxStatus)
			{
				case COMPLETE_RX_DATA:
					bluetooth.noOfRxBytes = BLUETOOTH_RX_MAX_LEN;
					break;
				case INCOMPLETE_RX_DATA:
					bluetooth.noOfRxBytes = Bluetooth_NumberOfBytesReceived();
					break;
			}
			WiFi_TransmitBytes(bluetooth.rxBuffer,bluetooth.noOfRxBytes);
		  EEPROM_StoreData(bluetooth.rxBuffer,bluetooth.noOfRxBytes,PAGE1);
			EEPROM_StoreData(&bluetooth.noOfRxBytes,1,PAGE10);
			Bluetooth_RxBufferInit(bluetooth.rxBuffer,BLUETOOTH_RX_MAX_LEN);
		}
		/*
		RTC AND SLEEP
		*/
		if(System_TimerDoneCounting(&rtcTimer))
		{
			DS3231_GetTime(&rtc);
			if(rtc.minutes >= SYSTEM_RUNTIME_MINUTES)
			{
				//1.)store configuration data in EEPROM
				//2.)store data from nodes in EEPROM
				//3.)put system to sleep
				EEPROM_StoreData(masterToNodeData,MASTER_TX_DATA_SIZE,PAGE128); //1
				System_GoToStandbyMode(); //3
			}
		}
	}
}


