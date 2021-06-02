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
#include "sensor.h"
#include "irrigation.h"
#include "communication.h"
#include "FSM.h"

//Private functions
static void StoreNodeData(MasterTxDataStructure* pMasterTx,
													MasterRxDataStructure* pMasterRx);

bool commErrType1 = false; //error due to corrupted data but appropriate data length
bool commErrType2 = false; //error due to incorrect data length

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
	HC12_RxBufferInit(masterRx.data, HC12_RX_BUFFER_SIZE);
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
			Master_EncodeTxMessage(masterTx.data, 
													 &masterTx.humStruct,
													 bme280Data.humidity);
			
			Master_EncodeTxMessage(masterTx.data,
													 &masterTx.tempStruct,
													 bme280Data.temperature);
			
			HC12_Transmit(masterTx.data);
		}
		
		if (HC12_Rx_BufferFull())
		{
			//checking for type 1 error
			commErrType1 = Master_RxDataError(masterRx.data);
			StoreNodeData(&masterTx,&masterRx); //shouldn't be called if error is detected
		}
		else
		{
			//checking for type 2 error 
			if (HC12_NumberOfBytesToReceive() != (HC12_RX_BUFFER_SIZE - 1))
			{
				System_TimerDelayMs(masterRx.dataRxTimeMs);
				if (HC12_NumberOfBytesToReceive() != (HC12_RX_BUFFER_SIZE - 1))
				{
					commErrType2 = true;
				}
			}
		}
	}
}

void StoreNodeData(MasterTxDataStructure* pMasterTx,
									 MasterRxDataStructure* pMasterRx)
{
	pMasterRx->moistureArr[pMasterTx->nodeID] = 
	Master_DecodeRxMessage(&pMasterRx->moistStruct,pMasterRx->data);
}
