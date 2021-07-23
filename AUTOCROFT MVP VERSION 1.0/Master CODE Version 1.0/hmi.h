#ifndef _HMI_H
#define _HMI_H

extern void HMI_Init(uint8_t* pMasterToNode,
										 uint8_t* pNodeToMaster,
										 uint8_t* pNodeToMasterArray);
extern void HMI_Execute(void);

#endif //_HMI_H
