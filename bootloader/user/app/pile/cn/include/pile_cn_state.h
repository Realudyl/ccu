/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   pile_cn_state.h
* Description                           :   国标桩用户程序状态机
* Version                               :   
* Author                                :   haisheng.dang@en-plus.com.cn
* Creat Date                            :   2024-05-20
* notice                                :   
****************************************************************************************************/
#ifndef _pile_cn_state_h_
#define _pile_cn_state_h_

#include "en_common.h"
#include "en_log.h"































extern ePrivDrvGunWorkStatus_t sPileCnStateGet(ePileGunIndex_t eGunIndex);
extern bool sPileCnStateSet(ePileGunIndex_t eGunIndex, ePrivDrvGunWorkStatus_t eState);

extern void sPileCnStateTask(void *pParam);









#endif

