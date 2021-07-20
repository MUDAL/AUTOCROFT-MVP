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

//Private defines
#define ASSIGNED_ID		0

/**
@brief Copies data from source buffer to target buffer. The buffer is an array  
of 8-bit integers.  
@param target: target buffer (recipient of data)  
@param source: source buffer (source of data)  
@param len: number of bytes in the buffers. The buffers must be of same length.  
@return None
*/
static void CopyData(uint8_t* target, uint8_t* source, uint8_t len)
{
	for(uint8_t i = 0; i < len; i++)
	{
		target[i] = source[i];
	}
}

static void ExecuteIrrigationMethod(NodeRxDataStructure* pNodeRx, sysTimer_t* pIrrigTimer)
{
	sensorLevel_t moistureLevel = LEVEL_UNDEFINED;
	sensorLevel_t humidityLevel = LEVEL_UNDEFINED;
	sensorLevel_t temperatureLevel = LEVEL_UNDEFINED;
	irrigMethod_t irrigationMethod = IRRIG_METHOD_UNDEFINED;
	uint8_t soilMoisture = CMS_GetMoisture();
	
	moistureLevel = Sensor_GetLevel(soilMoisture, pNodeRx->minMoist, pNodeRx->maxMoist);			
	humidityLevel = Sensor_GetLevel(pNodeRx->humidity, pNodeRx->minHum, pNodeRx->maxHum);
	temperatureLevel = Sensor_GetLevel(pNodeRx->temperature, pNodeRx->minTemp, pNodeRx->maxTemp);
	irrigationMethod = Irrigation_GetMethod(moistureLevel,humidityLevel,temperatureLevel);
	
	switch(irrigationMethod)
	{
		case NO_IRRIGATION:
		case IRRIG_METHOD_UNDEFINED:
			Solenoid_Control(SOLENOID_OFF);
			break;
		case LIGHT_IRRIGATION:
			//convert minimum irrigation time to milliseconds
			System_TimerInit(pIrrigTimer,(pNodeRx->minIrrigTime*1000));
			Solenoid_Control(SOLENOID_ON);
			break;
		case HEAVY_IRRIGATION:
			//convert maximum irrigation time to milliseconds
			System_TimerInit(pIrrigTimer,(pNodeRx->maxIrrigTime*1000));
			Solenoid_Control(SOLENOID_ON);
			break;
	}
	if(Solenoid_IsOn())
	{
		if(System_TimerDoneCounting(pIrrigTimer))
		{
			Solenoid_Control(SOLENOID_OFF);
		}
	}
}

int main(void)
{
	static NodeRxDataStructure nodeRx;
	static uint8_t prevMasterToNodeData[NODE_RX_DATA_SIZE];
	static sysTimer_t irrigTimer;
	//static sysTimer_t rtcTimer;
	//static ds3231_t rtc;
	uint8_t soilMoisture = 0;

	//Initializations
	System_Init();
	CMS_Init();
	Solenoid_Init();
	HC12_Init();
	HC12_RxBufferInit(nodeRx.data, NODE_RX_DATA_SIZE);
	DS3231_Init();
	//System_TimerInit(&rtcTimer,60000); //check time every 60 seconds
	EEPROM_Init();
	EEPROM_GetData(prevMasterToNodeData,NODE_RX_DATA_SIZE,PAGE1);
	CopyData(nodeRx.data,prevMasterToNodeData,NODE_RX_DATA_SIZE);
	Node_StoreData(&nodeRx);
	//System_ClearStandbyFlag();
		
	while(1)
	{
		/*
		-RECEPTION AND STORAGE OF CONFIGURATION DATA FROM MASTER
		-TRANSMISSION OF MOISTURE DATA TO MASTER
		*/
		if(HC12_Rx_BufferFull())
		{
			soilMoisture = CMS_GetMoisture();
			Node_StoreData(&nodeRx);
			if(nodeRx.nodeID == ASSIGNED_ID)
			{
				HC12_TransmitByte(soilMoisture);
			}
		}
		//IRRIGATION CONTROL
		ExecuteIrrigationMethod(&nodeRx,&irrigTimer);
		//ERROR DETECTION AND CORRECTION OF DATA FROM MASTER
		Node_RxErrorHandler(&nodeRx);
		/*
		RTC AND SLEEP
		*/
//		if(System_TimerDoneCounting(&rtcTimer))
//		{
//			DS3231_GetTime(&rtc);
//			if(rtc.minutes >= 20)
//			{
//				//1.)store configuration data from master in EEPROM
//				//2.)put system to sleep
					//EEPROM_StoreData(nodeRx.data,NODE_RX_DATA_SIZE,PAGE1);
//				//System_GoToStandbyMode();
//			}
//		}
	}
}

