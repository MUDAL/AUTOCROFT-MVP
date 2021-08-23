#ifndef KEYPAD_H
#define KEYPAD_H

/*
*		GPIO		|		KEYPAD		|		PIN-MODE			
*		PA1			|		ROW1			|		INPUT					
*		PA4			|		ROW2			|		INPUT
*		PA5			|		ROW3			|		INPUT
*		PA6			|		ROW4			|		INPUT
*		PB1			|		COLUMN1		|		OUTPUT
*		PB5			|		COLUMN2		|		OUTPUT
*		PB8			|		COLUMN3		|		OUTPUT
*		PB9	   	|		COLUMN4		|		OUTPUT	
*/


extern void Keypad_Init(void);
extern char Keypad_GetChar(void);
extern void Keypad_StrToInt(uint8_t* pInteger, char* pBuffer, uint8_t len);

#endif //KEYPAD_H
