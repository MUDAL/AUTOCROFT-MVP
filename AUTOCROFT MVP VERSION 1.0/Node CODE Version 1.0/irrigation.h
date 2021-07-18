#ifndef _IRRIGATION_H
#define _IRRIGATION_H

typedef enum 
{
	NO_IRRIGATION = 0,
	LIGHT_IRRIGATION,
	HEAVY_IRRIGATION,
	IRRIG_METHOD_UNDEFINED
}irrigMethod_t;

extern irrigMethod_t Irrigation_GetMethod(sensorLevel_t moistureLev,
																		      sensorLevel_t humidityLev,
																		      sensorLevel_t temperatureLev);

#endif //_IRRIGATION_H

