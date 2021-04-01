#include <stdint.h>
#include <stdbool.h>
#include "conversions.h"
#include "irrigation.h"
#include "sensor.h"
#include "lcd.h"
#include "FSM.h"
#include "display.h"

#define DELETE		' '

static void Display_Refresh(uint8_t row, uint8_t column)
{
	LCD_Set_Cursor(row,column);
	//Delete line
	for (uint8_t i = 0; i < 16; i++)
	{
		LCD_Write_Data(DELETE);
	}
}

//High level functions for displaying things on the LCD screen
void Display_Data(char* firstRowHeading,
								  char* secondRowHeading,
								  uint8_t row1Data,
								  uint8_t row2Data,
								  char measurementUnit1,
								  char measurementUnit2)
										
{
	char strData1[4] = "0";
	char strData2[4] = "0";
	static uint8_t prevRow1Data;
	static uint8_t prevRow2Data;
	
	//top
	if (prevRow1Data != row1Data)
	{
		Display_Refresh(0,0);
		prevRow1Data = row1Data;
	}
	
	LCD_Set_Cursor(0,0);
	LCD_Write_String(firstRowHeading);
	Conv_Integer_To_String(row1Data,strData1);
	LCD_Write_String(strData1);
	LCD_Set_Cursor(0,15);
	LCD_Write_Data(measurementUnit1);
	
	//bottom
	if (prevRow2Data != row2Data)
	{
		Display_Refresh(1,0);
		prevRow2Data = row2Data;
	}
	
	LCD_Set_Cursor(1,0);
	LCD_Write_String(secondRowHeading);
	Conv_Integer_To_String(row2Data,strData2);
	LCD_Write_String(strData2);
	LCD_Set_Cursor(1,15);
	LCD_Write_Data(measurementUnit2);
	
}


void Display_Node(char* firstRowHeading,
								  char* secondRowHeading,
									uint8_t displayMode,
								  uint8_t row1Data,
								  uint8_t row2Data,
								  char measurementUnit1,
								  char measurementUnit2)
										
{
	char strData1[4] = "0";
	char strData2[4] = "0";
	static uint8_t prevRow1Data;
	static uint8_t prevRow2Data;
	
	switch (displayMode)
	{
		case SUBSTATE_HIGHLIGHT_NODE_ID:
			//top
			LCD_Set_Cursor(0,0);
			LCD_Write_Data('>');
			LCD_Set_Cursor(0,3);
			LCD_Write_String(firstRowHeading);
			Conv_Integer_To_String(row1Data,strData1);
			LCD_Write_String(strData1);
			LCD_Set_Cursor(0,15);
			LCD_Write_Data(measurementUnit1);
			//bottom
			LCD_Set_Cursor(1,3);
			LCD_Write_String(secondRowHeading);
			Conv_Integer_To_String(row2Data,strData2);
			LCD_Write_String(strData2);
			LCD_Set_Cursor(1,15);
			LCD_Write_Data(measurementUnit2);
			break;
		
		case SUBSTATE_SET_NODE_ID:
			if (prevRow1Data != row1Data)
			{
				Display_Refresh(0,0);
				prevRow1Data = row1Data;
			}
			//top
			LCD_Set_Cursor(0,0);
			LCD_Write_Data('>');
			LCD_Set_Cursor(0,1);
			LCD_Write_Data('>');
			LCD_Set_Cursor(0,3);
			LCD_Write_String(firstRowHeading);
			Conv_Integer_To_String(row1Data,strData1);
			LCD_Write_String(strData1);
			LCD_Set_Cursor(0,15);
			LCD_Write_Data(measurementUnit1);
			//bottom			
			if (prevRow2Data != row2Data)
			{
				Display_Refresh(1,0);
				prevRow2Data = row2Data;
			}
			LCD_Set_Cursor(1,3);
			LCD_Write_String(secondRowHeading);
			Conv_Integer_To_String(row2Data,strData2);
			LCD_Write_String(strData2);
			LCD_Set_Cursor(1,15);
			LCD_Write_Data(measurementUnit2);
			break;
	}
}

