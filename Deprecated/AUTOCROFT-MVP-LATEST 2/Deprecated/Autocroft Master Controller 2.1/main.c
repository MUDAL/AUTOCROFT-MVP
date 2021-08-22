#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "sysTimer_struct.h"
#include "system.h"
#include "hc06.h"
#include "esp8266.h"
#include "sim800l.h"

#define HC06_RX_BUFFER_SIZE				30
#define SIM800L_BUFFER_SIZE				150 

static char sim800lRcvBuffer[ SIM800L_BUFFER_SIZE ];

static void Send_WiFi_Status_To_Bluetooth(uint8_t wifi_status);

int main(void)
{
	//contains data received from HC06 bluetooth module
	static char hc06RxBuffer[ HC06_RX_BUFFER_SIZE ]; 
	//static char sim800lRcvBuffer[ SIM800L_BUFFER_SIZE ];
	
	//bluetooth and gsm software timers
	sysTimer_t hc06RxTimer;
	sysTimer_t sim800lRxTimer;
	
	//bluetooth and wifi default status initialization
	bool hc06RxIsReady = false;
	bool sim800lRxActive = false;
	uint8_t esp8266Status = WIFI_IDLE;
	
	//System and module initialization
	System_Init();
	System_Software_Timer_Init(&hc06RxTimer,1000);
	System_Software_Timer_Init(&sim800lRxTimer,200);
	SIM800L_Init(sim800lRcvBuffer);
	HC06_Init(hc06RxBuffer);
	ESP8266_Init();
	
	//Setup for receiving SMS
	SIM800L_Transmit_String("AT+CMGF=1\r\n");
	System_Timer_DelayMs(50);
	SIM800L_Transmit_String("AT+CNMI=1,2,0,0,0\r\n");
	System_Timer_DelayMs(50);

	while(1)
	{
		
		hc06RxIsReady = HC06_Receiver_Data_Ready(&hc06RxTimer,HC06_RX_BUFFER_SIZE);
		
		if (hc06RxIsReady)
		{
			HC06_Transmit("RECEIVED\n");
			ESP8266_Transmit(hc06RxBuffer);
		}
		
		esp8266Status = ESP8266_Receive_Status();
		Send_WiFi_Status_To_Bluetooth(esp8266Status);
		
		if ( SIM800L_Is_Ringing() )
		{
			if ( !(SIM800L_Receiver_Data_Ready(&sim800lRxTimer,SIM800L_BUFFER_SIZE)) )
			{
				sim800lRxActive = true;
			}
		}
		
		if (sim800lRxActive)
		{
			//do stuff
			if ( (SIM800L_Receiver_Data_Ready(&sim800lRxTimer,SIM800L_BUFFER_SIZE)) )
			{
				sim800lRxActive = false;
			}	
		}
		
	}
	
}	

void Send_WiFi_Status_To_Bluetooth(uint8_t wifi_status)
{
	switch(wifi_status)
	{
		case SSID_RECEIVED:
			HC06_Transmit("SSID OK\n");
			break;
		
		case PASSWORD_RECEIVED:
			HC06_Transmit("PSWD OK\n");
			break;
		
		case WIFI_CONNECTED:
			HC06_Transmit("Connected to Wi-Fi\n");
			break;
		
		case WIFI_DISCONNECTED:
			HC06_Transmit("Disconnected from Wi-Fi\n");
			break;
	}
}

