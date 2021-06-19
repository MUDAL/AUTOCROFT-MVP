#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "gpio.h"
#include "systick.h"
#include "button.h"

//Private defines
#define PORTA							0
#define PORTB							1
#define PORTC							2
	
static GPIO_TypeDef* gpioPorts[3] = {GPIOA,GPIOB,GPIOC};

static void Button_Struct_Init(button_t* pButton,
															 uint8_t portIndex, 
															 uint8_t portLevel,  
														   uint8_t pinNumber,
														   uint32_t pullupConfig)
{
	GPIO_InputInit(gpioPorts[portIndex],
								 portLevel,
								 pinNumber,
								 pullupConfig,
								 true);
	
	pButton->portIndex = portIndex;
	pButton->portLevel = portLevel;
	pButton->pin = pinNumber;
	pButton->prevPressed = false;
}

static bool IsPressed(button_t* pButton)
{
	//Since pullups are used, logic 0 means button is pressed.
	if (!GPIO_InputRead(gpioPorts[pButton->portIndex], pButton->pin))
	{
		SysTick_DelayMs(10);
		if (!GPIO_InputRead(gpioPorts[pButton->portIndex], pButton->pin))
		{
			return true;
		}
	}
	return false;
}

void Button_Init(ButtonDataStructure* pButtonDataStructure)
{
	Button_Struct_Init(&pButtonDataStructure->send,
											PORTA,
											GPIO_PORT_REG_LOW,
											BUTTON_SEND_PIN,
											GPIO_PIN0_INPUT_PULLUP_OR_PULLDOWN);
	
	Button_Struct_Init(&pButtonDataStructure->back,
											PORTB,
											GPIO_PORT_REG_LOW,
											BUTTON_BACK_PIN,
											GPIO_PIN5_INPUT_PULLUP_OR_PULLDOWN);
	
	Button_Struct_Init(&pButtonDataStructure->up,											
											PORTB,
											GPIO_PORT_REG_HIGH,
											BUTTON_UP_PIN,
											GPIO_PIN8_INPUT_PULLUP_OR_PULLDOWN);
	
	Button_Struct_Init(&pButtonDataStructure->forward,											
											PORTB,
											GPIO_PORT_REG_HIGH,
											BUTTON_FORWARD_PIN,
											GPIO_PIN9_INPUT_PULLUP_OR_PULLDOWN);
	
//	Button_Struct_Init(&pButtonDataStructure->down,											
//											PORTB,
//											GPIO_PORT_REG_HIGH,
//											BUTTON_DOWN_PIN,
//											GPIO_PIN10_INPUT_PULLUP_OR_PULLDOWN);
//	
//	Button_Struct_Init(&pButtonDataStructure->enter,											
//											PORTB,
//											GPIO_PORT_REG_HIGH,
//											BUTTON_ENTER_PIN,
//											GPIO_PIN11_INPUT_PULLUP_OR_PULLDOWN);
}

/**
@brief scans button for it's current logic level
@param pButton: pointer to object of type button_t
@return  
1.) true: if button is pressed and not previously pressed
2.) false: if the condition above isn't the case
*/
bool Button_Read(button_t* pButton)
{
	if (IsPressed(pButton) && !pButton->prevPressed)
	{
		pButton->prevPressed = true;
		return true;
	}
	
	else if (!IsPressed(pButton) && pButton->prevPressed)
	{
		pButton->prevPressed = false;
	}
	return false;
}
