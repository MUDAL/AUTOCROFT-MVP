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

#define NODE_ID				0

//Globals
static MasterMessageDataStructure masterToNode;

//Private typedef
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
	static sysTimer_t irrigAlarm;
	static autocroft_t autocroftData;
	static uint8_t nodeIDFromMaster;
	
	//Initializations
	System_Init();
	CMS_Init();
	Solenoid_Init();
	HC12_Tx_Init();
	HC12_Rx_Buffer_Init(masterToNode.data, HC12_RX_BUFFER_SIZE);
	
	Master_Message_Init(&masterToNode);
	Node_Message_Init(&nodeToMaster);
	
	while(1)
	{
		
		if (HC12_Rx_Buffer_Full())
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
					System_Alarm_Init(&irrigAlarm, (autocroftData.irrigation.minTime * 1000));
					Solenoid_Switch(SOLENOID_ON);
					break;
				
				case HEAVY_WATERING:
					//convert maximum irrigation time to milliseconds
					System_Alarm_Init(&irrigAlarm, (autocroftData.irrigation.maxTime * 1000));
					Solenoid_Switch(SOLENOID_ON);
					break;
			}
			
			nodeIDFromMaster = Master_Message_Decode(&masterToNode.nodeID,masterToNode.data);
			if (nodeIDFromMaster == NODE_ID)
			{
				Node_Message_Encode(nodeToMaster.dataToSend, &nodeToMaster.moisture, autocroftData.moisture.value);
				HC12_Transmit(nodeToMaster.dataToSend);
			}
			
		}
		
		if (Solenoid_IsRunning())
		{
			if (System_Alarm_Ready(&irrigAlarm))
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
	autocroftData.moisture.minValue = Master_Message_Decode(&masterToNode.minMoisture, masterToNode.data);
	autocroftData.moisture.maxValue = Master_Message_Decode(&masterToNode.maxMoisture, masterToNode.data);
	autocroftData.moisture.level = Sensor_Get_Level(&autocroftData.moisture);
	
	autocroftData.humidity.value = Master_Message_Decode(&masterToNode.humidity, masterToNode.data);
	autocroftData.humidity.minValue = Master_Message_Decode(&masterToNode.minHumidity, masterToNode.data);
	autocroftData.humidity.maxValue = Master_Message_Decode(&masterToNode.maxHumidity, masterToNode.data);
	autocroftData.humidity.level = Sensor_Get_Level(&autocroftData.humidity);
	
	autocroftData.temperature.value = Master_Message_Decode(&masterToNode.temperature, masterToNode.data);
	autocroftData.temperature.minValue = Master_Message_Decode(&masterToNode.minTemperature, masterToNode.data);
	autocroftData.temperature.maxValue = Master_Message_Decode(&masterToNode.maxTemperature, masterToNode.data);
	autocroftData.temperature.level = Sensor_Get_Level(&autocroftData.temperature);
	
	autocroftData.irrigation.wateringMethod = Irrigation_Get_Method(autocroftData.moisture.level,
																																	autocroftData.humidity.level,
																																	autocroftData.temperature.level);
			
	autocroftData.irrigation.minTime = Master_Message_Decode(&masterToNode.minIrrigTime, masterToNode.data);
	autocroftData.irrigation.maxTime = Master_Message_Decode(&masterToNode.maxIrrigTime, masterToNode.data);
	
	return autocroftData;
}

