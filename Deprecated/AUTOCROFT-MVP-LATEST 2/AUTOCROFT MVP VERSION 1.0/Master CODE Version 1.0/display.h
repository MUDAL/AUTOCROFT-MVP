#ifndef _DISPLAY_H
#define _DISPLAY_H

#define NUMBER_OF_STATES 					6

enum DisplayStates
{
	STATE_DISPLAY_READINGS = 0,
	STATE_NODE,
	STATE_MOISTURE,
	STATE_HUMIDITY,
	STATE_TEMPERATURE,
	STATE_IRRIGATION_TIME
};

enum DisplaySubstates
{
	SUBSTATE_NIL = 0,
	SUBSTATE_HIGHLIGHT_NODE_ID,
	SUBSTATE_SET_NODE_ID,
	SUBSTATE_HIGHLIGHT_MIN,
	SUBSTATE_HIGHLIGHT_MAX,
	SUBSTATE_SET_MIN,
	SUBSTATE_SET_MAX
};

extern void Display_Bme280Data(char* firstRowHeading,
															 char* secondRowHeading,
															 uint8_t row1Data,
															 uint8_t row2Data,
															 char measurementUnit1,
															 char measurementUnit2);

extern void Display_NodeData(char* firstRowHeading,
													   char* secondRowHeading,
													   uint8_t displayMode,
													   uint8_t row1Data,
													   uint8_t row2Data);
														
extern void Display_Threshold(uint16_t minValue,
														  uint16_t maxValue,
														  uint8_t displayMode,
														  char* firstRowHeading,
														  char* secondRowHeading,
														  char measurementUnit);
														 
#endif //_DISPLAY_H

