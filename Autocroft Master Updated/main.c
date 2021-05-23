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
#include "message.h"
#include "error_correction.h"
#include "FSM.h"

int main(void)
{
	//Local variables
	static ButtonDataStructure button;
	static MasterMessageDataStructure masterToNode;
	static NodeMessageDataStructure nodeToMaster;
	static bme280_t bme280Data;
	
	//Initializations
	System_Init();
	Potentiometer_Init();
	LCD_Init();
	HC12_TxInit();
	HC12_Rx_BufferInit(nodeToMaster.data, HC12_RX_BUFFER_SIZE);
	BME280_Init();
	Button_Init(&button);
	
	Master_MessageInit(&masterToNode);
	Node_MessageInit(&nodeToMaster);
	FSM_Init(&button,&masterToNode,&nodeToMaster);
	
	while(1)
	{
		FSM_Execute(&button,&masterToNode,&nodeToMaster);
		
		if (Button_Read(&button.send))
		{
			BME280_Get_Data(&bme280Data);
			Master_MessageEncode(masterToNode.data, 
													 &masterToNode.humStruct,
													 bme280Data.humidity);
			
			Master_MessageEncode(masterToNode.data,
													 &masterToNode.tempStruct,
													 bme280Data.temperature);
			
			HC12_Transmit(masterToNode.data);
		}
		
		if (HC12_Rx_BufferFull())
		{
			nodeToMaster.moistureArr[masterToNode.nodeID] = 
				Node_MessageDecode(&nodeToMaster.moistStruct,
														nodeToMaster.data);
		}			
		
	}
	
}
