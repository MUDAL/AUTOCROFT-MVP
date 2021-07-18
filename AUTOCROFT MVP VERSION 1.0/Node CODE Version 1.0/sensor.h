#ifndef _SENSOR_H
#define _SENSOR_H

typedef enum 
{
	LOW = 0,
	AVERAGE,
	HIGH,
	LEVEL_UNDEFINED
}sensorLevel_t;

extern sensorLevel_t Sensor_GetLevel(uint8_t currentReading,
																		 uint8_t minValue,
																		 uint8_t maxValue);

#endif //_SENSOR_H
