#include <stdint.h>
#include <stdbool.h>
#include "conversions.h" 
#include "button.h"
#include "potentiometer.h"
#include "lcd.h"
#include "bme280.h"
#include "sensor.h"
#include "irrigation.h"
#include "hc12.h"
#include "message.h"
#include "FSM.h"

//Private defines
#define NUMBER_OF_STATES 					6

//Private enums
enum States
{
	STATE_DISPLAY_READINGS = 0,
	STATE_NODE,
	STATE_MOISTURE,
	STATE_HUMIDITY,
	STATE_TEMPERATURE,
	STATE_IRRIGATION_TIME
};

enum Substates
{
	SUBSTATE_NIL = 0,
	SUBSTATE_HIGHLIGHT_NODE_ID,
	SUBSTATE_SET_NODE_ID,
	SUBSTATE_HIGHLIGHT_MIN,
	SUBSTATE_HIGHLIGHT_MAX,
	SUBSTATE_SET_MIN,
	SUBSTATE_SET_MAX
};

//Private globals
static ButtonDataStructure* ptrButton;
static MasterMessageDataStructure* ptrMasterToNode;
static NodeMessageDataStructure* ptrNodeToMaster;
static uint8_t currentState = STATE_DISPLAY_READINGS;
static uint8_t currentSubstate = SUBSTATE_NIL;

//Function prototypes
static void refreshDisplay(uint8_t row, uint8_t column);
static void displayBme280Data(char* firstRowHeading,
														  char* secondRowHeading,
														  uint8_t row1Data,
														  uint8_t row2Data,
														  char measurementUnit1,
														  char measurementUnit2);
									
static void displayNodeData(char* firstRowHeading,
													  char* secondRowHeading,
													  uint8_t displayMode,
													  uint8_t row1Data,
													  uint8_t row2Data);

static void displaySensorThreshold(sensorThreshold_t* pSensorThres,
																	 uint8_t displayMode,
																	 char* firstRowHeading,
																	 char* secondRowHeading,
																	 char measurementUnit);

static void displayIrrigTime(irrigTime_t* pIrrigTime, 
														 uint8_t displayMode,
														 char* firstRowHeading,
														 char* secondRowHeading,
														 char measurementUnit);																		
	
static void FSM_StateTransition(button_t* pButton,
																uint8_t state,
																uint8_t substate);

static void FSM_DisplayBme280Data(uint8_t substate);
static void FSM_Node(uint8_t substate);
static void FSM_Moisture(uint8_t substate);
static void FSM_Humidity(uint8_t substate);
static void FSM_Temperature(uint8_t substate);
static void FSM_IrrigTime(uint8_t substate);

//Function definitions
void refreshDisplay(uint8_t row, uint8_t column)
{
	LCD_Set_Cursor(row,column);
	//Delete line
	for (uint8_t i = 0; i < 16; i++)
	{
		LCD_Write_Byte(' ');
	}
}

void displayBme280Data(char* firstRowHeading,
											 char* secondRowHeading,
											 uint8_t row1Data,
											 uint8_t row2Data,
											 char measurementUnit1,
											 char measurementUnit2)
										
{
	/*
	strData1 and strData2 character arrays
	are the string equivalents of 'row1Data'
	and 'row2Data'. Both arrays have their 
	first element initialized to '0' so that
	their integer equivalent is 0.
	*/
	char strData1[4] = "0";
	char strData2[4] = "0";
	static uint8_t prevRow1Data;
	static uint8_t prevRow2Data;
	
	//top
	if (prevRow1Data != row1Data)
	{
		refreshDisplay(0,0);
		prevRow1Data = row1Data;
	}
	
	LCD_Set_Cursor(0,0);
	LCD_Write_String(firstRowHeading);
	Conv_IntegerToString(row1Data,strData1);
	LCD_Write_String(strData1);
	LCD_Write_Byte(measurementUnit1);
	
	//bottom
	if (prevRow2Data != row2Data)
	{
		refreshDisplay(1,0);
		prevRow2Data = row2Data;
	}
	
	LCD_Set_Cursor(1,0);
	LCD_Write_String(secondRowHeading);
	Conv_IntegerToString(row2Data,strData2);
	LCD_Write_String(strData2);
	LCD_Write_Byte(measurementUnit2);
	
}


void displayNodeData(char* firstRowHeading,
										 char* secondRowHeading,
									   uint8_t displayMode,
								     uint8_t row1Data,
								     uint8_t row2Data)
										
