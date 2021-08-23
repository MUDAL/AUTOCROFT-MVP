#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "conversions.h"
#include "systick.h"
#include "gpio.h"
#include "keypad.h"

#define NUMBER_OF_COLUMNS   4
#define NUMBER_OF_ROWS			4

enum Rows
{
	ROW1 = 1,
	ROW2 = 4,
	ROW3 = 5,
	ROW4 = 6
};

enum Columns
{
	COLUMN1,
	COLUMN2,
	COLUMN3,
	COLUMN4
};

const uint8_t keypadRowPin[4] = {ROW1,ROW2,ROW3,ROW4};
//Keypad matrix
const char keypad[4][4] = {{'1','4','7','*'},
														{'2','5','8','0'},
														{'3','6','9','#'},
														{'A','B','C','D'}};

static bool IsDigit(char ch)
{
	if(ch >= '0' && ch <= '9')
	{
		return true;
	}
	return false;
}

static void ActivateColumn(uint8_t column)
{
	switch(column)
	{
		case COLUMN1:
			//clear PB1 and set PB5,8,9
			GPIO_OutputClearReg(GPIOB,(1<<GPIO_PIN1));
			GPIO_OutputRegWrite(GPIOB,((1<<GPIO_PIN5)|(1<<GPIO_PIN8)|(1<<GPIO_PIN9)));
			break;
		case COLUMN2:
			//clear PB5 and set PB1,8,9
			GPIO_OutputClearReg(GPIOB,(1<<GPIO_PIN5));
			GPIO_OutputRegWrite(GPIOB,((1<<GPIO_PIN1)|(1<<GPIO_PIN8)|(1<<GPIO_PIN9)));
			break;
		case COLUMN3:
			//clear PB8 and set PB1,5,9
			GPIO_OutputClearReg(GPIOB,(1<<GPIO_PIN8));
			GPIO_OutputRegWrite(GPIOB,((1<<GPIO_PIN1)|(1<<GPIO_PIN5)|(1<<GPIO_PIN9)));
			break;
		case COLUMN4:
			//clear PB9 and set PB1,5,8
			GPIO_OutputClearReg(GPIOB,(1<<GPIO_PIN9));
			GPIO_OutputRegWrite(GPIOB,((1<<GPIO_PIN1)|(1<<GPIO_PIN5)|(1<<GPIO_PIN8)));
			break;
	}
}

static void Keypad_StoreKeyPress(char key, char* keyBuffer, uint8_t len)
{
	for(uint8_t i = 0; i<len-1 ;i++)
	{
		keyBuffer[i] = keyBuffer[i+1];
	}
	keyBuffer[len-1] = key;	
}

void Keypad_Init(void)
{
	//Configure PB1,5,8,9, as output
	GPIO_OutputInit(GPIOB,
									GPIO_PORT_REG_LOW,
								 (GPIO_PIN1_OUTPUT_MODE_2MHZ | GPIO_PIN5_OUTPUT_MODE_2MHZ),
									GPIO_GEN_PUR_OUTPUT_PUSH_PULL);
	GPIO_OutputInit(GPIOB,
									GPIO_PORT_REG_HIGH,
								 (GPIO_PIN8_OUTPUT_MODE_2MHZ | GPIO_PIN9_OUTPUT_MODE_2MHZ),
									GPIO_GEN_PUR_OUTPUT_PUSH_PULL);
	
	//Configure PA1,4,5,6 as input with pull-up	
	GPIO_InputInitReg(GPIOA,
										GPIO_PORT_REG_LOW, 
									 ((1<<GPIO_PIN1)|(1<<GPIO_PIN4)|(1<<GPIO_PIN5)|(1<<GPIO_PIN6)),
									 (GPIO_PIN1_INPUT_PULLUP_OR_PULLDOWN | GPIO_PIN4_INPUT_PULLUP_OR_PULLDOWN | 
										GPIO_PIN5_INPUT_PULLUP_OR_PULLDOWN | GPIO_PIN6_INPUT_PULLUP_OR_PULLDOWN),
										GPIO_PULLUP_ENABLE);

	
}

char Keypad_GetChar(void)
{
	char keypadChar = '\0';
	for(uint8_t i = 0; i < NUMBER_OF_COLUMNS; i++)
	{
		ActivateColumn(i);
		for(uint8_t j = 0; j < NUMBER_OF_ROWS; j++)
		{
			/*
			Read keypad key and wait for it to stabilize.  
			*/
			if(!GPIO_InputRead(GPIOA,keypadRowPin[j]))
			{
				SysTick_DelayMs(30);
				keypadChar = keypad[i][j];
			}
		}
	}
	return keypadChar;
}

void Keypad_StrToInt(uint8_t* pInteger, char* pBuffer, uint8_t len)
{
	char numericKey = Keypad_GetChar();
	
	if(IsDigit(numericKey))
	{
		Keypad_StoreKeyPress(numericKey,pBuffer,len);
		*pInteger = Conv_StringToInteger(pBuffer);
	}	
}

