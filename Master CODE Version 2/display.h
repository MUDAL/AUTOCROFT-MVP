#ifndef _DISPLAY_H
#define _DISPLAY_H

extern void Display_Data(char* firstRowHeading,
												 char* secondRowHeading,
												 uint8_t row1Data,
												 uint8_t row2Data,
												 char measurementUnit1,
												 char measurementUnit2);
												 
extern void Display_Node(char* firstRowHeading,
												 char* secondRowHeading,
												 uint8_t displayMode,
												 uint8_t row1Data,
												 uint8_t row2Data,
												 char measurementUnit1,
												 char measurementUnit2);												 
																
extern void Display_SensorThreshold(sensorThreshold_t* pSensorThres,
																		uint8_t displayMode,
																		char* firstRowHeading,
																		char* secondRowHeading,
																		char measurementUnit);
																		
extern void Display_IrrigationTime(irrigTime_t* pIrrigTime, 
																	 uint8_t displayMode,
														       char* firstRowHeading,
														       char* secondRowHeading,
														       char measurementUnit);

#endif //_DISPLAY_H
