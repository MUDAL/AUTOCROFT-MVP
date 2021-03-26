#include <stdint.h>
#include "FSM.h"

static uint8_t currentState = STATE_MOISTURE;
static uint8_t currentSubstate = SUBSTATE_HIGHLIGHT_MIN;

uint8_t FSM_GetState(void)
{
	return currentState;
}

void FSM_SetState(uint8_t state)
{
	currentState = state;
}

uint8_t FSM_GetSubstate(void)
{
	return currentSubstate;
}

void FSM_SetSubstate(uint8_t subState)
{
	currentSubstate = subState;
}

