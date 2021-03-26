#include <stdint.h>
#include <stdbool.h>
#include "conversions.h"
#include "sysTimer_struct.h"
#include "sensor.h"
#include "irrigation.h"
#include "system.h"
#include "cms.h"
#include "solenoid.h"
#include "hc12.h"
#include "message.h"

static char masterToNodeMessage[MASTER_TO_NODE_MSG_SIZE];

int main(void)
{
	//Local variables
	static sensor_t moisture;
	static sensor_t humidity;
	static sensor_t temperature;
	
	static irrig_t irrigTime;
	
	System_Init();
	CMS_Init();
	Solenoid_Init();
	HC12_Tx_Init();
	HC12_Rx_Init(masterToNodeMessage);
	
	while(1)
	{
		/*
		Expect/Receive message from master. get all configuration data
		(thresholds) and store in EEPROM.
		*/
		
		/*
		Read from EEPROM;
		if appropriate data or previously stored data is in the EEPROM,
		proceed to the following otherwise continue expecting data
		from super node;
		
		//soilMoisture = CMS_Get_Moisture();
		and....
		compare humidity,temperature and soil moisture with
		the thresholds and determine the irrigation method.
		
		*/
		
		if (HC12_Rx_Done_Receiving())
		{
			HC12_Rx_Restart();
		}
		
	}
	
}