void Display_SensorThreshold(sensorThreshold_t* pSensorThres,
														 uint8_t displayMode,
														 char* firstRowHeading,
														 char* secondRowHeading,
														 char measurementUnit)
{
	/*
	Description:
	
	Parameters:
	
	Return:
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
			LCD_Write_Data('>');
			LCD_Set_Cursor(0,3);
			LCD_Write_String(firstRowHeading);
			Conv_Integer_To_String(pSensorThres->minValue,strMin);
			LCD_Write_String(strMin);
			LCD_Set_Cursor(0,15);
			LCD_Write_Data(measurementUnit);
			//bottom
			LCD_Set_Cursor(1,3);
			LCD_Write_String(secondRowHeading);
			Conv_Integer_To_String(pSensorThres->maxValue,strMax);
			LCD_Write_String(strMax);
			LCD_Set_Cursor(1,15);
			LCD_Write_Data(measurementUnit);
			break;
		
		case SUBSTATE_HIGHLIGHT_MAX:
			//top
			LCD_Set_Cursor(0,3);
			LCD_Write_String(firstRowHeading);
			Conv_Integer_To_String(pSensorThres->minValue,strMin);
			LCD_Write_String(strMin);
			LCD_Set_Cursor(0,15);
			LCD_Write_Data(measurementUnit);
			//bottom
			LCD_Set_Cursor(1,0);
			LCD_Write_Data('>');
			LCD_Set_Cursor(1,3);
			LCD_Write_String(secondRowHeading);
			Conv_Integer_To_String(pSensorThres->maxValue,strMax);
			LCD_Write_String(strMax);
			LCD_Set_Cursor(1,15);
			LCD_Write_Data(measurementUnit);
			break;
		
		case SUBSTATE_SET_MIN:
			//top
			if (prevMinValue != pSensorThres->minValue)
			{
				Display_Refresh(0,0);
				prevMinValue = pSensorThres->minValue;
			}
			LCD_Set_Cursor(0,0);
			LCD_Write_Data('>');
			LCD_Set_Cursor(0,1);
			LCD_Write_Data('>');
			LCD_Set_Cursor(0,3);
			LCD_Write_String(firstRowHeading);
			Conv_Integer_To_String(pSensorThres->minValue,strMin);
			LCD_Write_String(strMin);
			LCD_Set_Cursor(0,15);
			LCD_Write_Data(measurementUnit);
			
			//bottom
			LCD_Set_Cursor(1,3);
			LCD_Write_String(secondRowHeading);
			Conv_Integer_To_String(pSensorThres->maxValue,strMax);
			LCD_Write_String(strMax);
			LCD_Set_Cursor(1,15);
			LCD_Write_Data(measurementUnit);
			break;
		
		case SUBSTATE_SET_MAX:
			//top
			LCD_Set_Cursor(0,3);
			LCD_Write_String(firstRowHeading);
		  Conv_Integer_To_String(pSensorThres->minValue,strMin);
			LCD_Write_String(strMin);
			LCD_Set_Cursor(0,15);
			LCD_Write_Data(measurementUnit);
			//bottom
			if (prevMaxValue != pSensorThres->maxValue)
			{
				Display_Refresh(1,0);
				prevMaxValue = pSensorThres->maxValue;
			}
			LCD_Set_Cursor(1,0);
			LCD_Write_Data('>');
			LCD_Set_Cursor(1,1);
			LCD_Write_Data('>');
			LCD_Set_Cursor(1,3);
			LCD_Write_String(secondRowHeading);
			Conv_Integer_To_String(pSensorThres->maxValue,strMax);
			LCD_Write_String(strMax);
			LCD_Set_Cursor(1,15);
			LCD_Write_Data(measurementUnit);
			break;
	}
	
}

void Display_IrrigationTime(irrigTime_t* pIrrigTime, 
														uint8_t displayMode,
														char* firstRowHeading,
														char* secondRowHeading,
														char measurementUnit)
{
	/*
	Description:
	
	Parameters:
	
	Return:
	*/
	char strMin[5] = "0";
	char strMax[5] = "0";
	static uint16_t prevMinValue;
	static uint16_t prevMaxValue;
			
	switch (displayMode)
	{
		case SUBSTATE_HIGHLIGHT_MIN:
			//top
			LCD_Set_Cursor(0,0);
			LCD_Write_Data('>');
			LCD_Set_Cursor(0,3);
			LCD_Write_String(firstRowHeading);
			Conv_Integer_To_String(pIrrigTime->minValue,strMin);
			LCD_Write_String(strMin);
			LCD_Set_Cursor(0,15);
			LCD_Write_Data(measurementUnit);
			//bottom
			LCD_Set_Cursor(1,3);
			LCD_Write_String(secondRowHeading);
			Conv_Integer_To_String(pIrrigTime->maxValue,strMax);
			LCD_Write_String(strMax);
			LCD_Set_Cursor(1,15);
			LCD_Write_Data(measurementUnit);
			break;
		
		case SUBSTATE_HIGHLIGHT_MAX:
			//top
			LCD_Set_Cursor(0,3);
			LCD_Write_String(firstRowHeading);
			Conv_Integer_To_String(pIrrigTime->minValue,strMin);
			LCD_Write_String(strMin);
			LCD_Set_Cursor(0,15);
			LCD_Write_Data(measurementUnit);
			//bottom
			LCD_Set_Cursor(1,0);
			LCD_Write_Data('>');
			LCD_Set_Cursor(1,3);
			LCD_Write_String(secondRowHeading);
			Conv_Integer_To_String(pIrrigTime->maxValue,strMax);
			LCD_Write_String(strMax);
			LCD_Set_Cursor(1,15);
			LCD_Write_Data(measurementUnit);
			break;
		
		case SUBSTATE_SET_MIN:
			//top
			if (prevMinValue != pIrrigTime->minValue)
			{
				Display_Refresh(0,0);
				prevMinValue = pIrrigTime->minValue;
			}
			LCD_Set_Cursor(0,0);
			LCD_Write_Data('>');
			LCD_Set_Cursor(0,1);
			LCD_Write_Data('>');
			LCD_Set_Cursor(0,3);
			LCD_Write_String(firstRowHeading);
			Conv_Integer_To_String(pIrrigTime->minValue,strMin);
			LCD_Write_String(strMin);
			LCD_Set_Cursor(0,15);
			LCD_Write_Data(measurementUnit);
			
			//bottom
			LCD_Set_Cursor(1,3);
			LCD_Write_String(secondRowHeading);
			Conv_Integer_To_String(pIrrigTime->maxValue,strMax);
			LCD_Write_String(strMax);
			LCD_Set_Cursor(1,15);
			LCD_Write_Data(measurementUnit);
			break;
		
		case SUBSTATE_SET_MAX:
			//top
			LCD_Set_Cursor(0,3);
			LCD_Write_String(firstRowHeading);
		  Conv_Integer_To_String(pIrrigTime->minValue,strMin);
			LCD_Write_String(strMin);
			LCD_Set_Cursor(0,15);
			LCD_Write_Data(measurementUnit);
		
			//bottom
			if (prevMaxValue != pIrrigTime->maxValue)
			{
				Display_Refresh(1,0);
				prevMaxValue = pIrrigTime->maxValue;
			}
			LCD_Set_Cursor(1,0);
			LCD_Write_Data('>');
			LCD_Set_Cursor(1,1);
			LCD_Write_Data('>');
			LCD_Set_Cursor(1,3);
			LCD_Write_String(secondRowHeading);
			Conv_Integer_To_String(pIrrigTime->maxValue,strMax);
			LCD_Write_String(strMax);
			LCD_Set_Cursor(1,15);
			LCD_Write_Data(measurementUnit);
			break;
	}
	
}