{
	char strData1[4] = "0";
	char strData2[4] = "0";
	static uint8_t prevRow1Data;
	static uint8_t prevRow2Data;
	
	if (prevRow2Data != row2Data)
	{
		refreshDisplay(1,0);
		prevRow2Data = row2Data;
	}
	
	switch (displayMode)
	{
		case SUBSTATE_HIGHLIGHT_NODE_ID:
			//top
			LCD_Set_Cursor(0,0);
			LCD_Write_Byte('>');
			LCD_Set_Cursor(0,3);
			LCD_Write_String(firstRowHeading);
			Conv_IntegerToString(row1Data,strData1);
			LCD_Write_String(strData1);
			//bottom
			LCD_Set_Cursor(1,3);
			LCD_Write_String(secondRowHeading);
			Conv_IntegerToString(row2Data,strData2);
			LCD_Write_String(strData2);
			LCD_Write_Byte('%');
			break;
		
		case SUBSTATE_SET_NODE_ID:
			if (prevRow1Data != row1Data)
			{
				refreshDisplay(0,0);
				prevRow1Data = row1Data;
			}
			//top
			LCD_Set_Cursor(0,0);
			LCD_Write_String(">>");
			LCD_Set_Cursor(0,3);
			LCD_Write_String(firstRowHeading);
			Conv_IntegerToString(row1Data,strData1);
			LCD_Write_String(strData1);
			//bottom
			LCD_Set_Cursor(1,3);
			LCD_Write_String(secondRowHeading);
			Conv_IntegerToString(row2Data,strData2);
			LCD_Write_String(strData2);
			LCD_Write_Byte('%');
			break;
	}
}

void displaySensorThreshold(sensorThreshold_t* pSensorThres,
														uint8_t displayMode,
														char* firstRowHeading,
														char* secondRowHeading,
														char measurementUnit)
{
	/*
	strMin and strMax character arrays
	are the string equivalents of 'firstRowHeading'
	and 'secondRowHeading'. Both arrays have their 
	first element initialized to '0' so that
	their integer equivalent is 0.
	*/
	char strMin[4] = "0";
	char strMax[4] = "0";
	static uint8_t prevMinValue;
	static uint8_t prevMaxValue;
			
	switch (displayMode)
	{
		case SUBSTATE_HIGHLIGHT_MIN:
			//top
			LCD_Set_Cursor(0,0);
			LCD_Write_Byte('>');
			LCD_Set_Cursor(0,3);
			LCD_Write_String(firstRowHeading);
			Conv_IntegerToString(pSensorThres->minValue,strMin);
			LCD_Write_String(strMin);
			LCD_Write_Byte(measurementUnit);
			//bottom
			LCD_Set_Cursor(1,3);
			LCD_Write_String(secondRowHeading);
			Conv_IntegerToString(pSensorThres->maxValue,strMax);
			LCD_Write_String(strMax);
			LCD_Write_Byte(measurementUnit);
			break;
		
		case SUBSTATE_HIGHLIGHT_MAX:
			//top
			LCD_Set_Cursor(0,3);
			LCD_Write_String(firstRowHeading);
			Conv_IntegerToString(pSensorThres->minValue,strMin);
			LCD_Write_String(strMin);
			LCD_Write_Byte(measurementUnit);
			//bottom
			LCD_Set_Cursor(1,0);
			LCD_Write_Byte('>');
			LCD_Set_Cursor(1,3);
			LCD_Write_String(secondRowHeading);
			Conv_IntegerToString(pSensorThres->maxValue,strMax);
			LCD_Write_String(strMax);
			LCD_Write_Byte(measurementUnit);
			break;
		
		case SUBSTATE_SET_MIN:
			//top
			if (prevMinValue != pSensorThres->minValue)
			{
				refreshDisplay(0,0);
				prevMinValue = pSensorThres->minValue;
			}
			LCD_Set_Cursor(0,0);
			LCD_Write_String(">>");
			LCD_Set_Cursor(0,3);
			LCD_Write_String(firstRowHeading);
			Conv_IntegerToString(pSensorThres->minValue,strMin);
			LCD_Write_String(strMin);
			LCD_Write_Byte(measurementUnit);
			//bottom
			LCD_Set_Cursor(1,3);
			LCD_Write_String(secondRowHeading);
			Conv_IntegerToString(pSensorThres->maxValue,strMax);
			LCD_Write_String(strMax);
			LCD_Write_Byte(measurementUnit);
			break;
		
		case SUBSTATE_SET_MAX:
			//top
			LCD_Set_Cursor(0,3);
			LCD_Write_String(firstRowHeading);
		  Conv_IntegerToString(pSensorThres->minValue,strMin);
			LCD_Write_String(strMin);
			LCD_Write_Byte(measurementUnit);
			//bottom
			if (prevMaxValue != pSensorThres->maxValue)
			{
				refreshDisplay(1,0);
				prevMaxValue = pSensorThres->maxValue;
			}
			LCD_Set_Cursor(1,0);
			LCD_Write_String(">>");
			LCD_Set_Cursor(1,3);
			LCD_Write_String(secondRowHeading);
			Conv_IntegerToString(pSensorThres->maxValue,strMax);
			LCD_Write_String(strMax);
			LCD_Write_Byte(measurementUnit);
			break;
	}
	
}

