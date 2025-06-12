/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   pile_eu_state.h
* Description                           :   欧标桩实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-02-22
* notice                                :   
****************************************************************************************************/
#ifndef _pile_eu_state_H_
#define _pile_eu_state_H_

#include "en_common.h"
#include "en_log.h"
#include "en_mem.h"










//充电桩工作状态定义
typedef enum
{
    ePileEuStateAvailable               = 0x01,                                 //可用状态
    ePileEuStatePreparing               = 0x02,                                 //插枪状态
    ePileEuStateCharging                = 0x03,                                 //充电状态
    ePileEuStateSuspendedEv             = 0x04,                                 //车端暂停
    ePileEuStateSuspendedEvse           = 0x05,                                 //桩端暂停
    ePileEuStateFinishing               = 0x06,                                 //充电结束态
    ePileEuStateReserved                = 0x07,                                 //预约状态
    ePileEuStateUnavailable             = 0x08,                                 //不可用状态
    ePileEuStateFaulted                 = 0x09,                                 //故障态
    
    ePileEuStateMax
}ePileEuState_t;










































extern ePileEuState_t sPileEuStateGet(ePileGunIndex_t eGunIndex);
extern bool sPileEuStateSet(ePileGunIndex_t eGunIndex, ePileEuState_t eState);

extern void sPileEuStateTask(void *pParam);







#endif
























