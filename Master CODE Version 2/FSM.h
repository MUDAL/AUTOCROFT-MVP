#ifndef _FSM_H
#define _FSM_H

#define NUMBER_OF_STATES 					6

enum States
{
	STATE_DISPLAY_READINGS = 0,
	STATE_NODE,
	STATE_MOISTURE,
	STATE_HUMIDITY,
	STATE_TEMPERATURE,
	STATE_IRRIGATION_TIME
};

enum Substates
{
	SUBSTATE_NIL = 0,
	SUBSTATE_HIGHLIGHT_NODE_ID,
	SUBSTATE_SET_NODE_ID,
	SUBSTATE_HIGHLIGHT_MIN,
	SUBSTATE_HIGHLIGHT_MAX,
	SUBSTATE_SET_MIN,
	SUBSTATE_SET_MAX
};

extern uint8_t FSM_GetState(void);
extern void FSM_SetState(uint8_t state);
extern uint8_t FSM_GetSubstate(void);
extern void FSM_SetSubstate(uint8_t subState);

#endif //FSM_H
