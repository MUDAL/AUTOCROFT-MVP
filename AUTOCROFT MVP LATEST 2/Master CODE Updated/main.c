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
*Bluetooth and WiFi functionality
**Add:
-USART and DMA for bluetooth Rx
-USART for wifi Tx
*/

int main(void)
{
	//Local variables
	static ButtonDataStructure button;
	static uint8_t masterToNodeData[MASTER_TX_DATA_SIZE];
	//static ds3231_t rtc;
	uint8_t nodeID = 0; //valid node ID starts from 1
	uint8_t nodeData = 0; //soil moisture measured by a node
	
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
	HMI_Init(&button,masterToNodeData);
	//System_ClearStandbyFlag();
	
	//WELCOME MESSAGE
	LCD_WriteString("*** HELLO ***");
	System_TimerDelayMs(1000);
	
	//STEPS
	//1.)Initializations
	//2.)Clear standby flag (after system wakeup)
	//3.)Read configuration data from EEPROM memory
	
	while(1)
	{
		//4.)Request for all node data and store the data... 
		//[allocate about 2 minutes for this using real-time clock]
		//5.)Check for bluetooth data
		//6.)If valid bluetooth data is received i.e. the ...
		//SSID and password of WiFi network to connect to,
		//7.)Send the bluetooth data (WiFi SSID and password) to the ...
		//WiFi module.
		//8.)Execute Human Machine Interface (significant aspect of the application)
		
		//DS3231_GetTime(&rtc);
		HMI_Execute();

		if (HC12_Rx_BufferFull())
		{ 
			nodeData = HC12_ReadByte();
			nodeID = Master_GetNodeID();
			Master_StoreNodeData(nodeID,nodeData); //shouldn't be executed if error is detected
		}
	}
}


