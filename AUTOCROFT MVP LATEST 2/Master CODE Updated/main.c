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

//Private functions
static void StoreNodeData(MasterTxDataStructure* pMasterTx,
													MasterRxDataStructure* pMasterRx);

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
	HC12_RxBufferInit(&masterRx.data, HC12_RX_BUFFER_SIZE);
	BME280_Init();
	Button_Init(&button);
	Master_TxInit(&masterTx);
	Master_RxInit(&masterRx);
	FSM_Init(&button,&masterTx,&masterRx);
	
	while(1)
	{
		FSM_Execute();
		
		if (Button_Read(&button.send))
		{
			BME280_GetData(&bme280Data);
			masterTx.humidity = bme280Data.humidity;
			masterTx.temperature = bme280Data.temperature;
			
			Master_EncodeTxMessage(&masterTx, 
													   masterTx.humidity,
													   HUMIDITY);
			Master_EncodeTxMessage(&masterTx,
													   masterTx.temperature,
													   TEMPERATURE);
			HC12_TransmitBytes(masterTx.data, MASTER_TX_MSG_SIZE);
		}
		
		if (HC12_Rx_BufferFull())
		{
			StoreNodeData(&masterTx,&masterRx); //shouldn't be called if error is detected
		}
	}
}

void StoreNodeData(MasterTxDataStructure* pMasterTx,
									 MasterRxDataStructure* pMasterRx)
{
	pMasterRx->moistureArr[pMasterTx->nodeID] = 
	Master_DecodeRxMessage(pMasterRx);
}
