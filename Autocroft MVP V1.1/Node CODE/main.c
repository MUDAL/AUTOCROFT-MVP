#include <stdint.h>
#include <stdbool.h>
#include "conversions.h"
#include "system.h"
#include "sensor.h"
#include "irrigation.h"
#include "cms.h"
#include "solenoid.h"
#include "ds3231.h"
#include "hc12.h"
#include "communication.h"

/**
@brief Source code for Autocroft Node 
* ID = 0
* Channel number = 1
*/

/**
EEPROM MEMORY ALLOCATION
* PAGE 1: Configuration data from master controller
*/

static void ClearBuffer(uint8_t* pBuffer, uint8_t len)
{
	for(uint8_t i = 0; i < len; i++)
	{
		pBuffer[i] = 0;
	}
}

int main(void)
{
	//Local variables
	static NodeRxDataStructure nodeRx;
	static sysTimer_t irrigTimer;
	static sysTimer_t rtcTimer;
	static ds3231_t rtc;
	static uint8_t soilMoisture;
	//Node parameters
	const uint8_t nodeATcmdLength = 9;
	const uint8_t nodeID = 0;
	uint8_t nodeATcommand[nodeATcmdLength] = "AT+C001\r\n";
	//Sensor variables
	sensorLevel_t moistureLevel = LEVEL_UNDEFINED;
	sensorLevel_t humidityLevel = LEVEL_UNDEFINED;
	sensorLevel_t temperatureLevel = LEVEL_UNDEFINED;
	irrigMethod_t irrigationMethod = IRRIG_METHOD_UNDEFINED;

	//Initializations
	System_Init();
	CMS_Init();
	Solenoid_Init();
	HC12_Init();	 
	HC12_RxBufferInit(nodeRx.data, NODE_RX_DATA_SIZE);
	//Channel setting (AT command)
	HC12_SetPinControl(false);
	System_TimerDelayMs(40);
	HC12_TransmitBytes(nodeATcommand, nodeATcmdLength);
	HC12_SetPinControl(true);
	System_TimerDelayMs(80);
	//Node Rx buffer would have been corrupted with AT commands
	ClearBuffer(nodeRx.data,NODE_RX_DATA_SIZE); 
	//Reinitialize node's Rx buffer
	HC12_RxBufferInit(nodeRx.data, NODE_RX_DATA_SIZE); 
	DS3231_24HourFormat(); 	
	//Set minutes to 0 by default	
	DS3231_SetTime(0,0);	
	//Alarm to wake the system up every time the system is at 0 minutes. e.g. 9:00, 11:00
	DS3231_SetAlarm2(0);
	//60 seconds periodic software timer.
	System_TimerInit(&rtcTimer,60000); 
	System_ClearStandbyFlag();
	
	while(1)
	{
		/*
		-RECEPTION AND STORAGE OF CONFIGURATION DATA FROM MASTER
		-TRANSMISSION OF MOISTURE DATA TO MASTER
		*/
		if(HC12_RxBufferFull())
		{	
			Node_StoreRxData(&nodeRx);
			DS3231_SetTime(0,nodeRx.rtcMinute);
			if(nodeRx.nodeID == nodeID)
			{
				soilMoisture = CMS_GetMoisture();
				HC12_TransmitByte(soilMoisture);
			}	
			
			if(nodeRx.sysReset)
			{
				System_Reset();
			}

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
					//convert minimum irrigation time(minutes) to milliseconds
					System_TimerInit(&irrigTimer,(nodeRx.minIrrigTime*60*1000));
					Solenoid_Control(SOLENOID_ON);
					break;
				case HEAVY_IRRIGATION:
					//convert maximum irrigation time(minutes) to milliseconds
					System_TimerInit(&irrigTimer,(nodeRx.maxIrrigTime*60*1000));
					Solenoid_Control(SOLENOID_ON);
					break;
			}
		}
				
		if(HC12_IncompleteRxData() || (nodeRx.data[0] == IDLE_CHARACTER))
		{
			HC12_RxBufferInit(nodeRx.data, NODE_RX_DATA_SIZE);
		}			
		
		if(Solenoid_IsOn())
		{
			if(System_TimerDoneCounting(&irrigTimer))
			{
				Solenoid_Control(SOLENOID_OFF);
			}
		}

		if(System_TimerDoneCounting(&rtcTimer))
		{
			DS3231_GetTime(&rtc);
			if(rtc.minutes >= SYSTEM_RUNTIME_MINUTES)
			{
				System_GoToStandbyMode();
			}
		}
	}
}

