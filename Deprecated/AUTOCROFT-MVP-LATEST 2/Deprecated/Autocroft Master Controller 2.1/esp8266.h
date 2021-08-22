#ifndef _ESP8266_H
#define _ESP8266_H

//Status codes returned by the 'ESP8266' module
enum ESP8266_Status{
	WIFI_IDLE = 0,
	SSID_RECEIVED,
	PASSWORD_RECEIVED,
	WIFI_CONNECTED,
	WIFI_DISCONNECTED
};

//Initializes the ESP8266 WiFi module
extern void ESP8266_Init(void);
//Transmits data wirelessly via the ESP8266 module
extern void ESP8266_Transmit(char* data);
/*Returns any of the 5 'ESP8266_Status' codes above depending
on the data sent to the ESP8266 WiFi module.
*/
extern uint8_t ESP8266_Receive_Status(void);

#endif //_ESP8266_H
