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

char messageArray[94] = "mMin:xxxx,mMax:xxxx,hMin:xxxx,hMax:xxxx,tMin:xxxx,tMax:xxxx,iMin:xxxx,iMax:xxxx,h:xxxx,t:xxxx";

//Globals
static button_t sendButton;
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
	//local variables
	static char hc12RxBuffer[HC12_RX_BUFFER_SIZE];
	static bme280_t bme280Data;
	
	//sysTimer_t watchdogAlarm;
	
	//Initializations
	System_Init();
	Potentiometer_Init();
	LCD_Init();
	//System_Alarm_Init(&watchdogAlarm,100);
	//System_Watchdog_Start();
	//BME280_Init();
	//HC12_Init(hc12RxBuffer);
	
	Button_Init(&sendButton, BUTTON_SEND_PIN);
	Button_Init(&backButton, BUTTON_BACK_PIN);
	Button_Init(&upButton, BUTTON_UP_PIN);
	Button_Init(&forwardButton, BUTTON_FORWARD_PIN);
	Button_Init(&downButton, BUTTON_DOWN_PIN);
	Button_Init(&enterButton, BUTTON_ENTER_PIN);
	
	Message_Element_Init(&minMoistureMsg,0,4);
	Message_Element_Init(&maxMoistureMsg,10,14);
	Message_Element_Init(&minHumidityMsg,20,24);
	Message_Element_Init(&maxHumidityMsg,30,34);
	Message_Element_Init(&minTemperatureMsg,40,44);
	Message_Element_Init(&maxTemperatureMsg,50,54);
	Message_Element_Init(&minIrrigTimeMsg,60,64);
	Message_Element_Init(&maxIrrigTimeMsg,70,74);
	Message_Element_Init(&humidityMsg,80,81);
	Message_Element_Init(&temperatureMsg,87,88);
	
	while(1)
	{
		
		ExecuteFSM();
		
//		if (Button_Read(&sendButton))
//		{
//			BME280_Get_Data(&bme280Data);
//		}
		
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
			Message_Encode(&minMoistureMsg,moistureThres.minValue,messageArray);
			break;
		
		case SUBSTATE_SET_MAX:
			moistureThres.maxValue = Potentiometer_Get_PercentPosition();
			FSM_StateTransition(&enterButton, STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MAX);
			Message_Encode(&maxMoistureMsg,moistureThres.maxValue,messageArray);
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
			Message_Encode(&minHumidityMsg,humidityThres.minValue,messageArray);
			break;
		
		case SUBSTATE_SET_MAX:
			humidityThres.maxValue = Potentiometer_Get_PercentPosition();
			FSM_StateTransition(&enterButton, STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MAX);
			Message_Encode(&maxHumidityMsg,humidityThres.maxValue,messageArray);
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
			Message_Encode(&minTemperatureMsg,temperatureThres.minValue,messageArray);
			break;
		
		case SUBSTATE_SET_MAX:
			temperatureThres.maxValue = Potentiometer_Get_PercentPosition();
			FSM_StateTransition(&enterButton, STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MAX);	
			Message_Encode(&maxTemperatureMsg,temperatureThres.maxValue,messageArray);
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
			Message_Encode(&minIrrigTimeMsg,irrigTime.minValue,messageArray);
			break;
		
		case SUBSTATE_SET_MAX:
			irrigTime.maxValue = Potentiometer_Get_RawPosition();
			FSM_StateTransition(&enterButton, STATE_IRRIGATION_TIME, SUBSTATE_HIGHLIGHT_MAX);	
			Message_Encode(&maxIrrigTimeMsg,irrigTime.maxValue,messageArray);
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

