#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "gpio.h"
#include "sysTimer_struct.h"
#include "systick.h"
#include "button.h"

enum ButtonPress
{
	BUTTON_NOT_PRESSED = false,
	BUTTON_PRESSED = true
};

static void Button_Struct_Init(button_t* pButton, uint8_t pinNumber)
{
	pButton->pin = pinNumber;
	pButton->prevPressed = false;
}

static bool Button_Is_Debounced(button_t* pButton)
{
	GPIO_TypeDef* pGPIO = GPIOA;
	
	switch(pButton->pin)
	{
		case BUTTON_SEND_PIN:
			pGPIO = GPIOA;
			break;
		
		case BUTTON_BACK_PIN:
		case BUTTON_UP_PIN:
		case BUTTON_FORWARD_PIN:
		case BUTTON_DOWN_PIN:
		case BUTTON_ENTER_PIN:
			pGPIO = GPIOB;
			break;

	}
	
	//Since pullups are used, logic 0 means button is pressed.
	if (!GPIO_Input_Read(pGPIO,pButton->pin))
	{
		SysTick_DelayMs(10);
		if (!GPIO_Input_Read(pGPIO,pButton->pin))
		{
			return true;
		}
	}
	
	return false;
	
}

void Button_Init(ButtonDataStructure* pButtonDataStructure)
{
	GPIO_Input_Init(GPIOA,
								 GPIO_PORT_REG_LOW,
								 BUTTON_SEND_PIN,
								 GPIO_PIN0_INPUT_PULLUP_OR_PULLDOWN,
								 true);
	
	GPIO_Input_Init(GPIOB,
									GPIO_PORT_REG_LOW,
									BUTTON_BACK_PIN,
									GPIO_PIN5_INPUT_PULLUP_OR_PULLDOWN,
									true);

	GPIO_Input_Init(GPIOB,
									GPIO_PORT_REG_HIGH,
									BUTTON_UP_PIN,
									GPIO_PIN8_INPUT_PULLUP_OR_PULLDOWN,
									true);

	GPIO_Input_Init(GPIOB,
									GPIO_PORT_REG_HIGH,
									BUTTON_FORWARD_PIN,
									GPIO_PIN9_INPUT_PULLUP_OR_PULLDOWN,
									true);

	GPIO_Input_Init(GPIOB,
									GPIO_PORT_REG_HIGH,
									BUTTON_DOWN_PIN,
									GPIO_PIN10_INPUT_PULLUP_OR_PULLDOWN,
									true);

	GPIO_Input_Init(GPIOB,
									GPIO_PORT_REG_HIGH,
									BUTTON_ENTER_PIN,
									GPIO_PIN11_INPUT_PULLUP_OR_PULLDOWN,
									true);
									
	Button_Struct_Init(&pButtonDataStructure->send, BUTTON_SEND_PIN);
	Button_Struct_Init(&pButtonDataStructure->back, BUTTON_BACK_PIN);
	Button_Struct_Init(&pButtonDataStructure->up, BUTTON_UP_PIN);
	Button_Struct_Init(&pButtonDataStructure->forward, BUTTON_FORWARD_PIN);
	Button_Struct_Init(&pButtonDataStructure->down, BUTTON_DOWN_PIN);
	Button_Struct_Init(&pButtonDataStructure->enter, BUTTON_ENTER_PIN);
	
}

bool Button_Read(button_t* pButton)
{
	if (Button_Is_Debounced(pButton) && !pButton->prevPressed)
	{
		pButton->prevPressed = true;
		return BUTTON_PRESSED;
	}
	
	else if (!Button_Is_Debounced(pButton) && pButton->prevPressed)
	{
		pButton->prevPressed = false;
		return BUTTON_NOT_PRESSED;
	}
	
	return BUTTON_NOT_PRESSED;
	
}
