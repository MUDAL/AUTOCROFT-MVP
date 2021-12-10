#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "gpio.h"
#include "i2c.h"
#include "systick.h"
#include "lcd.h"

#define CMD_MODE			0
#define DATA_MODE			1
#define LCD_I2C_ADDR	0x27

enum fourBitCmd
{
	FUNCTION_SET_8BIT = 0x03, 
	FUNCTION_SET_4BIT = 0x02, 
	FUNCTION_SET_2LINE_5X8_DOT = 0x28, 
	CLEAR_DISPLAY = 0x01,
	DISPLAY_ON_CURSOR_ON = 0x0E,
	DISPLAY_ON_CURSOR_OFF = 0x0C,
	ENTRY_MODE_INCREMENT_CURSOR = 0x06	
};

enum PCF8574_Ports
{
	P0,P1,P2,P3,P4,P5,P6,P7
};

/**
@brief LCD pins:
- Register Select (RS)
- Read/Write (RW)
- Enable (EN)
- BackLight Anode (BL)
- Data pins (D4-D7)
*/
enum LCD_Pins
{
	RS = P0, RW = P1, EN = P2, BL = P3,
	D4 = P4, D5 = P5, D6 = P6, D7 = P7
};

enum NibbleBitPosition
{
	LOW_NIBBLE = 0,
	HIGH_NIBBLE = 4
};

static uint8_t i2cData;
const uint8_t ddramAddr[2][16] = {{0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
																	 0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F},
																	{0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,
																	 0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F}};

static void LCD_WriteNibble(char byte,uint8_t nibbleBitPos)
{
	uint8_t nibble = byte >> nibbleBitPos;
	//Send nibble (P4 to P7)
	i2cData &= ~0xF0;
	i2cData |= (nibble << 4);
	//High to low pulse on EN pin
	i2cData |= (1<<EN);
	I2C_Write(I2C1,LCD_I2C_ADDR,i2cData);
	SysTick_DelayMs(1);
	i2cData &= ~(1<<EN);
	I2C_Write(I2C1,LCD_I2C_ADDR,i2cData);
	SysTick_DelayMs(1);
}

static void LCD_WriteByte(uint8_t lcdMode,char byte)
{
	i2cData &= ~(1<<RS);
	i2cData |= (lcdMode<<RS);
	LCD_WriteNibble(byte,HIGH_NIBBLE);
	LCD_WriteNibble(byte,LOW_NIBBLE);
}
	
void LCD_Init(void)
{
	i2cData = 1<<BL; //turn backlight on
	//Initialization sequence (according to HD44780 datasheet)
	SysTick_DelayMs(16); //Power-on delay (must be greater than 15ms for 4.5v and 40ms for 2.7v)
	LCD_WriteByte(CMD_MODE,FUNCTION_SET_8BIT);
	SysTick_DelayMs(5); //wait for more than 4.1ms
	LCD_WriteByte(CMD_MODE,FUNCTION_SET_8BIT);
	SysTick_DelayMs(1); //wait for more than 100us
	//4-bit operation commands
	uint8_t fourBitCommand[5] =
	{
		FUNCTION_SET_4BIT,
		FUNCTION_SET_2LINE_5X8_DOT,
		CLEAR_DISPLAY,
		DISPLAY_ON_CURSOR_OFF,
		ENTRY_MODE_INCREMENT_CURSOR
	};	
	for(uint8_t i = 0; i < 5; i++)
	{
		LCD_WriteByte(CMD_MODE,fourBitCommand[i]);
	}		
}

void LCD_PrintChar(char data)
{
	//Select data register and send data
	LCD_WriteByte(DATA_MODE,data);
}

void LCD_PrintString(char* pData)
{
	while(*pData != '\0')
	{
		LCD_WriteByte(DATA_MODE,*pData);
		pData++;
	}
}

void LCD_Clear(void)
{
	//Select instruction register then clear the screen
	LCD_WriteByte(CMD_MODE,CLEAR_DISPLAY);
}

void LCD_SetCursor(uint8_t row, uint8_t column)
{
	if((row > 1) || (column > 15))
	{
		//Invalid
		return;
	}
	//Select instruction register then set cursor position
	LCD_WriteByte(CMD_MODE, ((1<<7)|ddramAddr[row][column]));
}
