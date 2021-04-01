#include <stdint.h>
#include <stdbool.h>
#include "conversions.h"
#include "sysTimer_struct.h"
#include "system.h"
#include "bme280.h"
#include "eeprom24c16.h"
#include "hc12.h"
#include "lcd.h"
#include "potentiometer.h"
#include "button.h"
#include "sensor.h"
#include "irrigation.h"
#include "display.h"
#include "FSM.h"
#include "message.h"

//Globals
static ButtonDataStructure button;
static MasterMessageDataStructure masterToNode;
static NodeMessageDataStructure nodeToMaster;

//Functions for handling the different states of the Finite State Machine
static void FSM_Display_BME280_Data(uint8_t substate);
static void FSM_Node(uint8_t substate);
static void FSM_Moisture(uint8_t substate);
static void FSM_Humidity(uint8_t substate);
static void FSM_Temperature(uint8_t substate);
static void FSM_IrrigTime(uint8_t substate);

static void (*pStateFunction[NUMBER_OF_STATES])(uint8_t substate) = 
{
		FSM_Display_BME280_Data,
		FSM_Node,
		FSM_Moisture,
		FSM_Humidity,
		FSM_Temperature,
		FSM_IrrigTime
};

static void ExecuteFSM(void);
static void FSM_StateTransition(button_t* pButton, uint8_t state, uint8_t substate);

int main(void)
{
	//Local variables
	sysTimer_t bme280Alarm;
	bme280_t bme280Data;
	//sysTimer_t watchdogAlarm;
	
	//Initializations
	System_Init();
	Potentiometer_Init();
	LCD_Init();
	HC12_Tx_Init();
	HC12_Rx_Init(nodeToMaster.data);
	
	Button_Init(&button);
	
	//System_Alarm_Init(&watchdogAlarm,100);
	System_Alarm_Init(&bme280Alarm, 100);
	//System_Watchdog_Start();
	BME280_Init();
	
	Master_Message_Init(&masterToNode);
	Node_Message_Init(&nodeToMaster);
	
	while(1)
	{
		
		ExecuteFSM();
		
		if (System_Alarm_Ready(&bme280Alarm))
		{
			BME280_Get_Data(&bme280Data);
		}
		
		if (Button_Read(&button.send))
		{
			BME280_Get_Data(&bme280Data);
			Master_Message_Encode(masterToNode.dataToSend, &masterToNode.humidity , bme280Data.humidity);
			Master_Message_Encode(masterToNode.dataToSend, &masterToNode.temperature, bme280Data.temperature);
			HC12_Transmit(masterToNode.dataToSend);
		}
		
		if (HC12_Rx_Done_Receiving())
		{
			HC12_Rx_Restart();
			nodeToMaster.moistureArr[masterToNode.nodeIDvalue] = Node_Message_Decode(&nodeToMaster.moisture,nodeToMaster.data);
		}
		
//		if (System_Alarm_Ready(&watchdogAlarm))
//		{
//			System_Watchdog_Refresh();
//		}
		
	}
	
}

void FSM_Display_BME280_Data(uint8_t substate)
{
	bme280_t bme280Data;
	BME280_Get_Data(&bme280Data);
	
	Display_Data("Hum: ",
							 "Temp: ",
							 bme280Data.humidity,
							 bme280Data.temperature,
							 '%',
							 'C');
	FSM_StateTransition(&button.forward, STATE_NODE, SUBSTATE_HIGHLIGHT_NODE_ID);
}

void FSM_Node(uint8_t substate)
{
	
	Display_Node("Node ID: ",
							 "Moisture: ",
							 substate,
							 masterToNode.nodeIDvalue,
							 nodeToMaster.moistureArr[masterToNode.nodeIDvalue],
							 ' ',
							 '%');

	switch (substate)
	{
		case SUBSTATE_HIGHLIGHT_NODE_ID:
			FSM_StateTransition(&button.back, STATE_DISPLAY_READINGS, SUBSTATE_NIL);
			FSM_StateTransition(&button.forward, STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&button.enter, STATE_NODE, SUBSTATE_SET_NODE_ID);
			break;
			
		case SUBSTATE_SET_NODE_ID:
			masterToNode.nodeIDvalue = Potentiometer_Get_PercentPosition();
			FSM_StateTransition(&button.enter, STATE_NODE, SUBSTATE_HIGHLIGHT_NODE_ID);
			Master_Message_Encode(masterToNode.dataToSend, &masterToNode.nodeID, masterToNode.nodeIDvalue);
			break;
	}
}

void FSM_Moisture(uint8_t substate)
{
	static sensorThreshold_t moistureThres; 

	Display_SensorThreshold(&moistureThres,
													substate,
													"MoistMin:",
													"MoistMax:",
													'%');
	switch (substate)
	{
		case SUBSTATE_HIGHLIGHT_MIN:
			FSM_StateTransition(&button.back, STATE_NODE, SUBSTATE_HIGHLIGHT_NODE_ID);
			FSM_StateTransition(&button.down, STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MAX);
			FSM_StateTransition(&button.forward, STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&button.enter, STATE_MOISTURE, SUBSTATE_SET_MIN);
			break;
		
		case SUBSTATE_HIGHLIGHT_MAX:
			FSM_StateTransition(&button.up, STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&button.forward, STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&button.enter, STATE_MOISTURE, SUBSTATE_SET_MAX);
			break;
		
		case SUBSTATE_SET_MIN:
			moistureThres.minValue = Potentiometer_Get_PercentPosition();
			FSM_StateTransition(&button.enter, STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MIN);
			Master_Message_Encode(masterToNode.dataToSend,&masterToNode.minMoisture,moistureThres.minValue);
			break;
		
		case SUBSTATE_SET_MAX:
			moistureThres.maxValue = Potentiometer_Get_PercentPosition();
			FSM_StateTransition(&button.enter, STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MAX);
			Master_Message_Encode(masterToNode.dataToSend,&masterToNode.maxMoisture,moistureThres.maxValue);
			break;
	}
	
}

