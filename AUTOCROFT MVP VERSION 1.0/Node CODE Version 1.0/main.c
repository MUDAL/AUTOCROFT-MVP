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

/**
EEPROM MEMORY ALLOCATION
* PAGE 1: Configuration data from master controller
*/

int main(void)
{
	//Local variables
	static NodeRxDataStructure nodeRx;
	static sysTimer_t irrigTimer;
	static sysTimer_t rtcTimer;
	static ds3231_t rtc;
	uint8_t soilMoisture = 0;
	sensorLevel_t moistureLevel = LEVEL_UNDEFINED;
	sensorLevel_t humidityLevel = LEVEL_UNDEFINED;
	sensorLevel_t temperatureLevel = LEVEL_UNDEFINED;
	irrigMethod_t irrigationMethod = IRRIG_METHOD_UNDEFINED;

	//Initializations
	System_Init();
	EEPROM_Init();
	EEPROM_GetData(nodeRx.data,NODE_RX_DATA_SIZE,PAGE1);
	CMS_Init();
	Solenoid_Init();
	HC12_Init();
	HC12_RxBufferInit(nodeRx.data, NODE_RX_DATA_SIZE);
	DS3231_Init();
	DS3231_SetAlarm2(0);
	System_TimerInit(&rtcTimer,60000); //60 seconds periodic software timer.
	Node_StoreRxData(&nodeRx);
	System_ClearStandbyFlag();
		
	while(1)
	{
		/*
		-RECEPTION AND STORAGE OF CONFIGURATION DATA FROM MASTER
		-TRANSMISSION OF MOISTURE DATA TO MASTER
		*/
		if(HC12_Rx_BufferFull())
		{
			soilMoisture = CMS_GetMoisture();
			Node_StoreRxData(&nodeRx);
			Node_TransmitData(&nodeRx,soilMoisture);
			
			moistureLevel = Sensor_GetLevel(soilMoisture, nodeRx.minMoist, nodeRx.maxMoist);			
			humidityLevel = Sensor_GetLevel(nodeRx.humidity, nodeRx.minHum, nodeRx.maxHum);
			temperatureLevel = Sensor_GetLevel(nodeRx.temperature, nodeRx.minTemp, nodeRx.maxTemp);
			irrigationMethod = Irrigation_GetMethod(moistureLevel, humidityLevel, temperatureLevel);
			
			switch(irrigationMethod)
			{
				case NO_IRRIGATION:
				case IRRIG_METHOD_UNDEFINED:
					Solenoid_Control(SOLENOID_OFF);
					break;
				case LIGHT_IRRIGATION:
					//convert minimum irrigation time to milliseconds
					System_TimerInit(&irrigTimer,(nodeRx.minIrrigTime*1000));
					Solenoid_Control(SOLENOID_ON);
					break;
				case HEAVY_IRRIGATION:
					//convert maximum irrigation time to milliseconds
					System_TimerInit(&irrigTimer,(nodeRx.maxIrrigTime*1000));
					Solenoid_Control(SOLENOID_ON);
					break;
			}
		}
		//ERROR DETECTION AND CORRECTION OF DATA FROM MASTER
		Node_RxErrorHandler(&nodeRx);
		//IRRIGATION CONTROL
		if(Solenoid_IsOn())
		{
			if(System_TimerDoneCounting(&irrigTimer))
			{
				Solenoid_Control(SOLENOID_OFF);
			}
		}
		/*
		RTC AND SLEEP
		*/
		if(System_TimerDoneCounting(&rtcTimer))
		{
			DS3231_GetTime(&rtc);
			if(rtc.minutes >= 20)
			{
				//1.)store configuration data from master in EEPROM
				//2.)put system to sleep
				EEPROM_StoreData(nodeRx.data,NODE_RX_DATA_SIZE,PAGE1);
				System_GoToStandbyMode();
			}
		}
	}
}

