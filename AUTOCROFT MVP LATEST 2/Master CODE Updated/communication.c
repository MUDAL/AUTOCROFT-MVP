#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "conversions.h"
#include "hc12.h"
#include "communication.h"

void Master_EncodeTxData(MasterTxDataStructure* pMasterTx, uint16_t data, dataIndex_t dataIndex)
{
	if(dataIndex > MAX_IRRIG_TIME)
	{//Invalid input
		return;
	}
	if(dataIndex != MIN_IRRIG_TIME && dataIndex != MAX_IRRIG_TIME)
	{
		pMasterTx->data[dataIndex] = data;
	}
	else
	{
		pMasterTx->data[dataIndex] = (data&0xFF00)>>8; //high byte
		pMasterTx->data[dataIndex + 1] = (data&0xFF); //low byte
	}
}

