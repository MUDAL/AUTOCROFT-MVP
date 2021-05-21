#include <stdint.h>
#include <stdbool.h>
#include "conversions.h"
#include "sysTimer_struct.h"
#include "system.h"
#include "sensor.h"
#include "irrigation.h"
#include "cms.h"
#include "solenoid.h"
#include "hc12.h"
#include "message.h"
#include "ds3231.h"
#include "eeprom24c16.h"

//Private defines
#define NODE_ID				0

//Private globals
static MasterMessageDataStructure masterToNode;

//Private typedefs
typedef struct
{
	sensor_t moisture;
	sensor_t humidity;
	sensor_t temperature;
	irrig_t irrigation;
}autocroft_t;

//Functions
static autocroft_t Node_Get_All_Data(void);

int main(void)
{
	
	//Local variables
	static NodeMessageDataStructure nodeToMaster;
	static sysTimer_t irrigTimer;
	//static ds3231_t rtc;
	static autocroft_t autocroftData;
	uint8_t nodeIDFromMaster = 0;
	
	//Initializations
	System_Init();
	CMS_Init();
	Solenoid_Init();
	HC12_TxInit();
	HC12_Rx_BufferInit(masterToNode.data, HC12_RX_BUFFER_SIZE);
	//DS3231_Init();
	//EEPROM_Init();
	Master_MessageInit(&masterToNode);
	Node_MessageInit(&nodeToMaster);

	//System_ClearStandbyFlag();
	
	while(1)
	{
		//DS3231_GetTime(&rtc);
			
		if (HC12_Rx_BufferFull())
		{
			autocroftData = Node_Get_All_Data();

			switch (autocroftData.irrigation.wateringMethod)
			{
				case NO_WATERING:
				case IRRIG_METHOD_UNDEFINED:
					Solenoid_Switch(SOLENOID_OFF);
					break;
				
				case LIGHT_WATERING:
					//convert minimum irrigation time to milliseconds
					System_TimerInit(&irrigTimer, (autocroftData.irrigation.minTime * 1000));
					Solenoid_Switch(SOLENOID_ON);
					break;
				
				case HEAVY_WATERING:
					//convert maximum irrigation time to milliseconds
					System_TimerInit(&irrigTimer, (autocroftData.irrigation.maxTime * 1000));
					Solenoid_Switch(SOLENOID_ON);
					break;
			}
			
			nodeIDFromMaster = Master_MessageDecode(&masterToNode.nodeID_Struct,masterToNode.data);
			if (nodeIDFromMaster == NODE_ID)
			{
				Node_MessageEncode(nodeToMaster.data, &nodeToMaster.moistStruct, autocroftData.moisture.value);
				HC12_Transmit(nodeToMaster.data);
			}
			
		}
		
		if (Solenoid_IsRunning())
		{
			if (System_TimerDoneCounting(&irrigTimer))
			{
				Solenoid_Switch(SOLENOID_OFF);
			}
		}
		
	}
	
}

autocroft_t Node_Get_All_Data(void)
{
	static autocroft_t autocroftData;
	
	autocroftData.moisture.value = CMS_Get_Moisture();
	autocroftData.moisture.minValue = Master_MessageDecode(&masterToNode.minMoistStruct, masterToNode.data);
	autocroftData.moisture.maxValue = Master_MessageDecode(&masterToNode.maxMoistStruct, masterToNode.data);
	autocroftData.moisture.level = Sensor_Get_Level(&autocroftData.moisture);
	
	autocroftData.humidity.value = Master_MessageDecode(&masterToNode.humStruct, masterToNode.data);
	autocroftData.humidity.minValue = Master_MessageDecode(&masterToNode.minHumStruct, masterToNode.data);
	autocroftData.humidity.maxValue = Master_MessageDecode(&masterToNode.maxHumStruct, masterToNode.data);
	autocroftData.humidity.level = Sensor_Get_Level(&autocroftData.humidity);
	
	autocroftData.temperature.value = Master_MessageDecode(&masterToNode.tempStruct, masterToNode.data);
	autocroftData.temperature.minValue = Master_MessageDecode(&masterToNode.minTempStruct, masterToNode.data);
	autocroftData.temperature.maxValue = Master_MessageDecode(&masterToNode.maxTempStruct, masterToNode.data);
	autocroftData.temperature.level = Sensor_Get_Level(&autocroftData.temperature);
	
	autocroftData.irrigation.wateringMethod = Irrigation_Get_Method(autocroftData.moisture.level,
																																	autocroftData.humidity.level,
																																	autocroftData.temperature.level);
			
	autocroftData.irrigation.minTime = Master_MessageDecode(&masterToNode.minTimeStruct, masterToNode.data);
	autocroftData.irrigation.maxTime = Master_MessageDecode(&masterToNode.maxTimeStruct, masterToNode.data);
	
	return autocroftData;
}

