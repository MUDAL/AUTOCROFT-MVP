#include <stdint.h>
#include "sensor.h"

sensorLevel_t Sensor_Get_Level(sensor_t* pSensor)
{
	sensorLevel_t lev;
	
	if (pSensor->value < pSensor->maxValue)
	{
		if (pSensor->value <= pSensor->minValue)
		{
			lev = LOW;
		}
		
		else
		{
			lev = AVERAGE;
		}
	}
	
	else
	{
		if (pSensor->value > pSensor->minValue)
		{
			lev = HIGH;
		}
		else
		{
			lev = LEV_UNDEFINED;
		}
	}
	
	return lev;
	
}