void displayIrrigTime(irrigTime_t* pIrrigTime, 
											uint8_t displayMode,
											char* firstRowHeading,
											char* secondRowHeading,
											char measurementUnit)
{
	char strMin[5] = "0";
	char strMax[5] = "0";
	static uint16_t prevMinValue;
	static uint16_t prevMaxValue;
			
	switch (displayMode)
	{
		case SUBSTATE_HIGHLIGHT_MIN:
			//top
			LCD_Set_Cursor(0,0);
			LCD_Write_Byte('>');
			LCD_Set_Cursor(0,3);
			LCD_Write_String(firstRowHeading);
			Conv_IntegerToString(pIrrigTime->minValue,strMin);
			LCD_Write_String(strMin);
			LCD_Write_Byte(measurementUnit);
			//bottom
			LCD_Set_Cursor(1,3);
			LCD_Write_String(secondRowHeading);
			Conv_IntegerToString(pIrrigTime->maxValue,strMax);
			LCD_Write_String(strMax);
			LCD_Write_Byte(measurementUnit);
			break;
		
		case SUBSTATE_HIGHLIGHT_MAX:
			//top
			LCD_Set_Cursor(0,3);
			LCD_Write_String(firstRowHeading);
			Conv_IntegerToString(pIrrigTime->minValue,strMin);
			LCD_Write_String(strMin);
			LCD_Write_Byte(measurementUnit);
			//bottom
			LCD_Set_Cursor(1,0);
			LCD_Write_Byte('>');
			LCD_Set_Cursor(1,3);
			LCD_Write_String(secondRowHeading);
			Conv_IntegerToString(pIrrigTime->maxValue,strMax);
			LCD_Write_String(strMax);
			LCD_Write_Byte(measurementUnit);
			break;
		
		case SUBSTATE_SET_MIN:
			//top
			if (prevMinValue != pIrrigTime->minValue)
			{
				refreshDisplay(0,0);
				prevMinValue = pIrrigTime->minValue;
			}
			LCD_Set_Cursor(0,0);
			LCD_Write_String(">>");
			LCD_Set_Cursor(0,3);
			LCD_Write_String(firstRowHeading);
			Conv_IntegerToString(pIrrigTime->minValue,strMin);
			LCD_Write_String(strMin);
			LCD_Write_Byte(measurementUnit);
			//bottom
			LCD_Set_Cursor(1,3);
			LCD_Write_String(secondRowHeading);
			Conv_IntegerToString(pIrrigTime->maxValue,strMax);
			LCD_Write_String(strMax);
			LCD_Write_Byte(measurementUnit);
			break;
		
		case SUBSTATE_SET_MAX:
			//top
			LCD_Set_Cursor(0,3);
			LCD_Write_String(firstRowHeading);
		  Conv_IntegerToString(pIrrigTime->minValue,strMin);
			LCD_Write_String(strMin);
			LCD_Write_Byte(measurementUnit);
			//bottom
			if (prevMaxValue != pIrrigTime->maxValue)
			{
				refreshDisplay(1,0);
				prevMaxValue = pIrrigTime->maxValue;
			}
			LCD_Set_Cursor(1,0);
			LCD_Write_String(">>");
			LCD_Set_Cursor(1,3);
			LCD_Write_String(secondRowHeading);
			Conv_IntegerToString(pIrrigTime->maxValue,strMax);
			LCD_Write_String(strMax);
			LCD_Write_Byte(measurementUnit);
			break;
	}
	
}

void FSM_StateTransition(button_t* pButton, uint8_t state, uint8_t substate)
{
	if (Button_Read(pButton))
	{
		//Refresh LCD screen
		LCD_Clear(); 
		currentState = state;
		currentSubstate = substate;
	}
}

void FSM_DisplayBme280Data(uint8_t substate)
{
	bme280_t bme280Data;
	BME280_Get_Data(&bme280Data);
	
	displayBme280Data("Hum: ",
										"Temp: ",
										bme280Data.humidity,
										bme280Data.temperature,
										'%',
										'C');
	FSM_StateTransition(&ptrButton->forward,
									    STATE_NODE,
										  SUBSTATE_HIGHLIGHT_NODE_ID);	
}

