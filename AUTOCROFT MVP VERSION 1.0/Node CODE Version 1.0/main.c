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
	static uint8_t soilMoisture;
	//Node parameters
	const uint8_t nodeID = 0;
	const uint8_t nodeATcmdLength = 9;
	uint8_t nodeATcommand[nodeATcmdLength] = "AT+C001\r\n";
	//Sensor variables
	sensorLevel_t moistureLevel = LEVEL_UNDEFINED;
	sensorLevel_t humidityLevel = LEVEL_UNDEFINED;
	sensorLevel_t temperatureLevel = LEVEL_UNDEFINED;
	irrigMethod_t irrigationMethod = IRRIG_METHOD_UNDEFINED;

	//Initializations
	System_Init();
	//EEPROM_Init();
	//EEPROM_GetData(nodeRx.data,NODE_RX_DATA_SIZE,PAGE1);
	CMS_Init();
	Solenoid_Init();
	HC12_Init();	 
	HC12_RxBufferInit(nodeRx.data, NODE_RX_DATA_SIZE);
	//Channel setting
	HC12_SetPinControl(false);
	System_TimerDelayMs(40);
	HC12_TransmitBytes(nodeATcommand, nodeATcmdLength);
	HC12_SetPinControl(true);
	System_TimerDelayMs(80);
	
	DS3231_Init();
	DS3231_24HourFormat(); 		
	DS3231_SetMinutes(0);	//Set minutes to 0 by default							 
	DS3231_SetAlarm2(0);//Alarm to wake the system up every time the system is at 0 minutes. e.g. 9:00, 11:00
	System_TimerInit(&rtcTimer,60000); //60 seconds periodic software timer.
	//Node_StoreRxData(&nodeRx);
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
			DS3231_SetMinutes(nodeRx.rtcMinute);
			if(nodeRx.nodeID == nodeID)
			{
				soilMoisture = CMS_GetMoisture();
				HC12_TransmitByte(soilMoisture);
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
			nodeRx.data[0] = 0;
			HC12_RxBufferInit(nodeRx.data, NODE_RX_DATA_SIZE);
		}			
		
		//IRRIGATION CONTROL
//		if(Solenoid_IsOn())
//		{
//			if(System_TimerDoneCounting(&irrigTimer))
//			{
//				Solenoid_Control(SOLENOID_OFF);
//			}
//		}
		/*
		RTC AND SLEEP
		*/
//		if(System_TimerDoneCounting(&rtcTimer))
//		{
//			DS3231_GetTime(&rtc);
//			if(rtc.minutes >= 25)
//			{
//				//1.)store configuration data from master in EEPROM
//				//2.)put system to sleep
//				//EEPROM_StoreData(nodeRx.data,NODE_RX_DATA_SIZE,PAGE1);
//				System_GoToStandbyMode();
//			}
//		}
	}
}

