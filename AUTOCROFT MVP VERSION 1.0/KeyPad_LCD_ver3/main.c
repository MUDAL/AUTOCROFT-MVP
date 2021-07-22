#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "sysTimer_struct.h"
#include "clocks.h"
#include "systick.h"
#include "keypad.h"
#include "lcd.h"

uint8_t keyPad_char;
char data[3] = "00";

int main(void)
{
	Clocks_Init();
	keyPad_Init();
	SysTick_Init();
	LCD_Init();

	LCD_Clear();
	
	LCD_Write_String("HUMIDITY: ");
	LCD_Set_Cursor(0, 10);
	LCD_Write_String(data);
	
	
	while(1)
	{
		keyPad_char = get_keyPadChar();
		
		if(keyPad_char != '\0')
		{
			LCD_Set_Cursor(0,10);
			registerKeyPress(keyPad_char, data, 2);
			LCD_Write_String(data);
			
		}
				

	}
}