void FSM_Node(uint8_t substate)
{
	displayNodeData("Node ID: ",
									"Moisture: ",
									substate,
									ptrMasterToNode->nodeID,
									ptrNodeToMaster->moistureArr[ptrMasterToNode->nodeID]);

	switch (substate)
	{
		case SUBSTATE_HIGHLIGHT_NODE_ID:
			FSM_StateTransition(&ptrButton->back, STATE_DISPLAY_READINGS, SUBSTATE_NIL);
			FSM_StateTransition(&ptrButton->forward, STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&ptrButton->enter, STATE_NODE, SUBSTATE_SET_NODE_ID);
			break;
			
		case SUBSTATE_SET_NODE_ID:
			ptrMasterToNode->nodeID = Potentiometer_GetPercentPosition();
		
			FSM_StateTransition(&ptrButton->enter, 
													STATE_NODE,
													SUBSTATE_HIGHLIGHT_NODE_ID);
		
			Master_MessageEncode(ptrMasterToNode->data,
													 &ptrMasterToNode->nodeID_Struct,
													 ptrMasterToNode->nodeID);
			break;
	}	
}

void FSM_Moisture(uint8_t substate)
{
	static sensorThreshold_t moistureThres; 

	displaySensorThreshold(&moistureThres,
												 substate,
												 "MoistMin:",
												 "MoistMax:",
												 '%');
	switch (substate)
	{
		case SUBSTATE_HIGHLIGHT_MIN:
			FSM_StateTransition(&ptrButton->back, STATE_NODE, SUBSTATE_HIGHLIGHT_NODE_ID);
			FSM_StateTransition(&ptrButton->down, STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MAX);
			FSM_StateTransition(&ptrButton->forward, STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&ptrButton->enter, STATE_MOISTURE, SUBSTATE_SET_MIN);
			break;
		
		case SUBSTATE_HIGHLIGHT_MAX:
			FSM_StateTransition(&ptrButton->back, STATE_NODE, SUBSTATE_HIGHLIGHT_NODE_ID);
			FSM_StateTransition(&ptrButton->up, STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&ptrButton->forward, STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&ptrButton->enter, STATE_MOISTURE, SUBSTATE_SET_MAX);
			break;
		
		case SUBSTATE_SET_MIN:
			moistureThres.minValue = Potentiometer_GetPercentPosition();
			FSM_StateTransition(&ptrButton->enter, STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MIN);
			Master_MessageEncode(ptrMasterToNode->data,
													 &ptrMasterToNode->minMoistStruct,
													 moistureThres.minValue);
			break;
		
		case SUBSTATE_SET_MAX:
			moistureThres.maxValue = Potentiometer_GetPercentPosition();
			FSM_StateTransition(&ptrButton->enter, STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MAX);
			Master_MessageEncode(ptrMasterToNode->data,
													 &ptrMasterToNode->maxMoistStruct,
													 moistureThres.maxValue);
			break;
	}
	
}
	
void FSM_Humidity(uint8_t substate)
{
	static sensorThreshold_t humidityThres;

	displaySensorThreshold(&humidityThres,
												 substate,
												 "HumMin:",
												 "HumMax:",
												 '%');
	switch(substate)
	{
		case SUBSTATE_HIGHLIGHT_MIN:
			FSM_StateTransition(&ptrButton->down, STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MAX);
			FSM_StateTransition(&ptrButton->forward, STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&ptrButton->enter, STATE_HUMIDITY, SUBSTATE_SET_MIN);
			FSM_StateTransition(&ptrButton->back, STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MIN);
			break;
		
		case SUBSTATE_HIGHLIGHT_MAX:
			FSM_StateTransition(&ptrButton->up, STATE_HUMIDITY,SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&ptrButton->forward, STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&ptrButton->enter, STATE_HUMIDITY, SUBSTATE_SET_MAX);	
			FSM_StateTransition(&ptrButton->back, STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MIN);
			break;
		
		case SUBSTATE_SET_MIN:
			humidityThres.minValue = Potentiometer_GetPercentPosition();
			FSM_StateTransition(&ptrButton->enter, STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MIN);
			Master_MessageEncode(ptrMasterToNode->data,
													 &ptrMasterToNode->minHumStruct,
													 humidityThres.minValue);
			break;
		
		case SUBSTATE_SET_MAX:
			humidityThres.maxValue = Potentiometer_GetPercentPosition();
			FSM_StateTransition(&ptrButton->enter, STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MAX);
			Master_MessageEncode(ptrMasterToNode->data,
													 &ptrMasterToNode->maxHumStruct,
													 humidityThres.maxValue);
			break;
	}
}


