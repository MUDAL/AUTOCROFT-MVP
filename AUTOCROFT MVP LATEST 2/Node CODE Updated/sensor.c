#include <stdint.h>
#include "sensor.h"

sensorLevel_t Sensor_GetLevel(uint8_t currentReading,
															uint8_t minValue,
															uint8_t maxValue)
{
	sensorLevel_t lev;
	
	if (minValue < maxValue)
	{
		if (currentReading < maxValue)
		{
			if (currentReading <= minValue)
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

