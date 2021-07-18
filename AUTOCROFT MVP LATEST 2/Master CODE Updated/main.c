#include <stdint.h>
#include <stdbool.h>
#include "conversions.h"
#include "system.h"
#include "bme280.h"
#include "eeprom24c16.h"
#include "hc12.h"
#include "lcd.h"
#include "ds3231.h"
#include "potentiometer.h"
#include "button.h"
#include "communication.h"
#include "wifi.h"
#include "bluetooth.h"
#include "hmi.h"

/**
@Description Autocroft master.  
Next development:
*Drivers for a keypad to replace potentiometer and buttons
*/

int main(void)
{
	//Local variables
	static ButtonDataStructure button;
	static uint8_t nodeToMasterData;
	static uint8_t masterToNodeData[MASTER_TX_DATA_SIZE];
	static uint8_t nodeToMasterDataArray[NO_OF_NODES];
	//static uint8_t bluetoothRxBuffer[BLUETOOTH_RX_MAX_LEN];
	//static sysTimer_t rtcTimer;
	//static ds3231_t rtc;
	
	//Initializations
	System_Init();
	LCD_Init();
	HC12_Init();
	HC12_RxBufferInit(&nodeToMasterData,MASTER_RX_DATA_SIZE);
	DS3231_Init();
	//DS3231_SetAlarm2(0);
	//System_TimerInit(&rtcTimer,60000); //check time every 60 seconds
	EEPROM_Init();
	WiFi_Init();
	BME280_Init();
	Button_Init(&button);
	//Bluetooth_Init();
	//Bluetooth_RxBufferInit(bluetoothRxBuffer,BLUETOOTH_RX_MAX_LEN);
	HMI_Init(&button,
						masterToNodeData,
						&nodeToMasterData,
						nodeToMasterDataArray);
	//System_ClearStandbyFlag();
	
	//STEPS
	//0.)Power external modules 
	//1.)Initializations
	//2.)Clear standby flag (after system wakeup)
	//3.)Read configuration data from EEPROM memory....
	//i.e. settings(min moisture, humidity etc) and WiFi ssid and password.
	//4.)Request for all node data and wait for data to be received from... 
	//all nodes.
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
		RTC AND SLEEP
		*/
		
//		if(System_TimerDoneCounting(&rtcTimer))
//		{
//			DS3231_GetTime(&rtc);
//			if(rtc.minutes >= 20)
//			{
//				//1.)store configuration data in EEPROM
//				//2.)put system to sleep
//				//System_GoToStandbyMode();
//			}
//		}
		
		/*
		BLUETOOTH AND WIFI
		*/
		
//		if(Bluetooth_DataIsReady(bluetoothRxBuffer))
//		{
//			//1.)get number of bytes in the data received after allocated rx time elapses.
//			uint8_t bluetoothRxBytes = Bluetooth_NumberOfBytesReceived();
//			//2.)send the bluetooth rx data to the wifi module.
//			WiFi_TransmitBytes(bluetoothRxBuffer,bluetoothRxBytes);
//			//3.)store bluetooth rx data in the EEPROM
//			//4.)reinitialize bluetooth rx buffer for another data reception.
//			Bluetooth_RxBufferInit(bluetoothRxBuffer,BLUETOOTH_RX_MAX_LEN);
//		}
	}
}


