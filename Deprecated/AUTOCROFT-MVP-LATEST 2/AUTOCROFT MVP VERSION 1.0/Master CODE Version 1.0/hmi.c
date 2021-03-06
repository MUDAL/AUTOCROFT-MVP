#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "keypad.h"
#include "lcd.h"
#include "display.h"
#include "bme280.h"
#include "hc12.h"
#include "communication.h"
#include "hmi.h"

/**
@Description The HMI(Human Machine Interface) application
handles user inputs, configuration of parameters, LCD outputs
and screen transitions. It is based on a Finite State Machine (FSM)
design.
*/

//Private globals
static uint8_t* ptrMasterToNode;
static uint8_t* ptrNodeToMaster;
static uint8_t* ptrNodeToMasterArray;
static uint8_t currentState = STATE_DISPLAY_READINGS;
static uint8_t currentSubstate = SUBSTATE_NIL;

//Function definitions
/**
@brief Resets master-to-node data which is stored using the  
'uint8_t' specifier. e.g. node ID, minimum moisture, maximum humidity etc.  
@param pData: pointer to data to be reset.  
@param pKeypadBuffer: A keypad buffer contains the string equivalent of a  
particular master-to-node data. It is also cleared by this function.  
@param keypadBufferLen: length of the keypad buffer.  
@param currSubstate: current substate of the FSM.  
@return None
*/
static void Reset_MasterToNode8bitData(uint8_t* pData,
																			 char* pKeypadBuffer,
																			 uint8_t keypadBufferLen,
																			 uint8_t currSubstate)
{
	if(currentSubstate == currSubstate)
	{
		memset(pKeypadBuffer,'0',keypadBufferLen);
		*pData = 0;
	}
}

/**
@brief Resets master-to-node data which is stored using the  
'uint16_t' specifier. This applies only to the minimum and maximum  
irrigation time settings.  
@param pData: pointer to data to be reset.  
@param pKeypadBuffer: A keypad buffer contains the string equivalent of a  
particular master-to-node data. It is also cleared by this function.  
@param keypadBufferLen: length of the keypad buffer.  
@param currSubstate: current substate of the FSM.  
@return None
*/
static void Reset_MasterToNode16bitData(uint16_t* pData,
																				char* pKeypadBuffer,
																				uint8_t keypadBufferLen,
																				uint8_t currSubstate)
{
	if(currentSubstate == currSubstate)
	{
		memset(pKeypadBuffer,'0',keypadBufferLen);
		*pData = 0;
	}
}

/**
@brief Initiates a state transition based on the pressing of a key on the  
keypad.
@param key: When the pressed key on the keypad matches the argument passed  
to this parameter, the appropriate state transition occurs.  
@param state: When a state transition occurs, the next state the FSM would be  
in would be the argument passed to this parameter.  
@param substate: When a state transition occurs, the next sub-state the FSM would be  
in would be the argument passed to this parameter.
@return None.  
*/
static void FSM_StateTransition(char key, uint8_t state, uint8_t substate)
{
	//Refresh LCD screen
	char keypadChar = Keypad_GetChar();
	if(keypadChar == key)
	{
		LCD_Clear(); 
		currentState = state;
		currentSubstate = substate;
	}
}

static void FSM_DisplayBme280Data(uint8_t substate)
{
	bme280_t bme280Data;
	BME280_GetData(&bme280Data);
	
	Display_Bme280Data("Hum: ",
										 "Temp: ",
										 bme280Data.humidity,
										 bme280Data.temperature,
										 '%',
										 'C');
	FSM_StateTransition('A',STATE_NODE, SUBSTATE_HIGHLIGHT_NODE_ID);		
}