void FSM_Temperature(uint8_t substate)
{
	static sensorThreshold_t temperatureThres;
	
	displaySensorThreshold(&temperatureThres,
												 substate,
												 "TempMin:",
												 "TempMax:",
												 'C');
	switch (substate)
	{
		case SUBSTATE_HIGHLIGHT_MIN:
			FSM_StateTransition(&ptrButton->down, STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MAX);
			FSM_StateTransition(&ptrButton->forward, STATE_IRRIGATION_TIME, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&ptrButton->enter, STATE_TEMPERATURE, SUBSTATE_SET_MIN);
			FSM_StateTransition(&ptrButton->back, STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MIN);
			break;
		
		case SUBSTATE_HIGHLIGHT_MAX:
			FSM_StateTransition(&ptrButton->up, STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&ptrButton->forward, STATE_IRRIGATION_TIME, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&ptrButton->enter, STATE_TEMPERATURE, SUBSTATE_SET_MAX);	
			FSM_StateTransition(&ptrButton->back, STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MIN);
			break;
		
		case SUBSTATE_SET_MIN:
			temperatureThres.minValue = Potentiometer_GetPercentPosition();
			FSM_StateTransition(&ptrButton->enter, STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MIN);
			Master_MessageEncode(ptrMasterToNode->data,
													 &ptrMasterToNode->minTempStruct,
													 temperatureThres.minValue);
			break;
		
		case SUBSTATE_SET_MAX:
			temperatureThres.maxValue = Potentiometer_GetPercentPosition();
			FSM_StateTransition(&ptrButton->enter, STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MAX);	
			Master_MessageEncode(ptrMasterToNode->data,
													 &ptrMasterToNode->maxTempStruct,
													 temperatureThres.maxValue);
			break;
	}
}
	
void FSM_IrrigTime(uint8_t substate)
{
	static irrigTime_t irrigTime;
	
	displayIrrigTime(&irrigTime,
									 substate,
									 "TimeMin:",
									 "TimeMax:",
									 's');	
	switch(substate)
	{
		case SUBSTATE_HIGHLIGHT_MIN:
			FSM_StateTransition(&ptrButton->down, STATE_IRRIGATION_TIME, SUBSTATE_HIGHLIGHT_MAX);
	    FSM_StateTransition(&ptrButton->enter, STATE_IRRIGATION_TIME, SUBSTATE_SET_MIN);
	    FSM_StateTransition(&ptrButton->back, STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MIN);
			break;
		
		case SUBSTATE_HIGHLIGHT_MAX:
			FSM_StateTransition(&ptrButton->up, STATE_IRRIGATION_TIME, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&ptrButton->enter, STATE_IRRIGATION_TIME, SUBSTATE_SET_MAX);
			FSM_StateTransition(&ptrButton->back, STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MIN);
			break;
		
		case SUBSTATE_SET_MIN:
			irrigTime.minValue = Potentiometer_GetRawPosition();
			FSM_StateTransition(&ptrButton->enter, STATE_IRRIGATION_TIME, SUBSTATE_HIGHLIGHT_MIN);	
			Master_MessageEncode(ptrMasterToNode->data,
													 &ptrMasterToNode->minTimeStruct,
													 irrigTime.minValue);
			break;
		
		case SUBSTATE_SET_MAX:
			irrigTime.maxValue = Potentiometer_GetRawPosition();
			FSM_StateTransition(&ptrButton->enter, STATE_IRRIGATION_TIME, SUBSTATE_HIGHLIGHT_MAX);	
			Master_MessageEncode(ptrMasterToNode->data,
													 &ptrMasterToNode->maxTimeStruct,
													 irrigTime.maxValue);
			break;
	}
}

void FSM_Init(ButtonDataStructure* pButton,
							MasterMessageDataStructure* pMasterToNode,
							NodeMessageDataStructure* pNodeToMaster)
{
	ptrButton = pButton;
	ptrMasterToNode = pMasterToNode;
	ptrNodeToMaster = pNodeToMaster;
}

void FSM_Execute(ButtonDataStructure* pButton,
								 MasterMessageDataStructure* pMasterToNode,
							   NodeMessageDataStructure* pNodeToMaster)
{
	static void (*pStateFunction[NUMBER_OF_STATES])(uint8_t substate) = 
	{
		FSM_DisplayBme280Data,
		FSM_Node,
		FSM_Moisture,
		FSM_Humidity,
		FSM_Temperature,
		FSM_IrrigTime
	};

	pStateFunction[currentState](currentSubstate);
}

