#ifndef _BUTTON_H
#define _BUTTON_H

#define BUTTON_SEND_PIN										0
#define BUTTON_BACK_PIN										5
#define BUTTON_UP_PIN											8
#define BUTTON_FORWARD_PIN								9
#define BUTTON_DOWN_PIN										10
#define BUTTON_ENTER_PIN									11

typedef struct
{
	uint8_t portIndex;
	uint8_t portLevel;
	uint8_t pin;
	bool prevPressed;
}button_t;

typedef struct
{
	button_t send;
	button_t back;
	button_t up;
	button_t forward;
	button_t down;
	button_t enter;
}ButtonDataStructure;

extern void Button_Init(ButtonDataStructure* pButtonDataStructure);
extern bool Button_Read(button_t* pButton);

#endif //_BUTTON_H
