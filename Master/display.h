#ifndef _DISPLAY_H
#define _DISPLAY_H

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
