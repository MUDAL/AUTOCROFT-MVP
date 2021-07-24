#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "gpio.h"
#include "systick.h"
#include "lcd.h"

#define LCD_RS														GPIO_PIN11
#define LCD_EN														GPIO_PIN12
#define LCD_D4														GPIO_PIN12	
#define LCD_D5														GPIO_PIN13
#define LCD_D6														GPIO_PIN14
#define LCD_D7														GPIO_PIN15

#define LCD_DATA_MODE												  true
#define LCD_CMD_MODE												  false

#define CMD_FUNCTION_SET_8BIT									0x03 
#define CMD_FUNCTION_SET_4BIT									0x02 
#define CMD_FUNCTION_SET_2LINE_5X8_DOT				0x28 
#define CMD_CLEAR_DISPLAY											0x01
#define CMD_DISPLAY_ON_CURSOR_ON							0x0E
#define CMD_DISPLAY_ON_CURSOR_OFF							0x0C
#define CMD_ENTRY_MODE_INCREMENT_CURSOR				0x06

/*
All data and commands are stored in the LCD's DDRAM
*/

/*
NB: For 4-bit mode, send high nibble first then send
low nibble.
*/

/*Two-dimensional array to map column and row
combinations to 16x2 LCD DDRAM addresses. (see HD44780 datasheet)
*/
const uint8_t ddramAddr[2][16] = {{0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
																	 0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F},
																	{0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,
																	 0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F}};

static void LCD_Write(bool regSelect, char byte)
{
	uint8_t lowNibble = byte << 4;
	
	//Select instruction register or data register 
	GPIO_OutputWrite(GPIOA, LCD_RS, regSelect);
	//Send high nibble to DB4-DB7
	GPIO_OutputClearReg(GPIOB,
										 (1<<LCD_D4)|
										 (1<<LCD_D5)|
										 (1<<LCD_D6)|
										 (1<<LCD_D7));
	
	GPIO_OutputRegWrite(GPIOB,
										 (((byte & (1<<4)) >> 4) << LCD_D4)|
										 (((byte & (1<<5)) >> 5) << LCD_D5)|
										 (((byte & (1<<6)) >> 6) << LCD_D6)|
										 (((byte & (1<<7)) >> 7) << LCD_D7) );
	
	//High to Low pulse
	GPIO_OutputWrite(GPIOA,LCD_EN,true);
	SysTick_DelayMs(1);
	GPIO_OutputWrite(GPIOA,LCD_EN,false);
	SysTick_DelayMs(1);
	
	//Send low nibble to DB4-DB7
	GPIO_OutputClearReg(GPIOB,
										 (1<<LCD_D4)|
										 (1<<LCD_D5)|
										 (1<<LCD_D6)|
										 (1<<LCD_D7));
	
	GPIO_OutputRegWrite(GPIOB,
										 (((lowNibble & (1<<4)) >> 4) << LCD_D4)|
										 (((lowNibble & (1<<5)) >> 5) << LCD_D5)|
										 (((lowNibble & (1<<6)) >> 6) << LCD_D6)|
										 (((lowNibble & (1<<7)) >> 7) << LCD_D7) );
	
	//High to Low Pulse
	GPIO_OutputWrite(GPIOA,LCD_EN,true);
	SysTick_DelayMs(1);
	GPIO_OutputWrite(GPIOA,LCD_EN,false);
	SysTick_DelayMs(1);
	
}

void LCD_Init(void)
{
	//PA11 and PA12 as output
	GPIO_OutputInit(GPIOA,
									GPIO_PORT_REG_HIGH,
									GPIO_PIN11_OUTPUT_MODE_2MHZ | GPIO_PIN12_OUTPUT_MODE_2MHZ,
									GPIO_GEN_PUR_OUTPUT_PUSH_PULL);
	
	//PB12-15 as output
	GPIO_OutputInit(GPIOB,
									GPIO_PORT_REG_HIGH,
								 (GPIO_PIN12_OUTPUT_MODE_2MHZ | 
									GPIO_PIN13_OUTPUT_MODE_2MHZ | 
									GPIO_PIN14_OUTPUT_MODE_2MHZ | 
									GPIO_PIN15_OUTPUT_MODE_2MHZ),
									GPIO_GEN_PUR_OUTPUT_PUSH_PULL);
	
	//Initialization sequence (according to HD44780 datasheet)
	SysTick_DelayMs(16); //Power-on delay (must be greater than 15ms for 4.5v and 40ms for 2.7v)
	LCD_Write(LCD_CMD_MODE,CMD_FUNCTION_SET_8BIT);
	SysTick_DelayMs(5); //wait for more than 4.1ms
	LCD_Write(LCD_CMD_MODE,CMD_FUNCTION_SET_8BIT);
	SysTick_DelayMs(1); //wait for more than 100us
	
	//Select all initialization commands for 4 bit operation
	LCD_Write(LCD_CMD_MODE,CMD_FUNCTION_SET_4BIT);
	LCD_Write(LCD_CMD_MODE,CMD_FUNCTION_SET_2LINE_5X8_DOT); 
	LCD_Write(LCD_CMD_MODE,CMD_CLEAR_DISPLAY);
	LCD_Write(LCD_CMD_MODE,CMD_DISPLAY_ON_CURSOR_OFF); 
	LCD_Write(LCD_CMD_MODE,CMD_ENTRY_MODE_INCREMENT_CURSOR); 
}

void LCD_WriteByte(char data)
{
	//Select data register and send data
	LCD_Write(LCD_DATA_MODE,data);
	
}

void LCD_WriteString(char* pData)
{
	/*
	Description:
	
	Parameters:
	
	Return:
	
	*/

	while(*pData != '\0')
	{
		LCD_WriteByte(*pData);
		pData++;
	}
	
}

void LCD_Clear(void)
{
	//Select instruction register then clear the screen
	LCD_Write(LCD_CMD_MODE, CMD_CLEAR_DISPLAY);
}

void LCD_SetCursor(uint8_t row, uint8_t column)
{
	if ((row > 1) || (column > 16))
	{
		//Invalid
		return;
	}
	//Select instruction register then set cursor position
	LCD_Write(LCD_CMD_MODE, ((1<<7) | ddramAddr[row][column]));
}
