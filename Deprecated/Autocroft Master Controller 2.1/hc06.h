#ifndef _HC06_H
#define _HC06_H

//Initializes the HC06 bluetooth module
extern void HC06_Init(char* hc06RxBuffer);
//Transmits data wirelessly via the HC06 module
extern void HC06_Transmit(char* pData);
/*Returns 'true' if HC06 module has received data and the
data is ready to be processed.
*/
extern bool HC06_Receiver_Data_Ready(sysTimer_t* pTimer, uint32_t numberOfBytes);

#endif //_HC06_H
