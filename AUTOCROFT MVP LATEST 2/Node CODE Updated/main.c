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
#define ASSIGNED_ID				1

//Private functions
static void StoreAllData(NodeDataStructure* pNode);

int main(void)
{
	static NodeDataStructure node;
	static sysTimer_t irrigTimer;
	//static ds3231_t rtc;
	sensorLevel_t moistureLevel = LEV_UNDEFINED;
	sensorLevel_t humidityLevel = LEV_UNDEFINED;
	sensorLevel_t temperatureLevel = LEV_UNDEFINED;
	irrigMethod_t wateringMethod = IRRIG_METHOD_UNDEFINED;
	
	//Initializations
	System_Init();
	CMS_Init();
	Solenoid_Init();
	HC12_TxInit();
	HC12_RxBufferInit(node.RxData, HC12_RX_BUFFER_SIZE);
	//DS3231_Init();
	//EEPROM_Init();
	Node_Init(&node);
	//System_ClearStandbyFlag();
	
	while(1)
	{
		//DS3231_GetTime(&rtc);
			
		if (HC12_Rx_BufferFull())
		{
			StoreAllData(&node);
			moistureLevel = Sensor_GetLevel(node.TxData, node.minMoist, node.maxMoist);			
			humidityLevel = Sensor_GetLevel(node.humidity, node.minHum, node.maxHum);
			temperatureLevel = Sensor_GetLevel(node.temperature, node.minTemp, node.maxTemp);
			wateringMethod = Irrigation_GetMethod(moistureLevel,humidityLevel,temperatureLevel);
			
			switch (wateringMethod)
			{
				case NO_WATERING:
				case IRRIG_METHOD_UNDEFINED:
					Solenoid_Switch(SOLENOID_OFF);
					break;
				case LIGHT_WATERING:
					//convert minimum irrigation time to milliseconds
					System_TimerInit(&irrigTimer, (node.minIrrigTime * 1000));
					Solenoid_Switch(SOLENOID_ON);
					break;
				case HEAVY_WATERING:
					//convert maximum irrigation time to milliseconds
					System_TimerInit(&irrigTimer, (node.maxIrrigTime * 1000));
					Solenoid_Switch(SOLENOID_ON);
					break;
			}
			
			node.nodeID = Node_DecodeRxMessage(&node,
																				 NODE_ID);
			if (node.nodeID == ASSIGNED_ID)
			{
				HC12_TransmitByte(node.TxData);
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

void StoreAllData(NodeDataStructure* pNode)
{
	pNode->TxData = CMS_GetMoisture();
	pNode->minMoist = Node_DecodeRxMessage(pNode,MIN_MOISTURE);
	pNode->maxMoist = Node_DecodeRxMessage(pNode,MAX_MOISTURE);
	
	pNode->humidity= Node_DecodeRxMessage(pNode,HUMIDITY);
	pNode->minHum = Node_DecodeRxMessage(pNode,MIN_HUMIDITY);
	pNode->maxHum = Node_DecodeRxMessage(pNode,MAX_HUMIDITY);
	
	pNode->temperature = Node_DecodeRxMessage(pNode,TEMPERATURE);
	pNode->minTemp = Node_DecodeRxMessage(pNode,MIN_TEMPERATURE);
	pNode->maxTemp = Node_DecodeRxMessage(pNode,MAX_TEMPERATURE);
			
	pNode->minIrrigTime = Node_DecodeRxMessage(pNode,MIN_IRRIG_TIME);
	pNode->maxIrrigTime = Node_DecodeRxMessage(pNode,MAX_IRRIG_TIME);
}

