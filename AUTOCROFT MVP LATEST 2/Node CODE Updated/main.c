#include <stdint.h>
#include <stdbool.h>
#include "conversions.h"
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
#define ASSIGNED_ID		0

int main(void)
{
	static NodeRxDataStructure nodeRx;
	static sysTimer_t irrigTimer;
	//static ds3231_t rtc;
	uint8_t soilMoisture = 0;
	sensorLevel_t moistureLevel = LEV_UNDEFINED;
	sensorLevel_t humidityLevel = LEV_UNDEFINED;
	sensorLevel_t temperatureLevel = LEV_UNDEFINED;
	irrigMethod_t wateringMethod = IRRIG_METHOD_UNDEFINED;
	
	//Initializations
	System_Init();
	CMS_Init();
	Solenoid_Init();
	HC12_Init();
	HC12_RxBufferInit(nodeRx.data, NODE_RX_DATA_SIZE);
	//DS3231_Init();
	//EEPROM_Init();
	//System_ClearStandbyFlag();
	
	while(1)
	{
		//DS3231_GetTime(&rtc);
		if (HC12_Rx_BufferFull())
		{
			Node_StoreRxData(&nodeRx);
			soilMoisture = CMS_GetMoisture();
			if (nodeRx.nodeID == ASSIGNED_ID)
			{
				HC12_TransmitByte(soilMoisture);
			}
			
			moistureLevel = Sensor_GetLevel(soilMoisture, nodeRx.minMoist, nodeRx.maxMoist);			
			humidityLevel = Sensor_GetLevel(nodeRx.humidity, nodeRx.minHum, nodeRx.maxHum);
			temperatureLevel = Sensor_GetLevel(nodeRx.temperature, nodeRx.minTemp, nodeRx.maxTemp);
			wateringMethod = Irrigation_GetMethod(moistureLevel,humidityLevel,temperatureLevel);
			
			switch (wateringMethod)
			{
				case NO_WATERING:
				case IRRIG_METHOD_UNDEFINED:
					Solenoid_Control(SOLENOID_OFF);
					break;
				case LIGHT_WATERING:
					//convert minimum irrigation time to milliseconds
					System_TimerInit(&irrigTimer,(nodeRx.minIrrigTime*1000));
					Solenoid_Control(SOLENOID_ON);
					break;
				case HEAVY_WATERING:
					//convert maximum irrigation time to milliseconds
					System_TimerInit(&irrigTimer,(nodeRx.maxIrrigTime*1000));
					Solenoid_Control(SOLENOID_ON);
					break;
			}
		}
		if (Solenoid_IsOn())
		{
			if (System_TimerDoneCounting(&irrigTimer))
			{
				Solenoid_Control(SOLENOID_OFF);
			}
		}
	}
}

