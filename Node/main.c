#include <stdint.h>
#include <stdbool.h>
#include "conversions.h"
#include "sysTimer_struct.h"
#include "system.h"
#include "sensor.h"
#include "irrigation.h"
#include "cms.h"
#include "solenoid.h"
#include "hc12.h"
#include "message.h"

//Globals
static char masterToNodeMessage[MASTER_TO_NODE_MSG_SIZE];

static sensor_t moisture;
static sensor_t humidity;
static sensor_t temperature;
	
static irrig_t irrigation;
	
static message_t minMoistureMsg;
static message_t maxMoistureMsg;
static message_t minHumidityMsg;
static message_t maxHumidityMsg;
static message_t minTemperatureMsg;
static message_t maxTemperatureMsg;
static message_t minIrrigTimeMsg;
static message_t maxIrrigTimeMsg;
static message_t humidityMsg;
static message_t temperatureMsg;

static void Node_Get_All_Data(void);

int main(void)
{
	
	//Local variables
	sysTimer_t irrigAlarm;
	
	System_Init();
	CMS_Init();
	Solenoid_Init();
	HC12_Tx_Init();
	HC12_Rx_Init(masterToNodeMessage);
	
	message_t* pMsgStructArr[10] = 
	{
		&minMoistureMsg,
		&maxMoistureMsg,
		&minHumidityMsg,
		&maxHumidityMsg,
		&minTemperatureMsg,
		&maxTemperatureMsg,
		&minIrrigTimeMsg,
		&maxIrrigTimeMsg,
		&humidityMsg,
		&temperatureMsg
	};
	
	const uint8_t colonIndexArr[10] = 
	{
		MIN_MOISTURE_COLON_INDEX,
		MAX_MOISTURE_COLON_INDEX,
		MIN_HUMIDITY_COLON_INDEX,
		MAX_HUMIDITY_COLON_INDEX,
		MIN_TEMPERATURE_COLON_INDEX,
		MAX_TEMPERATURE_COLON_INDEX,
		MIN_IRRIG_TIME_COLON_INDEX,
		MAX_IRRIG_TIME_COLON_INDEX,
		HUMIDITY_COLON_INDEX,
		TEMPERATURE_COLON_INDEX
	};
	
	const uint8_t commaIndexArr[10] = 
	{
		MIN_MOISTURE_COMMA_INDEX,
		MAX_MOISTURE_COMMA_INDEX,
		MIN_HUMIDITY_COMMA_INDEX,
		MAX_HUMIDITY_COMMA_INDEX,
		MIN_TEMPERATURE_COMMA_INDEX,
		MAX_TEMPERATURE_COMMA_INDEX,
		MIN_IRRIG_TIME_COMMA_INDEX,
		MAX_IRRIG_TIME_COMMA_INDEX,
		HUMIDITY_COMMA_INDEX,
		TEMPERATURE_COMMA_INDEX
	};
	
	//Initialize message structs. Message struct contains
	//information common to all parts of the entire message array.
	for(uint8_t i = 0; i < 10; i++)
	{
		Message_Struct_Init(pMsgStructArr[i],
												colonIndexArr[i],
												commaIndexArr[i]);
	}
	
	while(1)
	{
		
		if (HC12_Rx_Done_Receiving())
		{
			HC12_Rx_Restart();
			Node_Get_All_Data();
			
			switch (irrigation.wateringMethod)
			{
				case NO_WATERING:
				case IRRIG_METHOD_UNDEFINED:
					Solenoid_Switch(SOLENOID_OFF);
					break;
				
				case LIGHT_WATERING:
					//convert minimum irrigation time to milliseconds
					System_Alarm_Init(&irrigAlarm, (irrigation.minTime * 1000));
					Solenoid_Switch(SOLENOID_ON);
					break;
				
				case HEAVY_WATERING:
					//convert maximum irrigation time to milliseconds
					System_Alarm_Init(&irrigAlarm, (irrigation.maxTime * 1000));
					Solenoid_Switch(SOLENOID_ON);
					break;
			}
		}
		
		if (Solenoid_IsRunning())
		{
			if (System_Alarm_Ready(&irrigAlarm))
			{
				Solenoid_Switch(SOLENOID_OFF);
			}
		}
		
	}
	
}

void Node_Get_All_Data(void)
{
	moisture.value = CMS_Get_Moisture();
	moisture.minValue = Message_Decode(&minMoistureMsg, masterToNodeMessage);
	moisture.maxValue = Message_Decode(&maxMoistureMsg, masterToNodeMessage);
	moisture.level = Sensor_Get_Level(&moisture);
	
	humidity.value = Message_Decode(&humidityMsg, masterToNodeMessage);
	humidity.minValue = Message_Decode(&minHumidityMsg, masterToNodeMessage);
	humidity.maxValue = Message_Decode(&maxHumidityMsg, masterToNodeMessage);
	humidity.level = Sensor_Get_Level(&humidity);
	
	temperature.value = Message_Decode(&temperatureMsg, masterToNodeMessage);
	temperature.minValue = Message_Decode(&minTemperatureMsg, masterToNodeMessage);
	temperature.maxValue = Message_Decode(&maxTemperatureMsg, masterToNodeMessage);
	temperature.level = Sensor_Get_Level(&temperature);
	
	irrigation.wateringMethod = Irrigation_Get_Method(moisture.level,
																										humidity.level,
																										temperature.level);
	irrigation.minTime = Message_Decode(&minIrrigTimeMsg, masterToNodeMessage);
	irrigation.maxTime = Message_Decode(&maxIrrigTimeMsg, masterToNodeMessage);	
}
