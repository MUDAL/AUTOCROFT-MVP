#include <stdint.h>
#include <stdbool.h>
#include "conversions.h"
#include "sysTimer_struct.h"
#include "system.h"
#include "bme280.h"
#include "eeprom24c16.h"
#include "hc12.h"
#include "lcd.h"
#include "potentiometer.h"
#include "button.h"
#include "communication.h"
#include "FSM.h"

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
	static bme280_t bme280Data;
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
	FSM_Init(&button,masterToNodeData);
	//WELCOME MESSAGE
	LCD_WriteString("*** WELCOME ***");
	System_TimerDelayMs(1000);
	
	while(1)
	{
		FSM_Execute();
		if (Button_Read(&button.send))
		{ 
			BME280_GetData(&bme280Data);
			Master_EncodeTxData(masterToNodeData,bme280Data.humidity,HUMIDITY);
			Master_EncodeTxData(masterToNodeData,bme280Data.temperature,TEMPERATURE);
			HC12_TransmitBytes(masterToNodeData, MASTER_TX_DATA_SIZE);
		}
		if (HC12_Rx_BufferFull())
		{ 
			nodeData = HC12_ReadByte();
			nodeID = Master_GetNodeID();
			Master_StoreNodeData(nodeID,nodeData); //shouldn't be executed if error is detected
		}
	}
}


