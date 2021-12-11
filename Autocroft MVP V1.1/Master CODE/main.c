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
#include "hmi.h"

/**
@brief Source code for Autocroft Master  
*/

/**
EEPROM MEMORY ALLOCATION
* PAGE 128: Master-to-node data
*/

int main(void)
{
	static uint8_t nodeToMasterData;
	static uint8_t masterToNodeData[MASTER_TX_DATA_SIZE];
	static uint8_t nodeToMasterDataArray[NO_OF_NODES];
	static sysTimer_t rtcTimer;
	static ds3231_t rtc;

	//Initializations
	System_Init();
	EEPROM_GetData(masterToNodeData,MASTER_TX_DATA_SIZE,PAGE128);
	Keypad_Init();
	LCD_Init();
	HC12_Init();
	HC12_RxBufferInit(&nodeToMasterData,MASTER_RX_DATA_SIZE);
	DS3231_24HourFormat(); 
	DS3231_SetTime(0,0); 
	//Alarm to wake the system up every time the system is at 0 minutes. e.g. 9:00, 11:00
	DS3231_SetAlarm2(0);
	//60 seconds periodic software timer
	System_TimerInit(&rtcTimer,60000); 
	BME280_Init();
	HMI_Init(masterToNodeData,&nodeToMasterData,nodeToMasterDataArray);
	System_ClearStandbyFlag();
	LCD_PrintString("AUTOCROFT");
	LCD_SetCursor(1,0);
	LCD_PrintString("FW: VER 1.1");
	System_TimerDelayMs(800);
	LCD_Clear();
	LCD_PrintString("Initializing....");
	LCD_SetCursor(1,0);
	LCD_PrintString("Please wait");
	
	Master_TransmitReceive(masterToNodeData,
											   MASTER_TX_DATA_SIZE,
												 &nodeToMasterData,
											   nodeToMasterDataArray,
											   NO_OF_NODES);
	while(1)
	{
		HMI_Execute();
		
		if(System_TimerDoneCounting(&rtcTimer))
		{
			DS3231_GetTime(&rtc);
			if(rtc.minutes >= SYSTEM_RUNTIME_MINUTES)
			{
				EEPROM_StoreData(masterToNodeData,MASTER_TX_DATA_SIZE,PAGE128); 
				System_GoToStandbyMode(); 
			}
		}
	}
}


