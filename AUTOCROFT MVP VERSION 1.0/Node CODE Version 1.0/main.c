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

/**
@brief Performs irrigation (if required) based on current soil moisture, environmental temperature  
and humidity data received from the master.  
@param pNodeRx: pointer to the data structure containing data received from the master  
by the node.  
@param pIrrigTimer: pointer to software timer that determines the irrigation duration.  
@return None.  
*/
static void TriggerIrrigation(NodeRxDataStructure* pNodeRx, sysTimer_t* pIrrigTimer)
{
	static bool irrigationStarted;
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
			if(!irrigationStarted)
			{
				//convert minimum irrigation time to milliseconds
				System_TimerInit(pIrrigTimer,(pNodeRx->minIrrigTime*1000));
				Solenoid_Control(SOLENOID_ON);
				irrigationStarted = true;
			}
			break;
		case HEAVY_IRRIGATION:
			if(!irrigationStarted)
			{
				//convert maximum irrigation time to milliseconds
				System_TimerInit(pIrrigTimer,(pNodeRx->maxIrrigTime*1000));
				Solenoid_Control(SOLENOID_ON);
				irrigationStarted = true;
			}
			break;
	}
	if(Solenoid_IsOn())
	{
		if(System_TimerDoneCounting(pIrrigTimer))
		{
			Solenoid_Control(SOLENOID_OFF);
			irrigationStarted = false;
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
	Node_StoreRxData(&nodeRx);
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
			Node_StoreRxData(&nodeRx);
			Node_TransmitData(&nodeRx,soilMoisture);
		}
		//ERROR DETECTION AND CORRECTION OF DATA FROM MASTER
		Node_RxErrorHandler(&nodeRx);
		//IRRIGATION CONTROL
		TriggerIrrigation(&nodeRx,&irrigTimer);
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

