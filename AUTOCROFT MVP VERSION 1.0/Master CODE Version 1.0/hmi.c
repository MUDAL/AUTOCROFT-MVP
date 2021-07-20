#include <stdint.h>
#include <stdbool.h>
#include "conversions.h" 
#include "button.h"
#include "potentiometer.h"
#include "lcd.h"
#include "bme280.h"
#include "hc12.h"
#include "communication.h"
#include "hmi.h"

/**
@Description The HMI(Human Machine Interface) application
handles user inputs, configuration of parameters, LCD outputs
and screen transitions. It is based on a Finite State Machine (FSM)
design.
*/

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
static uint8_t* ptrMasterToNode;
static uint8_t* ptrNodeToMaster;
static uint8_t* ptrNodeToMasterArray;
static uint8_t currentState = STATE_DISPLAY_READINGS;
static uint8_t currentSubstate = SUBSTATE_NIL;

//Function definitions
static void RefreshDisplay(uint8_t row, uint8_t column)
{
	LCD_SetCursor(row,column);
	//Delete line
	for (uint8_t i = 0; i < 16; i++)
	{
		LCD_WriteByte(' ');
	}
}

static void DisplayBme280Data(char* firstRowHeading,
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
		RefreshDisplay(0,0);
		prevRow1Data = row1Data;
	}
	LCD_SetCursor(0,0);
	LCD_WriteString(firstRowHeading);
	Conv_IntegerToString(row1Data,strData1);
	LCD_WriteString(strData1);
	LCD_WriteByte(measurementUnit1);
	
	//bottom
	if (prevRow2Data != row2Data)
	{
		RefreshDisplay(1,0);
		prevRow2Data = row2Data;
	}
	LCD_SetCursor(1,0);
	LCD_WriteString(secondRowHeading);
	Conv_IntegerToString(row2Data,strData2);
	LCD_WriteString(strData2);
	LCD_WriteByte(measurementUnit2);
}


static void DisplayNodeData(char* firstRowHeading,
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
		RefreshDisplay(1,0);
		prevRow2Data = row2Data;
	}
	
	switch (displayMode)
	{
		case SUBSTATE_HIGHLIGHT_NODE_ID:
			//top
			LCD_SetCursor(0,0);
			LCD_WriteByte('>');
			LCD_SetCursor(0,3);
			LCD_WriteString(firstRowHeading);
			Conv_IntegerToString(row1Data,strData1);
			LCD_WriteString(strData1);
			//bottom
			LCD_SetCursor(1,3);
			LCD_WriteString(secondRowHeading);
			Conv_IntegerToString(row2Data,strData2);
			LCD_WriteString(strData2);
			LCD_WriteByte('%');
			break;
		
		case SUBSTATE_SET_NODE_ID:
			if (prevRow1Data != row1Data)
			{
				RefreshDisplay(0,0);
				prevRow1Data = row1Data;
			}
			//top
			LCD_SetCursor(0,0);
			LCD_WriteString(">>");
			LCD_SetCursor(0,3);
			LCD_WriteString(firstRowHeading);
			Conv_IntegerToString(row1Data,strData1);
			LCD_WriteString(strData1);
			//bottom
			LCD_SetCursor(1,3);
			LCD_WriteString(secondRowHeading);
			Conv_IntegerToString(row2Data,strData2);
			LCD_WriteString(strData2);
			LCD_WriteByte('%');
			break;
	}
}

