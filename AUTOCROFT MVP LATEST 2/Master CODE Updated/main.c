#include <stdint.h>
#include <stdbool.h>
#include "conversions.h"
#include "system.h"
#include "bme280.h"
#include "eeprom24c16.h"
#include "hc12.h"
#include "lcd.h"
#include "potentiometer.h"
#include "button.h"
#include "communication.h"
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
	uint8_t nodeID = 0; //valid node ID starts from 1
	uint8_t nodeData = 0; //soil moisture measured by a node
	
	//Initializations
	System_Init();
	Potentiometer_Init();
	LCD_Init();
	HC12_TxInit();
	HC12_RxInit();
	BME280_Init();
	Button_Init(&button);
	HMI_Init(&button,masterToNodeData);
	//WELCOME MESSAGE
	LCD_WriteString("*** HELLO ***");
	System_TimerDelayMs(1000);
	
	//STEPS
	//1.)Initializations
	//2.)Clear standby flag (after system wakeup)
	
	while(1)
	{
		//3.)Request for all node data and store the data... 
		//[allocate about 2 minutes for this using real-time clock]
		//4.)Check for bluetooth data
		//5.)If valid bluetooth data is received i.e. the ...
		//SSID and password of WiFi network to connect to,
		//6.)Send the bluetooth data (WiFi SSID and password) to the ...
		//WiFi module.
		//7.)Execute Human Machine Interface (significant aspect of the application)
		
		HMI_Execute();

		if (HC12_Rx_BufferFull())
		{ 
			nodeData = HC12_ReadByte();
			nodeID = Master_GetNodeID();
			Master_StoreNodeData(nodeID,nodeData); //shouldn't be executed if error is detected
		}
	}
}


