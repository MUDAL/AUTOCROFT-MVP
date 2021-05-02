#ifndef _SIM800L_H
#define _SIM800L_H

//Initializes the SIM800L radio module
extern void SIM800L_Init(char* sim800lRxBuffer);
//Transmits single byte data wirelessly via the SIM800L module
extern void SIM800L_Transmit_Byte(char data);
//Transmits multibyte data wirelessly via the SIM800L module
extern void SIM800L_Transmit_String(char* pData);

extern bool SIM800L_Receiver_Data_Ready(sysTimer_t* pTimer, uint32_t numberOfBytes);
//If an SMS or call is received, the SIM800L Ring pin pulses low for about 120ms
extern bool SIM800L_Is_Ringing(void);

extern void SIM800L_Get_SMS(char* sim800lRxBuffer,
														uint32_t sim800lRxBufferSize,
														char* smsBuffer);

#endif //_SIM800L_H