static void DisplayThreshold(uint16_t minValue,
														 uint16_t maxValue,
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
	char strMin[5] = "0";
	char strMax[5] = "0";
	static uint8_t prevMinValue;
	static uint8_t prevMaxValue;
			
	switch (displayMode)
	{
		case SUBSTATE_HIGHLIGHT_MIN:
			//top
			LCD_SetCursor(0,0);
			LCD_WriteByte('>');
			LCD_SetCursor(0,3);
			LCD_WriteString(firstRowHeading);
			Conv_IntegerToString(minValue,strMin);
			LCD_WriteString(strMin);
			LCD_WriteByte(measurementUnit);
			//bottom
			LCD_SetCursor(1,3);
			LCD_WriteString(secondRowHeading);
			Conv_IntegerToString(maxValue,strMax);
			LCD_WriteString(strMax);
			LCD_WriteByte(measurementUnit);
			break;
		
		case SUBSTATE_HIGHLIGHT_MAX:
			//top
			LCD_SetCursor(0,3);
			LCD_WriteString(firstRowHeading);
			Conv_IntegerToString(minValue,strMin);
			LCD_WriteString(strMin);
			LCD_WriteByte(measurementUnit);
			//bottom
			LCD_SetCursor(1,0);
			LCD_WriteByte('>');
			LCD_SetCursor(1,3);
			LCD_WriteString(secondRowHeading);
			Conv_IntegerToString(maxValue,strMax);
			LCD_WriteString(strMax);
			LCD_WriteByte(measurementUnit);
			break;
		
		case SUBSTATE_SET_MIN:
			//top
			if (prevMinValue != minValue)
			{
				RefreshDisplay(0,0);
				prevMinValue = minValue;
			}
			LCD_SetCursor(0,0);
			LCD_WriteString(">>");
			LCD_SetCursor(0,3);
			LCD_WriteString(firstRowHeading);
			Conv_IntegerToString(minValue,strMin);
			LCD_WriteString(strMin);
			LCD_WriteByte(measurementUnit);
			//bottom
			LCD_SetCursor(1,3);
			LCD_WriteString(secondRowHeading);
			Conv_IntegerToString(maxValue,strMax);
			LCD_WriteString(strMax);
			LCD_WriteByte(measurementUnit);
			break;
		
		case SUBSTATE_SET_MAX:
			//top
			LCD_SetCursor(0,3);
			LCD_WriteString(firstRowHeading);
		  Conv_IntegerToString(minValue,strMin);
			LCD_WriteString(strMin);
			LCD_WriteByte(measurementUnit);
			//bottom
			if (prevMaxValue != maxValue)
			{
				RefreshDisplay(1,0);
				prevMaxValue = maxValue;
			}
			LCD_SetCursor(1,0);
			LCD_WriteString(">>");
			LCD_SetCursor(1,3);
			LCD_WriteString(secondRowHeading);
			Conv_IntegerToString(maxValue,strMax);
			LCD_WriteString(strMax);
			LCD_WriteByte(measurementUnit);
			break;
	}
}

static void FSM_StateTransition(button_t* pButton,
																uint8_t state,
																uint8_t substate)
{
	if (Button_Read(pButton))
	{
		//Refresh LCD screen
		LCD_Clear(); 
		currentState = state;
		currentSubstate = substate;
	}
}

static void FSM_DisplayBme280Data(uint8_t substate)
{
	bme280_t bme280Data;
	BME280_GetData(&bme280Data);
	
	DisplayBme280Data("Hum: ",
										"Temp: ",
										bme280Data.humidity,
										bme280Data.temperature,
										'%',
										'C');
	FSM_StateTransition(&ptrButton->forward, STATE_NODE, SUBSTATE_HIGHLIGHT_NODE_ID);	
}

static void FSM_Node(uint8_t substate)
{
	static uint16_t nodeID;
	uint8_t nodeMoisture = ptrNodeToMasterArray[nodeID];
	
	DisplayNodeData("Node ID: ",
									"Moisture: ",
									substate,
									nodeID,
									nodeMoisture);

	switch (substate)
	{
		case SUBSTATE_HIGHLIGHT_NODE_ID:
			FSM_StateTransition(&ptrButton->back, STATE_DISPLAY_READINGS, SUBSTATE_NIL);
			FSM_StateTransition(&ptrButton->forward, STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MIN);
			//FSM_StateTransition(&ptrButton->enter, STATE_NODE, SUBSTATE_SET_NODE_ID);
			break;
			
		case SUBSTATE_SET_NODE_ID:
			nodeID = Potentiometer_GetPercentPosition();
			//FSM_StateTransition(&ptrButton->enter, STATE_NODE, SUBSTATE_HIGHLIGHT_NODE_ID);
			break;
	}	
}