void FSM_Humidity(uint8_t substate)
{
	static sensorThreshold_t humidityThres;

	Display_SensorThreshold(&humidityThres,
													substate,
													"HumMin:",
													"HumMax:",
													'%');
	switch(substate)
	{
		case SUBSTATE_HIGHLIGHT_MIN:
			FSM_StateTransition(&button.down, STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MAX);
			FSM_StateTransition(&button.forward, STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&button.enter, STATE_HUMIDITY, SUBSTATE_SET_MIN);
			FSM_StateTransition(&button.back, STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MIN);
			break;
		
		case SUBSTATE_HIGHLIGHT_MAX:
			FSM_StateTransition(&button.up, STATE_HUMIDITY,SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&button.forward, STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&button.enter, STATE_HUMIDITY, SUBSTATE_SET_MAX);	
			FSM_StateTransition(&button.back, STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MIN);
			break;
		
		case SUBSTATE_SET_MIN:
			humidityThres.minValue = Potentiometer_Get_PercentPosition();
			FSM_StateTransition(&button.enter, STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MIN);
			Master_Message_Encode(masterToNode.dataToSend,&masterToNode.minHumidity,humidityThres.minValue);
			break;
		
		case SUBSTATE_SET_MAX:
			humidityThres.maxValue = Potentiometer_Get_PercentPosition();
			FSM_StateTransition(&button.enter, STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MAX);
			Master_Message_Encode(masterToNode.dataToSend,&masterToNode.maxHumidity,humidityThres.maxValue);
			break;
	}
}

void FSM_Temperature(uint8_t substate)
{
	static sensorThreshold_t temperatureThres;
	
	Display_SensorThreshold(&temperatureThres,
													substate,
												  "TempMin:",
												  "TempMax:",
												  'C');
	switch (substate)
	{
		case SUBSTATE_HIGHLIGHT_MIN:
			FSM_StateTransition(&button.down, STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MAX);
			FSM_StateTransition(&button.forward, STATE_IRRIGATION_TIME, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&button.enter, STATE_TEMPERATURE, SUBSTATE_SET_MIN);
			FSM_StateTransition(&button.back, STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MIN);
			break;
		
		case SUBSTATE_HIGHLIGHT_MAX:
			FSM_StateTransition(&button.up, STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&button.forward, STATE_IRRIGATION_TIME, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&button.enter, STATE_TEMPERATURE, SUBSTATE_SET_MAX);	
			FSM_StateTransition(&button.back, STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MIN);
			break;
		
		case SUBSTATE_SET_MIN:
			temperatureThres.minValue = Potentiometer_Get_PercentPosition();
			FSM_StateTransition(&button.enter, STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MIN);
			Master_Message_Encode(masterToNode.dataToSend,&masterToNode.minTemperature,temperatureThres.minValue);
			break;
		
		case SUBSTATE_SET_MAX:
			temperatureThres.maxValue = Potentiometer_Get_PercentPosition();
			FSM_StateTransition(&button.enter, STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MAX);	
			Master_Message_Encode(masterToNode.dataToSend,&masterToNode.maxTemperature,temperatureThres.maxValue);
			break;
	}
}

void FSM_IrrigTime(uint8_t substate)
{
	static irrigTime_t irrigTime;
	
	Display_IrrigationTime(&irrigTime,
												 substate,
												 "TimeMin:",
												 "TimeMax:",
												 's');	
	switch(substate)
	{
		case SUBSTATE_HIGHLIGHT_MIN:
			FSM_StateTransition(&button.down, STATE_IRRIGATION_TIME, SUBSTATE_HIGHLIGHT_MAX);
	    FSM_StateTransition(&button.enter, STATE_IRRIGATION_TIME, SUBSTATE_SET_MIN);
	    FSM_StateTransition(&button.back, STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MIN);
			break;
		
		case SUBSTATE_HIGHLIGHT_MAX:
			FSM_StateTransition(&button.up, STATE_IRRIGATION_TIME, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&button.enter, STATE_IRRIGATION_TIME, SUBSTATE_SET_MAX);
			FSM_StateTransition(&button.back, STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MIN);
			break;
		
		case SUBSTATE_SET_MIN:
			irrigTime.minValue = Potentiometer_Get_RawPosition();
			FSM_StateTransition(&button.enter, STATE_IRRIGATION_TIME, SUBSTATE_HIGHLIGHT_MIN);	
			Master_Message_Encode(masterToNode.dataToSend,&masterToNode.minIrrigTime,irrigTime.minValue);
			break;
		
		case SUBSTATE_SET_MAX:
			irrigTime.maxValue = Potentiometer_Get_RawPosition();
			FSM_StateTransition(&button.enter, STATE_IRRIGATION_TIME, SUBSTATE_HIGHLIGHT_MAX);	
			Master_Message_Encode(masterToNode.dataToSend,&masterToNode.maxIrrigTime,irrigTime.maxValue);
			break;
	}
}

void ExecuteFSM(void)
{
	uint8_t state = FSM_GetState();
	uint8_t substate = FSM_GetSubstate();
	pStateFunction[state](substate);
}

void FSM_StateTransition(button_t* pButton, uint8_t state, uint8_t substate)
{
	if (Button_Read(pButton))
	{
		//Refresh LCD screen
		LCD_Clear(); 
		FSM_SetState(state);
		FSM_SetSubstate(substate);
	}
}

