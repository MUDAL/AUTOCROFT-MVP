#include <stdint.h>
#include "conversions.h" 
#include "lcd.h"
#include "communication.h"
#include "display.h"

static void RefreshDisplay(uint8_t row, uint8_t column)
{
	LCD_SetCursor(row,column);
	//Delete line
	for(uint8_t i = 0; i < 16; i++)
	{
		LCD_WriteByte(' ');
	}
}

void Display_Bme280Data(char* firstRowHeading,
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
	if(prevRow1Data != row1Data)
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
	if(prevRow2Data != row2Data)
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


void Display_NodeData(char* firstRowHeading,
										  char* secondRowHeading,
										  uint8_t displayMode,
										  uint8_t row1Data,
										  uint8_t row2Data)
										
{
	char strData1[4] = "0";
	char strData2[4] = "0";
	static uint8_t prevRow1Data;
	static uint8_t prevRow2Data;
	
	if(prevRow2Data != row2Data)
	{
		RefreshDisplay(1,0);
		prevRow2Data = row2Data;
	}
	
	switch(displayMode)
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
			if(row1Data > (NO_OF_NODES - 1))
			{
				LCD_SetCursor(1,3);
				LCD_WriteString(secondRowHeading);
				LCD_WriteString("NIL");
			}
			else
			{
				LCD_SetCursor(1,3);
				LCD_WriteString(secondRowHeading);
				Conv_IntegerToString(row2Data,strData2);
				LCD_WriteString(strData2);
				LCD_WriteByte('%');
			}
			break;
		
		case SUBSTATE_SET_NODE_ID:
			if(prevRow1Data != row1Data)
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
			if(row1Data > (NO_OF_NODES - 1))
			{
				LCD_SetCursor(1,3);
				LCD_WriteString(secondRowHeading);
				LCD_WriteString("NIL");
			}
			else
			{
				LCD_SetCursor(1,3);
				LCD_WriteString(secondRowHeading);
				Conv_IntegerToString(row2Data,strData2);
				LCD_WriteString(strData2);
				LCD_WriteByte('%');
			}
			break;
	}
}

void Display_Threshold(uint16_t minValue,
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
			
	switch(displayMode)
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
			if(prevMinValue != minValue)
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
			if(prevMaxValue != maxValue)
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