static void FSM_Moisture(uint8_t substate)
{
	static uint8_t minMoist;
	static uint8_t maxMoist;
	
	DisplayThreshold(minMoist,
									 maxMoist,
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
			//FSM_StateTransition(&ptrButton->enter, STATE_MOISTURE, SUBSTATE_SET_MIN);
			break;
		
		case SUBSTATE_HIGHLIGHT_MAX:
			FSM_StateTransition(&ptrButton->back, STATE_NODE, SUBSTATE_HIGHLIGHT_NODE_ID);
			FSM_StateTransition(&ptrButton->up, STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&ptrButton->forward, STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MIN);
			//FSM_StateTransition(&ptrButton->enter, STATE_MOISTURE, SUBSTATE_SET_MAX);
			break;
		
		case SUBSTATE_SET_MIN:
			minMoist = Potentiometer_GetPercentPosition();
			//FSM_StateTransition(&ptrButton->enter, STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MIN);
			Master_EncodeTxData(ptrMasterToNode, minMoist, MIN_MOISTURE);
			break;
		
		case SUBSTATE_SET_MAX:
			maxMoist = Potentiometer_GetPercentPosition();
			//FSM_StateTransition(&ptrButton->enter, STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MAX);
			Master_EncodeTxData(ptrMasterToNode, maxMoist, MAX_MOISTURE);
			break;
	}
}
	
static void FSM_Humidity(uint8_t substate)
{
	static uint8_t minHum;
	static uint8_t maxHum;
	
	DisplayThreshold(minHum,
									 maxHum,
									 substate,
									 "HumMin:",
									 "HumMax:",
									 '%');
	switch(substate)
	{
		case SUBSTATE_HIGHLIGHT_MIN:
			FSM_StateTransition(&ptrButton->down, STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MAX);
			FSM_StateTransition(&ptrButton->forward, STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MIN);
			//FSM_StateTransition(&ptrButton->enter, STATE_HUMIDITY, SUBSTATE_SET_MIN);
			FSM_StateTransition(&ptrButton->back, STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MIN);
			break;
		
		case SUBSTATE_HIGHLIGHT_MAX:
			FSM_StateTransition(&ptrButton->up, STATE_HUMIDITY,SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&ptrButton->forward, STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MIN);
			//FSM_StateTransition(&ptrButton->enter, STATE_HUMIDITY, SUBSTATE_SET_MAX);	
			FSM_StateTransition(&ptrButton->back, STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MIN);
			break;
		
		case SUBSTATE_SET_MIN:
			minHum = Potentiometer_GetPercentPosition();
			//FSM_StateTransition(&ptrButton->enter, STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MIN);
			Master_EncodeTxData(ptrMasterToNode, minHum, MIN_HUMIDITY);
			break;
		
		case SUBSTATE_SET_MAX:
			maxHum = Potentiometer_GetPercentPosition();
			//FSM_StateTransition(&ptrButton->enter, STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MAX);
			Master_EncodeTxData(ptrMasterToNode, maxHum, MAX_HUMIDITY);
			break;
	}
}


