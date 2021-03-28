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
static button_t backButton;
static button_t upButton;
static button_t forwardButton;
static button_t downButton;
static button_t enterButton;

static sensorThreshold_t moistureThres; 
static sensorThreshold_t humidityThres;
static sensorThreshold_t temperatureThres;

static irrigTime_t irrigTime;

static message_t minMoistureMsg;
static message_t maxMoistureMsg;
static message_t minHumidityMsg;
static message_t maxHumidityMsg;
static message_t minTemperatureMsg;
static message_t maxTemperatureMsg;
static message_t minIrrigTimeMsg;
static message_t maxIrrigTimeMsg;
static message_t humidityMsg;
static message_t temperatureMsg;

static char masterToNodeMessage[MASTER_TO_NODE_MSG_SIZE];

//Functions for handling the different states of the Finite State Machine
static void FSM_Moisture(uint8_t substate);
static void FSM_Humidity(uint8_t substate);
static void FSM_Temperature(uint8_t substate);
static void FSM_IrrigTime(uint8_t substate);

static void (*pStateFunction[NUMBER_OF_STATES])(uint8_t substate) = 
{
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
	button_t sendButton;
	static char nodeToMasterMessage[NODE_TO_MASTER_MSG_SIZE];
	static bme280_t bme280Data;
	
	//sysTimer_t watchdogAlarm;
	
	//Initializations
	System_Init();
	Potentiometer_Init();
	LCD_Init();
	HC12_Tx_Init();
	HC12_Rx_Init(nodeToMasterMessage);
	
	Button_Init(&sendButton, BUTTON_SEND_PIN);
	Button_Init(&backButton, BUTTON_BACK_PIN);
	Button_Init(&upButton, BUTTON_UP_PIN);
	Button_Init(&forwardButton, BUTTON_FORWARD_PIN);
	Button_Init(&downButton, BUTTON_DOWN_PIN);
	Button_Init(&enterButton, BUTTON_ENTER_PIN);
	
	//Initialize message array. The message array is the data
	//to be sent from master to node.
	Message_Msg_To_Send_Init(masterToNodeMessage);
	
	message_t* pMsgStructArr[10] = 
	{
		&minMoistureMsg,
		&maxMoistureMsg,
		&minHumidityMsg,
		&maxHumidityMsg,
		&minTemperatureMsg,
		&maxTemperatureMsg,
		&minIrrigTimeMsg,
		&maxIrrigTimeMsg,
		&humidityMsg,
		&temperatureMsg
	};
	
	const uint8_t colonIndexArr[10] = 
	{
		MIN_MOISTURE_COLON_INDEX,
		MAX_MOISTURE_COLON_INDEX,
		MIN_HUMIDITY_COLON_INDEX,
		MAX_HUMIDITY_COLON_INDEX,
		MIN_TEMPERATURE_COLON_INDEX,
		MAX_TEMPERATURE_COLON_INDEX,
		MIN_IRRIG_TIME_COLON_INDEX,
		MAX_IRRIG_TIME_COLON_INDEX,
		HUMIDITY_COLON_INDEX,
		TEMPERATURE_COLON_INDEX
	};
	
	const uint8_t commaIndexArr[10] = 
	{
		MIN_MOISTURE_COMMA_INDEX,
		MAX_MOISTURE_COMMA_INDEX,
		MIN_HUMIDITY_COMMA_INDEX,
		MAX_HUMIDITY_COMMA_INDEX,
		MIN_TEMPERATURE_COMMA_INDEX,
		MAX_TEMPERATURE_COMMA_INDEX,
		MIN_IRRIG_TIME_COMMA_INDEX,
		MAX_IRRIG_TIME_COMMA_INDEX,
		HUMIDITY_COMMA_INDEX,
		TEMPERATURE_COMMA_INDEX
	};
	
	//Initialize message structs. Message struct contains
	//information common to all parts of the entire message array.
	for(uint8_t i = 0; i < 10; i++)
	{
		Message_Struct_Init(pMsgStructArr[i],
												colonIndexArr[i],
												commaIndexArr[i]);
	}
	
	//System_Alarm_Init(&watchdogAlarm,100);
	//System_Watchdog_Start();
	BME280_Init();
	//Run BME280 sensor once to get rough readings
	BME280_Get_Data(&bme280Data);
	
	while(1)
	{
		
		ExecuteFSM();
		
		if (Button_Read(&sendButton))
		{
			BME280_Get_Data(&bme280Data);
			Message_Encode(&humidityMsg, bme280Data.humidity, masterToNodeMessage);
			Message_Encode(&temperatureMsg, bme280Data.temperature, masterToNodeMessage);
			HC12_Transmit(masterToNodeMessage);
		}
		
//		if (System_Alarm_Ready(&watchdogAlarm))
//		{
//			System_Watchdog_Refresh();
//		}
		
	}
	
}

