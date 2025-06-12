/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   fault.c
* Description                           :   故障告警码系统实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-06-11
* notice                                :   
****************************************************************************************************/
#ifndef _fault_H_
#define _fault_H_

#include "en_common.h"
#include "private_drv_opt.h"





























extern void sFaultInit(void);

//故障告警码增减api
extern bool sFaultCodeAdd(u8 u8GunId, ePrivDrvFaultCode_t eCode);
extern bool sFaultCodeSub(u8 u8GunId, ePrivDrvFaultCode_t eCode);
extern bool sFaultCodeGet(u8 u8GunId, stPrivDrvFaultCode_t *pFault);

extern bool sFaultCodeChkNum (u8 u8GunId);
extern bool sFaultCodeChkCode(u8 u8GunId, ePrivDrvFaultCode_t eCode);














#endif






























