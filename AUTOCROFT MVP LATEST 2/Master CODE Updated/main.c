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
	static uint8_t masterToNodeData[MASTER_TX_DATA_SIZE];
	static uint8_t nodeToMasterData[NO_OF_NODES];
	static uint8_t bluetoothRxBuffer[BLUETOOTH_RX_MAX_LEN];
	//static ds3231_t rtc;
	
	//Initializations
	System_Init();
	Potentiometer_Init();
	LCD_Init();
	HC12_Init();
	//DS3231_Init();
	//EEPROM_Init();
	WiFi_Init();
	BME280_Init();
	Button_Init(&button);
	Bluetooth_Init();
	Bluetooth_RxBufferInit(bluetoothRxBuffer,BLUETOOTH_RX_MAX_LEN);
	HMI_Init(&button,masterToNodeData,nodeToMasterData);
	System_ClearStandbyFlag();
	
	//STEPS
	//0.)Power external modules 
	//1.)Initializations
	//2.)Clear standby flag (after system wakeup)
	//3.)Read configuration data from EEPROM memory....
	//i.e. settings(min moisture, humidity etc) and WiFi ssid and password.
	//4.)Request for all node data and wait for data to be received from... 
	//all nodes.
	LCD_WriteString("Initializing....");
	LCD_SetCursor(1,0);
	LCD_WriteString("Please wait");
	Master_TransmitReceive(masterToNodeData,
											   MASTER_TX_DATA_SIZE,
											   nodeToMasterData,
											   NO_OF_NODES,
											   100);
	while(1)
	{
		//5.)Check for bluetooth data
		//6.)If valid bluetooth data is received i.e. the ...
		//SSID and password of WiFi network to connect to.
		//7.)Send the bluetooth data (WiFi SSID and password) to the ...
		//WiFi module.
		//8.)Execute Human Machine Interface (significant aspect of the application)
		
		//DS3231_GetTime(&rtc);
		
		if(Bluetooth_DataIsReady(bluetoothRxBuffer))
		{
			//1.)get number of bytes in the data received after allocated rx time elapses.
			uint8_t noOfRxBluetoothBytes = Bluetooth_NumberOfBytesReceived();
			//2.)send the bluetooth rx data to the wifi module.
			WiFi_TransmitBytes(bluetoothRxBuffer,noOfRxBluetoothBytes);
			//3.)store bluetooth rx data in the EEPROM
			//4.)reinitialize bluetooth rx buffer for another data reception.
			Bluetooth_ClearRxBuffer(bluetoothRxBuffer);
		}
		
		HMI_Execute();

	}
}


