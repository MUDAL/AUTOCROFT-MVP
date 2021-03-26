#ifndef _BUTTON_H
#define _BUTTON_H

#define BUTTON_DEFAULT_PULL_UP_STATE			true
#define BUTTON_DEFAULT_PULL_DOWN_STATE		false

#define BUTTON_SEND_PIN										0
#define BUTTON_BACK_PIN										5
#define BUTTON_UP_PIN											8
#define BUTTON_FORWARD_PIN								9
#define BUTTON_DOWN_PIN										10
#define BUTTON_ENTER_PIN									11

typedef struct
{
	
	uint8_t pin;
	bool prevPressed;
	
}button_t;

extern void Button_Init(button_t* pButton, uint8_t pinNumber);
extern bool Button_Read(button_t* pButton);

#endif //_BUTTON_H
