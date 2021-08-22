#include <stdint.h>
#include "sensor.h"
#include "irrigation.h"

/*
									Cases for irrigation
										
S/N		Moisture		Humidity					Temperature					Irrigation method
1			Low					Low								Low									Light
2			Low					Low								Average							Light
3			Low					Low								High								Heavy
4			Low					Average						Low									Light
5			Low					Average						Average							Light
6			Low					Average						High								Heavy
7			Low					High 							Low									Nil
8			Low					High 							Average							Light
9			Low					High							High								Nil
10		Average			Low								Low									Nil
11		Average			Low								Average							Nil
12		Average 		Low								High								Light
13		Average			Average 					Low									Nil
14		Average 		Average						Average							Nil
15		Average			Average						High								Light
16		Average			High							Low									Nil
17		Average			High							Average							Nil
18		Average			High							High								Light
19		High				Low								Low									Nil
20		High				Low								Average							Nil
21		High				Low								High								Light
22		High				Average						Low									Nil
23		High				Average						Average							Nil
24		High				Average						High								Light
25		High				High							Low									Nil
26		High				High							Average							Nil
27		High				High							High								Nil

*/

//Lookup table containing the 27 cases for irrigation

const irrigMethod_t irrigCase[3][3][3] = {
	
	{ {LIGHT_WATERING,LIGHT_WATERING,HEAVY_WATERING}, 
		{LIGHT_WATERING,LIGHT_WATERING,HEAVY_WATERING}, 
		{NO_WATERING,LIGHT_WATERING,NO_WATERING}
	},
	
	{ {NO_WATERING,NO_WATERING,LIGHT_WATERING}, 
		{NO_WATERING,NO_WATERING,LIGHT_WATERING}, 
		{NO_WATERING,NO_WATERING,LIGHT_WATERING} 
	},
	
	{ {NO_WATERING,NO_WATERING,LIGHT_WATERING}, 
		{NO_WATERING,NO_WATERING,LIGHT_WATERING}, 
		{NO_WATERING,NO_WATERING,NO_WATERING}
	}
	
}; 

irrigMethod_t Irrigation_GetMethod(sensorLevel_t moistureLev,
																	 sensorLevel_t humidityLev,
																	 sensorLevel_t temperatureLev)
{
	if ( (moistureLev == LEV_UNDEFINED) ||
		   (humidityLev == LEV_UNDEFINED) ||
			 (temperatureLev == LEV_UNDEFINED) )
	{
		return IRRIG_METHOD_UNDEFINED;
	}
	return irrigCase[moistureLev][humidityLev][temperatureLev];
}

