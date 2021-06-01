#include <stdint.h>
#include <stdbool.h>
#include "conversions.h"
#include "sysTimer_struct.h"
#include "system.h"
#include "sensor.h"
#include "irrigation.h"
#include "cms.h"
#include "solenoid.h"
#include "ds3231.h"
#include "eeprom24c16.h"
#include "hc12.h"
#include "communication.h"

//Private defines
#define NODE_ID				1

//Private globals
static NodeRxDataStructure nodeRx;

//Private typedefs
typedef struct
{
	sensor_t moisture;
	sensor_t humidity;
	sensor_t temperature;
	irrig_t irrigation;
}autocroft_t;

//Private functions
static autocroft_t GetAllData(void);

bool commErrType1 = false; //error due to corrupted data but appropriate data length
bool commErrType2 = false; //error due to incorrect data length

int main(void)
{
	//Local variables
	static NodeTxDataStructure nodeTx;
	static sysTimer_t irrigTimer;
	//static ds3231_t rtc;
	static autocroft_t autocroftData;
	
	//Initializations
	System_Init();
	CMS_Init();
	Solenoid_Init();
	HC12_TxInit();
	HC12_RxBufferInit(nodeRx.data, HC12_RX_BUFFER_SIZE);
	//DS3231_Init();
	//EEPROM_Init();
	Node_RxInit(&nodeRx);
	Node_TxInit(&nodeTx);
	//System_ClearStandbyFlag();
	
	while(1)
	{
		//DS3231_GetTime(&rtc);
			
		if (HC12_Rx_BufferFull())
		{
			//checking for type 1 error
			commErrType1 = Node_RxDataError(nodeRx.data);
			autocroftData = GetAllData();

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
			
			nodeRx.nodeID = Node_DecodeRxMessage(&nodeRx.nodeID_Struct,
																							nodeRx.data);
			if (nodeRx.nodeID == NODE_ID)
			{
				Node_EncodeTxMessage(nodeTx.data,
													 &nodeTx.moistStruct,
													 autocroftData.moisture.value);
				HC12_Transmit(nodeTx.data);
			}
		}
		
		else
		{
			//checking for type 2 error
			if (HC12_NumberOfBytesToReceive() != (HC12_RX_BUFFER_SIZE - 1))
			{
				System_TimerDelayMs(nodeRx.dataRxTimeMs);
				if (HC12_NumberOfBytesToReceive() != (HC12_RX_BUFFER_SIZE - 1))
				{
					commErrType2 = true;
				}
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

autocroft_t GetAllData(void)
{
	static autocroft_t autocroftData;
	
	autocroftData.moisture.value = CMS_GetMoisture();
	autocroftData.moisture.minValue = Node_DecodeRxMessage(&nodeRx.minMoistStruct,
																													nodeRx.data);
	autocroftData.moisture.maxValue = Node_DecodeRxMessage(&nodeRx.maxMoistStruct,
																													nodeRx.data);
	autocroftData.moisture.level = Sensor_GetLevel(&autocroftData.moisture);
	
	autocroftData.humidity.value = Node_DecodeRxMessage(&nodeRx.humStruct,
																											 nodeRx.data);
	autocroftData.humidity.minValue = Node_DecodeRxMessage(&nodeRx.minHumStruct,
																													nodeRx.data);
	autocroftData.humidity.maxValue = Node_DecodeRxMessage(&nodeRx.maxHumStruct,
																													nodeRx.data);
	autocroftData.humidity.level = Sensor_GetLevel(&autocroftData.humidity);
	
	autocroftData.temperature.value = Node_DecodeRxMessage(&nodeRx.tempStruct,
																													nodeRx.data);
	autocroftData.temperature.minValue = Node_DecodeRxMessage(&nodeRx.minTempStruct,
																														 nodeRx.data);
	autocroftData.temperature.maxValue = Node_DecodeRxMessage(&nodeRx.maxTempStruct,
																														 nodeRx.data);
	autocroftData.temperature.level = Sensor_GetLevel(&autocroftData.temperature);
	
	autocroftData.irrigation.wateringMethod = Irrigation_GetMethod(autocroftData.moisture.level,
																																 autocroftData.humidity.level,
																																 autocroftData.temperature.level);
			
	autocroftData.irrigation.minTime = Node_DecodeRxMessage(&nodeRx.minTimeStruct,
																													 nodeRx.data);
	autocroftData.irrigation.maxTime = Node_DecodeRxMessage(&nodeRx.maxTimeStruct,
																													 nodeRx.data);
	
	return autocroftData;
}