static void FSM_Temperature(uint8_t substate)
{	
	static uint8_t minTemp;
	static uint8_t maxTemp;
	
	DisplayThreshold(minTemp,
									 maxTemp,
									 substate,
									 "TempMin:",
									 "TempMax:",
									 'C');
	switch (substate)
	{
		case SUBSTATE_HIGHLIGHT_MIN:
			FSM_StateTransition(&ptrButton->down, STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MAX);
			FSM_StateTransition(&ptrButton->forward, STATE_IRRIGATION_TIME, SUBSTATE_HIGHLIGHT_MIN);
			//FSM_StateTransition(&ptrButton->enter, STATE_TEMPERATURE, SUBSTATE_SET_MIN);
			FSM_StateTransition(&ptrButton->back, STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MIN);
			break;
		
		case SUBSTATE_HIGHLIGHT_MAX:
			FSM_StateTransition(&ptrButton->up, STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition(&ptrButton->forward, STATE_IRRIGATION_TIME, SUBSTATE_HIGHLIGHT_MIN);
			//FSM_StateTransition(&ptrButton->enter, STATE_TEMPERATURE, SUBSTATE_SET_MAX);	
			FSM_StateTransition(&ptrButton->back, STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MIN);
			break;
		
		case SUBSTATE_SET_MIN:
			minTemp = Potentiometer_GetPercentPosition();
			//FSM_StateTransition(&ptrButton->enter, STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MIN);
			Master_EncodeTxData(ptrMasterToNode, minTemp, MIN_TEMPERATURE);
			break;
		
		case SUBSTATE_SET_MAX:
			maxTemp = Potentiometer_GetPercentPosition();
			//FSM_StateTransition(&ptrButton->enter, STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MAX);	
			Master_EncodeTxData(ptrMasterToNode, maxTemp, MAX_TEMPERATURE);
			break;
	}
}
	
static void FSM_IrrigTime(uint8_t substate)
{
	static uint16_t minIrrigTime;
	static uint16_t maxIrrigTime;
	
	DisplayThreshold(minIrrigTime,
									 maxIrrigTime,
									 substate,
									 "TimeMin:",
									 "TimeMax:",
									 's');	
	switch(substate)
	{
		case SUBSTATE_HIGHLIGHT_MIN:
			FSM_StateTransition(&ptrButton->down, STATE_IRRIGATION_TIME, SUBSTATE_HIGHLIGHT_MAX);
	    //FSM_StateTransition(&ptrButton->enter, STATE_IRRIGATION_TIME, SUBSTATE_SET_MIN);
	    FSM_StateTransition(&ptrButton->back, STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MIN);
			break;
		
		case SUBSTATE_HIGHLIGHT_MAX:
			FSM_StateTransition(&ptrButton->up, STATE_IRRIGATION_TIME, SUBSTATE_HIGHLIGHT_MIN);
			//FSM_StateTransition(&ptrButton->enter, STATE_IRRIGATION_TIME, SUBSTATE_SET_MAX);
			FSM_StateTransition(&ptrButton->back, STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MIN);
			break;
		
		case SUBSTATE_SET_MIN:
			minIrrigTime = Potentiometer_GetRawPosition();
			//FSM_StateTransition(&ptrButton->enter, STATE_IRRIGATION_TIME, SUBSTATE_HIGHLIGHT_MIN);	
			Master_EncodeTxData(ptrMasterToNode, minIrrigTime, MIN_IRRIG_TIME);
			break;
		
		case SUBSTATE_SET_MAX:
			maxIrrigTime = Potentiometer_GetRawPosition();
			//FSM_StateTransition(&ptrButton->enter, STATE_IRRIGATION_TIME, SUBSTATE_HIGHLIGHT_MAX);	
			Master_EncodeTxData(ptrMasterToNode, maxIrrigTime, MAX_IRRIG_TIME);
			break;
	}
}

void HMI_Init(ButtonDataStructure* pButton,
							uint8_t* pMasterToNode,
							uint8_t* pNodeToMaster,
							uint8_t* pNodeToMasterArray)
{
	ptrButton = pButton;
	ptrMasterToNode = pMasterToNode;
	ptrNodeToMaster = pNodeToMaster;
	ptrNodeToMasterArray = pNodeToMasterArray;
}

void HMI_Execute(void)
{
	static bme280_t bme280Data;
	//Array of FSM function pointers
	static void (*pStateFunction[NUMBER_OF_STATES])(uint8_t substate) = 
	{
		FSM_DisplayBme280Data,
		FSM_Node,
		FSM_Moisture,
		FSM_Humidity,
		FSM_Temperature,
		FSM_IrrigTime
	};
	//Core of the HMI
	pStateFunction[currentState](currentSubstate);
	//If send button is pressed (# on keypad), send data to the nodes
	if (Button_Read(&ptrButton->send))
	{
		BME280_GetData(&bme280Data);
		Master_EncodeTxData(ptrMasterToNode,bme280Data.humidity,HUMIDITY);
		Master_EncodeTxData(ptrMasterToNode,bme280Data.temperature,TEMPERATURE);
		LCD_Clear();
		LCD_WriteString("Collecting data");
		LCD_SetCursor(1,0);
		LCD_WriteString("Please wait");
		//Send data to all nodes and receive their moisture readings
		Master_TransmitReceive(ptrMasterToNode,
													 MASTER_TX_DATA_SIZE,
													 ptrNodeToMaster,
													 ptrNodeToMasterArray,
													 NO_OF_NODES,
													 150);
		LCD_Clear();
	}
	
}