static void FSM_Node(uint8_t substate)
{
	static uint8_t nodeID;
	static char keyBuffer[3] = "00";
	const uint8_t keyBufferLen = 2;
	uint8_t nodeMoisture = ptrNodeToMasterArray[nodeID];
	
	Display_NodeData("Node ID: ",
									 "Moisture: ",
									 substate,
									 nodeID,
									 nodeMoisture);

	switch(substate)
	{
		case SUBSTATE_HIGHLIGHT_NODE_ID:
			FSM_StateTransition('B',STATE_DISPLAY_READINGS, SUBSTATE_NIL);	
			FSM_StateTransition('A',STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition('#',STATE_NODE, SUBSTATE_SET_NODE_ID);
			Reset_MasterToNode8bitData(&nodeID,keyBuffer,keyBufferLen,SUBSTATE_SET_NODE_ID);
			break;
			
		case SUBSTATE_SET_NODE_ID:
			Keypad_StrTo8bitInt(&nodeID,keyBuffer,keyBufferLen);
		  FSM_StateTransition('#',STATE_NODE, SUBSTATE_HIGHLIGHT_NODE_ID);
			Master_EncodeTxData(ptrMasterToNode, nodeID, NODE_ID);
			break;
	}	
}

static void FSM_Moisture(uint8_t substate)
{
	static uint8_t minMoist;
	static uint8_t maxMoist;
	static char keyBuffer[3] = "00";
	const uint8_t keyBufferLen = 2;
	
	Display_Threshold(minMoist,
									  maxMoist,
									  substate,
									  "MoistMin:",
									  "MoistMax:",
									  '%');
	switch(substate)
	{
		case SUBSTATE_HIGHLIGHT_MIN:
			FSM_StateTransition('B',STATE_NODE, SUBSTATE_HIGHLIGHT_NODE_ID);
			FSM_StateTransition('D',STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MAX);
			FSM_StateTransition('A',STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition('#',STATE_MOISTURE, SUBSTATE_SET_MIN);
			Reset_MasterToNode8bitData(&minMoist,keyBuffer,keyBufferLen,SUBSTATE_SET_MIN);
			break;
		
		case SUBSTATE_HIGHLIGHT_MAX:
			FSM_StateTransition('B',STATE_NODE, SUBSTATE_HIGHLIGHT_NODE_ID);
			FSM_StateTransition('C',STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition('A',STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition('#',STATE_MOISTURE, SUBSTATE_SET_MAX);
			Reset_MasterToNode8bitData(&maxMoist,keyBuffer,keyBufferLen,SUBSTATE_SET_MAX);
			break;
		
		case SUBSTATE_SET_MIN:
			Keypad_StrTo8bitInt(&minMoist,keyBuffer,keyBufferLen);
			FSM_StateTransition('#',STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MIN);
			Master_EncodeTxData(ptrMasterToNode, minMoist, MIN_MOISTURE);
			break;
		
		case SUBSTATE_SET_MAX:
			Keypad_StrTo8bitInt(&maxMoist,keyBuffer,keyBufferLen);
			FSM_StateTransition('#',STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MAX);
			Master_EncodeTxData(ptrMasterToNode, maxMoist, MAX_MOISTURE);
			break;
	}
}
	
static void FSM_Humidity(uint8_t substate)
{
	static uint8_t minHum;
	static uint8_t maxHum;
	static char keyBuffer[3] = "00";
	const uint8_t keyBufferLen = 2;
	
	Display_Threshold(minHum,
									  maxHum,
									  substate,
									  "HumMin:",
									  "HumMax:",
									  '%');
	switch(substate)
	{
		case SUBSTATE_HIGHLIGHT_MIN:
			FSM_StateTransition('D',STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MAX);
			FSM_StateTransition('A',STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition('#',STATE_HUMIDITY, SUBSTATE_SET_MIN);
			FSM_StateTransition('B',STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MIN);
			Reset_MasterToNode8bitData(&minHum,keyBuffer,keyBufferLen,SUBSTATE_SET_MIN);
			break;
		
		case SUBSTATE_HIGHLIGHT_MAX:
			FSM_StateTransition('C',STATE_HUMIDITY,SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition('A',STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition('#',STATE_HUMIDITY, SUBSTATE_SET_MAX);
			FSM_StateTransition('B',STATE_MOISTURE, SUBSTATE_HIGHLIGHT_MIN);
			Reset_MasterToNode8bitData(&maxHum,keyBuffer,keyBufferLen,SUBSTATE_SET_MAX);
			break;
		
		case SUBSTATE_SET_MIN:
			Keypad_StrTo8bitInt(&minHum,keyBuffer,keyBufferLen);
			FSM_StateTransition('#',STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MIN);
			Master_EncodeTxData(ptrMasterToNode, minHum, MIN_HUMIDITY);
			break;
		
		case SUBSTATE_SET_MAX:
			Keypad_StrTo8bitInt(&maxHum,keyBuffer,keyBufferLen);
			FSM_StateTransition('#',STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MAX);
			Master_EncodeTxData(ptrMasterToNode, maxHum, MAX_HUMIDITY);
			break;
	}
}


static void FSM_Temperature(uint8_t substate)
{	
	static uint8_t minTemp;
	static uint8_t maxTemp;
	static char keyBuffer[3] = "00";
	const uint8_t keyBufferLen = 2;
	
	Display_Threshold(minTemp,
									  maxTemp,
									  substate,
									  "TempMin:",
									  "TempMax:",
									  'C');
	switch(substate)
	{
		case SUBSTATE_HIGHLIGHT_MIN:
			FSM_StateTransition('D',STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MAX);
			FSM_StateTransition('A',STATE_IRRIGATION_TIME, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition('#',STATE_TEMPERATURE, SUBSTATE_SET_MIN);
			FSM_StateTransition('B',STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MIN);
			Reset_MasterToNode8bitData(&minTemp,keyBuffer,keyBufferLen,SUBSTATE_SET_MIN);
			break;
		
		case SUBSTATE_HIGHLIGHT_MAX:
			FSM_StateTransition('C',STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition('A',STATE_IRRIGATION_TIME, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition('#',STATE_TEMPERATURE, SUBSTATE_SET_MAX);
			FSM_StateTransition('B',STATE_HUMIDITY, SUBSTATE_HIGHLIGHT_MIN);
			Reset_MasterToNode8bitData(&maxTemp,keyBuffer,keyBufferLen,SUBSTATE_SET_MAX);
			break;
		
		case SUBSTATE_SET_MIN:
			Keypad_StrTo8bitInt(&minTemp,keyBuffer,keyBufferLen);
			FSM_StateTransition('#',STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MIN);
			Master_EncodeTxData(ptrMasterToNode, minTemp, MIN_TEMPERATURE);
			break;
		
		case SUBSTATE_SET_MAX:
			Keypad_StrTo8bitInt(&maxTemp,keyBuffer,keyBufferLen);
			FSM_StateTransition('#', STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MAX);	
			Master_EncodeTxData(ptrMasterToNode, maxTemp, MAX_TEMPERATURE);
			break;
	}
}
	
static void FSM_IrrigTime(uint8_t substate)
{
	static uint16_t minIrrigTime;
	static uint16_t maxIrrigTime;
	static char keyBuffer[5] = "0000";
	const uint8_t keyBufferLen = 4;
	
	Display_Threshold(minIrrigTime,
									  maxIrrigTime,
									  substate,
									  "TimeMin:",
									  "TimeMax:",
									  's');	
	switch(substate)
	{
		case SUBSTATE_HIGHLIGHT_MIN:
			FSM_StateTransition('D', STATE_IRRIGATION_TIME, SUBSTATE_HIGHLIGHT_MAX);
	    FSM_StateTransition('#', STATE_IRRIGATION_TIME, SUBSTATE_SET_MIN);
	    FSM_StateTransition('B', STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MIN);
			Reset_MasterToNode16bitData(&minIrrigTime,keyBuffer,keyBufferLen,SUBSTATE_SET_MIN);
			break;
		
		case SUBSTATE_HIGHLIGHT_MAX:
			FSM_StateTransition('C', STATE_IRRIGATION_TIME, SUBSTATE_HIGHLIGHT_MIN);
			FSM_StateTransition('#', STATE_IRRIGATION_TIME, SUBSTATE_SET_MAX);
			FSM_StateTransition('B', STATE_TEMPERATURE, SUBSTATE_HIGHLIGHT_MIN);
			Reset_MasterToNode16bitData(&maxIrrigTime,keyBuffer,keyBufferLen,SUBSTATE_SET_MAX);
			break;
		
		case SUBSTATE_SET_MIN:
			Keypad_StrTo16bitInt(&minIrrigTime,keyBuffer,keyBufferLen);
			FSM_StateTransition('#', STATE_IRRIGATION_TIME, SUBSTATE_HIGHLIGHT_MIN);	
			Master_EncodeTxData(ptrMasterToNode, minIrrigTime, MIN_IRRIG_TIME);
			break;
		
		case SUBSTATE_SET_MAX:
			Keypad_StrTo16bitInt(&maxIrrigTime,keyBuffer,keyBufferLen);
			FSM_StateTransition('#', STATE_IRRIGATION_TIME, SUBSTATE_HIGHLIGHT_MAX);	
			Master_EncodeTxData(ptrMasterToNode, maxIrrigTime, MAX_IRRIG_TIME);
			break;
	}
}

/**
@brief Initializes the HMI.  
The HMI runs an FSM program and for the FSM to work, it needs  
the three parameters of the HMI_Init() function.  
@param pMasterToNode: pointer to master-to-node data.  
@param pNodeToMaster: pointer to node-to-master data (from single node).  
@param pNodeToMasterArray: pointer to node-to-master data (multiple nodes).  
@return None
*/
void HMI_Init(uint8_t* pMasterToNode,
							uint8_t* pNodeToMaster,
							uint8_t* pNodeToMasterArray)
{
	ptrMasterToNode = pMasterToNode;
	ptrNodeToMaster = pNodeToMaster;
	ptrNodeToMasterArray = pNodeToMasterArray;
}

/**
@brief Runs the HMI application.  
@param None
@return None
*/
void HMI_Execute(void)
{
	static bme280_t bme280Data;
	//Array of FSM function pointers
	static void (*pStateFunction[NUMBER_OF_STATES])(uint8_t substate) = 
	{
		FSM_DisplayBme280Data,
		FSM_Node,
		FSM_Moisture,
		FSM_Humidity,
		FSM_Temperature,
		FSM_IrrigTime
	};
	//Core of the HMI
	pStateFunction[currentState](currentSubstate);
	//If '*' key on keypad is pressed, send data to the nodes
	char keyPadChar = Keypad_GetChar();
	
	if(keyPadChar == '*')
	{
		BME280_GetData(&bme280Data);
		Master_EncodeTxData(ptrMasterToNode,bme280Data.humidity,HUMIDITY);
		Master_EncodeTxData(ptrMasterToNode,bme280Data.temperature,TEMPERATURE);
		LCD_Clear();
		LCD_WriteString("Collecting data");
		LCD_SetCursor(1,0);
		LCD_WriteString("Please wait");
		//Send data to all nodes and receive their moisture readings
		Master_TransmitReceive(ptrMasterToNode,
													 MASTER_TX_DATA_SIZE,
													 ptrNodeToMaster,
													 ptrNodeToMasterArray,
													 NO_OF_NODES,
													 150);
		LCD_Clear();
	}
	
}
