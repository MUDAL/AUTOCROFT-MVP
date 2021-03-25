#include <stdint.h>
#include "sensor.h"

sensorLevel_t Sensor_Get_Level(sensor_t* pSensor)
{
	sensorLevel_t lev;
	
	if (pSensor->value <= pSensor->minValue)
	{
		lev = LOW;
	}
	
	else if ( (pSensor->value > pSensor->minValue) && (pSensor->value < pSensor->maxValue) )
	{
		lev = AVERAGE;
	}
	
	else if (pSensor->value >= pSensor->maxValue)
	{
		lev = HIGH;
	}
	
	else
	{
		lev = UNDEFINED;
	}
	
	return lev;
	
}


