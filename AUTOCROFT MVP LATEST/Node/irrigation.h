#ifndef _IRRIGATION_H
#define _IRRIGATION_H

typedef enum 
{
	
	NO_WATERING = 0,
	LIGHT_WATERING,
	HEAVY_WATERING,
	IRRIG_METHOD_UNDEFINED
	
}irrigMethod_t;

typedef struct
{
	irrigMethod_t wateringMethod;
	uint16_t minTime;
	uint16_t maxTime;
	
}irrig_t;

extern irrigMethod_t Irrigation_GetMethod(sensorLevel_t moistureLev,
																		      sensorLevel_t humidityLev,
																		      sensorLevel_t temperatureLev);

#endif //_IRRIGATION_H

