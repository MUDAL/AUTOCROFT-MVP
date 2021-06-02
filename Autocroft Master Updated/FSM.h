#ifndef _FSM_H
#define _FSM_H

extern void FSM_Init(ButtonDataStructure* pButton,
										 MasterMessageDataStructure* pMasterToNode,
										 NodeMessageDataStructure* pNodeToMaster);

extern void FSM_Execute(void);

#endif //FSM_H
