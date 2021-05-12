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
#include "FSM.h"

int main(void)
{
	 //Local variables
	static ButtonDataStructure button;
	static MasterMessageDataStructure masterToNode;
	static NodeMessageDataStructure nodeToMaster;
	static bme280_t bme280Data;
	//sysTimer_t watchdogAlarm;
	
	//Initializations
	System_Init();
	Potentiometer_Init();
	LCD_Init();
	HC12_Tx_Init();
	HC12_Rx_Buffer_Init(nodeToMaster.data, HC12_RX_BUFFER_SIZE);
	
	//System_Alarm_Init(&watchdogAlarm,100);
	//System_Watchdog_Start();
	BME280_Init();
	
	Button_Init(&button);
	Master_Message_Init(&masterToNode);
	Node_Message_Init(&nodeToMaster);
	FSM_Init(&button,&masterToNode,&nodeToMaster);
	
	while(1)
	{
		FSM_Execute(&button,&masterToNode,&nodeToMaster);
		
		if (Button_Read(&button.send))
		{
			BME280_Get_Data(&bme280Data);
			Master_Message_Encode(masterToNode.dataToSend, 
													  &masterToNode.humidity,
														bme280Data.humidity);
			
			Master_Message_Encode(masterToNode.dataToSend,
														&masterToNode.temperature,
														bme280Data.temperature);
			
			HC12_Transmit(masterToNode.dataToSend);
		}
		
		if (HC12_Rx_Buffer_Full())
		{
			nodeToMaster.moistureArr[masterToNode.nodeIDvalue] = 
				Node_Message_Decode(&nodeToMaster.moisture,
														nodeToMaster.data);
		}			
		
	}
	
}
