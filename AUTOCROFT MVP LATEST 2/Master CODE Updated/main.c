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
*Drivers for a keypad to replace buttons and potentiometer
*Bluetooth and WiFi functionality (no user interface)
*/

//Debug
//uint8_t inc = 0;
//uint8_t received = 0;

int main(void)
{
	//Local variables
	static ButtonDataStructure button;
	static MasterTxDataStructure masterTx;
	static MasterRxDataStructure masterRx;
	static bme280_t bme280Data;
	
	//Initializations
	System_Init();
	Potentiometer_Init();
	LCD_Init();
	HC12_TxInit();
	HC12_RxInit();
	BME280_Init();
	Button_Init(&button);
	FSM_Init(&button,&masterTx,&masterRx);
	
	while(1)
	{
		FSM_Execute();
		if (Button_Read(&button.send))
		{ 
			//inc++;
			BME280_GetData(&bme280Data);
			masterTx.humidity = bme280Data.humidity;
			masterTx.temperature = bme280Data.temperature;
			Master_EncodeTxData(&masterTx,masterTx.humidity,HUMIDITY);
			Master_EncodeTxData(&masterTx,masterTx.temperature,TEMPERATURE);
			HC12_TransmitBytes(masterTx.data, MASTER_TX_MSG_SIZE);
		}
		if (HC12_Rx_BufferFull())
		{ 
			//received++;
			masterRx.data = HC12_ReadByte();
			masterRx.moistureArr[masterTx.nodeID] = masterRx.data; //shouldn't be executed if error is detected
		}
	}
}


