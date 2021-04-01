#ifndef _HC12_H
#define _HC12_H

#define HC12_RX_BUFFER_SIZE				15
#define HC12_USE_INTERRUPTS				1

//Initializes the HC12 module
extern void HC12_Tx_Init(void);
extern void HC12_Rx_Init(char* hc12RxBuffer);
//Transmits data wirelessly via the HC12 module
extern void HC12_Transmit(char* pData);
/*Returns 'true' if HC12 module has received data and the
data is ready to be processed.
*/
extern bool HC12_Rx_Done_Receiving(void);

extern void HC12_Rx_Restart(void);

#endif //_HC12_H
