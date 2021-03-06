#include <stdint.h>
#include "sensor.h"

sensorLevel_t Sensor_GetLevel(sensor_t* pSensor)
{
	sensorLevel_t lev;
	
	if (pSensor->minValue < pSensor->maxValue)
	{
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
			lev = HIGH;
		}
	}
	
	else
	{
		lev = LEV_UNDEFINED;
	}
	
	return lev;
	
}

