#ifndef _SENSOR_H
#define _SENSOR_H

typedef enum 
{
	
	LOW = 0,
	AVERAGE,
	HIGH,
	UNDEFINED
	
}sensorLevel_t;

typedef struct
{
	uint8_t value;
	uint8_t minValue;
	uint8_t maxValue;
	
}sensor_t;

extern sensorLevel_t Sensor_Get_Level(sensor_t* pSensor);

#endif //_SENSOR_H
