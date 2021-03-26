#ifndef _FSM_H
#define _FSM_H

#define NUMBER_OF_STATES 					4
#define NUMBER_OF_SUBSTATES				4

enum States
{
	STATE_MOISTURE = 0,
	STATE_HUMIDITY,
	STATE_TEMPERATURE,
	STATE_IRRIGATION_TIME
};

enum Substates
{
	SUBSTATE_HIGHLIGHT_MIN = 0,
	SUBSTATE_HIGHLIGHT_MAX,
	SUBSTATE_SET_MIN,
	SUBSTATE_SET_MAX
};

extern uint8_t FSM_GetState(void);
extern void FSM_SetState(uint8_t state);
extern uint8_t FSM_GetSubstate(void);
extern void FSM_SetSubstate(uint8_t subState);

#endif //FSM_H