void FSM_Moisture(uint8_t substate)
{
	
	Display_SensorThreshold(&moistureThres,
													substate,
													"MoistMin:",
													"MoistMax:",
													'%');
	switch (substate)
	{
		case SUBSTATE_HIGHLIGHT_MIN:
			FSM_StateTransition(&downButton, STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MAX);
			FSM_StateTransition(&forwardButton, STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&enterButton, STATE_MOISTURE, SUBSTATE_SET_MIN);
			break;
		
		case SUBSTATE_HIGHLIGHT_MAX:
			FSM_StateTransition(&upButton, STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&forwardButton, STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&enterButton, STATE_MOISTURE, SUBSTATE_SET_MAX);
			break;
		
		case SUBSTATE_SET_MIN:
			moistureThres.minValue = Potentiometer_Get_PercentPosition();
			FSM_StateTransition(&enterButton, STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MIN);
			Message_Encode(&minMoistureMsg,moistureThres.minValue,masterToNodeMessage);
			break;
		
		case SUBSTATE_SET_MAX:
			moistureThres.maxValue = Potentiometer_Get_PercentPosition();
			FSM_StateTransition(&enterButton, STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MAX);
			Message_Encode(&maxMoistureMsg,moistureThres.maxValue,masterToNodeMessage);
			break;
	}
	
}

void FSM_Humidity(uint8_t substate)
{
	Display_SensorThreshold(&humidityThres,
													substate,
													"HumMin:",
													"HumMax:",
													'%');
	switch(substate)
	{
		case SUBSTATE_HIGHLIGHT_MIN:
			FSM_StateTransition(&downButton, STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MAX);
			FSM_StateTransition(&forwardButton, STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&enterButton, STATE_HUMIDITY, SUBSTATE_SET_MIN);
			FSM_StateTransition(&backButton, STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MIN);
			break;
		
		case SUBSTATE_HIGHLIGHT_MAX:
			FSM_StateTransition(&upButton, STATE_HUMIDITY,SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&forwardButton, STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&enterButton, STATE_HUMIDITY, SUBSTATE_SET_MAX);	
			FSM_StateTransition(&backButton, STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MIN);
			break;
		
		case SUBSTATE_SET_MIN:
			humidityThres.minValue = Potentiometer_Get_PercentPosition();
			FSM_StateTransition(&enterButton, STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MIN);
			Message_Encode(&minHumidityMsg,humidityThres.minValue,masterToNodeMessage);
			break;
		
		case SUBSTATE_SET_MAX:
			humidityThres.maxValue = Potentiometer_Get_PercentPosition();
			FSM_StateTransition(&enterButton, STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MAX);
			Message_Encode(&maxHumidityMsg,humidityThres.maxValue,masterToNodeMessage);
			break;
	}
}

void FSM_Temperature(uint8_t substate)
{
	Display_SensorThreshold(&temperatureThres,
													substate,
												  "TempMin:",
												  "TempMax:",
												  'C');
	switch (substate)
	{
		case SUBSTATE_HIGHLIGHT_MIN:
			FSM_StateTransition(&downButton, STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MAX);
			FSM_StateTransition(&forwardButton, STATE_IRRIGATION_TIME, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&enterButton, STATE_TEMPERATURE, SUBSTATE_SET_MIN);
			FSM_StateTransition(&backButton, STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MIN);
			break;
		
		case SUBSTATE_HIGHLIGHT_MAX:
			FSM_StateTransition(&upButton, STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&forwardButton, STATE_IRRIGATION_TIME, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&enterButton, STATE_TEMPERATURE, SUBSTATE_SET_MAX);	
			FSM_StateTransition(&backButton, STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MIN);
			break;
		
		case SUBSTATE_SET_MIN:
			temperatureThres.minValue = Potentiometer_Get_PercentPosition();
			FSM_StateTransition(&enterButton, STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MIN);
			Message_Encode(&minTemperatureMsg,temperatureThres.minValue,masterToNodeMessage);
			break;
		
		case SUBSTATE_SET_MAX:
			temperatureThres.maxValue = Potentiometer_Get_PercentPosition();
			FSM_StateTransition(&enterButton, STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MAX);	
			Message_Encode(&maxTemperatureMsg,temperatureThres.maxValue,masterToNodeMessage);
			break;
	}
}

void FSM_IrrigTime(uint8_t substate)
{
	
	Display_IrrigationTime(&irrigTime,
												 substate,
												 "TimeMin:",
												 "TimeMax:",
												 's');	
	switch(substate)
	{
		case SUBSTATE_HIGHLIGHT_MIN:
			FSM_StateTransition(&downButton, STATE_IRRIGATION_TIME, SUBSTATE_HIGHLIGHT_MAX);
	    FSM_StateTransition(&enterButton, STATE_IRRIGATION_TIME, SUBSTATE_SET_MIN);
	    FSM_StateTransition(&backButton, STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MIN);
			break;
		
		case SUBSTATE_HIGHLIGHT_MAX:
			FSM_StateTransition(&upButton, STATE_IRRIGATION_TIME, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&enterButton, STATE_IRRIGATION_TIME, SUBSTATE_SET_MAX);
			FSM_StateTransition(&backButton, STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MIN);
			break;
		
		case SUBSTATE_SET_MIN:
			irrigTime.minValue = Potentiometer_Get_RawPosition();
			FSM_StateTransition(&enterButton, STATE_IRRIGATION_TIME, SUBSTATE_HIGHLIGHT_MIN);	
			Message_Encode(&minIrrigTimeMsg,irrigTime.minValue,masterToNodeMessage);
			break;
		
		case SUBSTATE_SET_MAX:
			irrigTime.maxValue = Potentiometer_Get_RawPosition();
			FSM_StateTransition(&enterButton, STATE_IRRIGATION_TIME, SUBSTATE_HIGHLIGHT_MAX);	
			Message_Encode(&maxIrrigTimeMsg,irrigTime.maxValue,masterToNodeMessage);
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

