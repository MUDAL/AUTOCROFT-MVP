#ifndef _FSM_H
#define _FSM_H

extern void FSM_Init(ButtonDataStructure* pButton,
										 MasterTxDataStructure* pMasterToNode,
										 MasterRxDataStructure* pNodeToMaster);

extern void FSM_Execute(void);

#endif //FSM_H
