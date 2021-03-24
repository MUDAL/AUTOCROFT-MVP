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
			Display_Refresh(0,0);
			//top
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
			Display_Refresh(1,0);
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
			Display_Refresh(0,0);
			//top
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
			Display_Refresh(1,0);
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

