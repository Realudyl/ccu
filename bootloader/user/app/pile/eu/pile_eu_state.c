/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   pile_eu_state.c
* Description                           :   欧标桩实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-02-22
* notice                                :   
****************************************************************************************************/
#include "pile_eu.h"
#include "pile_eu_config.h"
#include "pile_eu_state.h"

#include "bms_ccs.h"
#include "bms_ccs_api.h"
#include "bms_ccs_v2g_din_api.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "pile_eu_state";




extern stPileEuCache_t stPileEuCache;




//ccs状态描述字符串
u8 u8PileEuStateString[ePileEuStateMax][36] = 
{
    "Available-----可用状态",
    "Preparing-----插枪状态",
    "Charging------充电状态",
    "SuspendedEV---车端暂停",
    "SuspendedEVSE-桩端暂停",
    "Finishing-----充电结束态",
    "Reserved------预约状态",
    "Unavailable---不可用状态",
    "Faulted-------故障态",
    "Unknow--------未知态"
};





ePileEuState_t sPileEuStateGet(ePileGunIndex_t eGunIndex);
bool sPileEuStateSet(ePileGunIndex_t eGunIndex, ePileEuState_t eState);

void sPileEuStateTask(void *pParam);

void sPileEuSelfChecking(ePileGunIndex_t eGunIndex);

void sPileEuStateAvailable(ePileGunIndex_t eGunIndex);
void sPileEuStatePreparing(ePileGunIndex_t eGunIndex);
void sPileEuStateCharging(ePileGunIndex_t eGunIndex);
void sPileEuStateSuspendedEv(ePileGunIndex_t eGunIndex);
void sPileEuStateSuspendedEvse(ePileGunIndex_t eGunIndex);
void sPileEuStateFinishing(ePileGunIndex_t eGunIndex);
void sPileEuStateReserved(ePileGunIndex_t eGunIndex);
void sPileEuStateUnavailable(ePileGunIndex_t eGunIndex);
void sPileEuStateFaulted(ePileGunIndex_t eGunIndex);















/***************************************************************************************************
* Description                           :   充电桩 状态获取函数
* Author                                :   Hall
* Creat Date                            :   2024-02-22
* notice                                :   
****************************************************************************************************/
ePileEuState_t sPileEuStateGet(ePileGunIndex_t eGunIndex)
{
    return ((eGunIndex >= ePileGunIndexNum) ? ePileEuStateMax : stPileEuCache.eState[eGunIndex]);
}







/***************************************************************************************************
* Description                           :   充电桩 状态设置函数
* Author                                :   Hall
* Creat Date                            :   2024-02-22
* notice                                :   
****************************************************************************************************/
bool sPileEuStateSet(ePileGunIndex_t eGunIndex, ePileEuState_t eState)
{
    if((eGunIndex >= ePileGunIndexNum) || (eState >= ePileEuStateMax))
    {
        return(false);
    }
    
    if(stPileEuCache.eState[eGunIndex] != eState)
    {
        //有变化才打印和赋值---避免无效的刷屏
        EN_SLOGI(TAG, "充电枪%d, 状态切换:%d, %s", eGunIndex, eState, u8PileEuStateString[eState - 1]);
        stPileEuCache.eState[eGunIndex] = eState;
    }
    
    return(true);
}



























/**********************************************************************************************
* Description                           :   欧标桩 状态机任务
* Author                                :   Hall
* modified Date                         :   2024-02-22
* notice                                :   
***********************************************************************************************/
void sPileEuStateTask(void *pParam)
{
    i32 eGunIndex = (*((i32 *)pParam));
    
    EN_SLOGI(TAG, "任务建立:充电桩状态机管理主任务, 对应枪序号:%d", eGunIndex);
    
    while(1)
    {
        switch(stPileEuCache.eState[eGunIndex])
        {
        case ePileEuStateAvailable:
            sPileEuStateAvailable(eGunIndex);
            break;
        case ePileEuStatePreparing:
            sPileEuStatePreparing(eGunIndex);
            break;
        case ePileEuStateCharging:
            sPileEuStateCharging(eGunIndex);
            break;
        case ePileEuStateSuspendedEv:
            break;
        case ePileEuStateSuspendedEvse:
            sPileEuStateSuspendedEvse(eGunIndex);
            break;
        case ePileEuStateFinishing:
            sPileEuStateFinishing(eGunIndex);
            break;
        case ePileEuStateReserved:
            break;
        case ePileEuStateUnavailable:
            break;
        case ePileEuStateFaulted:
            break;
        default:
            //完成自检并跳转到合适的状态
            sPileEuSelfChecking(eGunIndex);
            break;
        }
        
        vTaskDelay(20 / portTICK_RATE_MS);
    }
    
    vTaskDelete(NULL);
}







/**********************************************************************************************
* Description                           :   欧标桩 上电自检逻辑
* Author                                :   Hall
* modified Date                         :   2024-03-11
* notice                                :   
***********************************************************************************************/
void sPileEuSelfChecking(ePileGunIndex_t eGunIndex)
{
    //暂时没代码 后面在这里增加自检逻辑
    sPileEuStateSet(eGunIndex, ePileEuStateAvailable);
}









/**********************************************************************************************
* Description                           :   欧标桩 状态机管理 之 Available 状态管理
* Author                                :   Hall
* modified Date                         :   2024-02-22
* notice                                :   
***********************************************************************************************/
void sPileEuStateAvailable(ePileGunIndex_t eGunIndex)
{
    i32  i32PlcIndex = eGunIndex - stPileEuCache.eGunIndexBaseCcs;
    
    if(sAdcAppSampleGetCpState(eGunIndex) == eChgCpStateB)
    {
        stPileEuCache.u8CpCnt[eGunIndex]++;
        if(stPileEuCache.u8CpCnt[eGunIndex] >= 5)
        {
            //插枪 
            sBmsCcsSetSlacEnable(i32PlcIndex, 1);
            
            sPileEuStateSet(eGunIndex, ePileEuStatePreparing);                      //A2
        }
        
    }
    else
    {
        stPileEuCache.u8CpCnt[eGunIndex] = 0;
    }
}








/**********************************************************************************************
* Description                           :   欧标桩 状态机管理 之 Preparing 状态管理
* Author                                :   Hall
* modified Date                         :   2024-02-26
* notice                                :   
***********************************************************************************************/
void sPileEuStatePreparing(ePileGunIndex_t eGunIndex)
{
    eV2gDinMsgId_t eId;
    i32  i32PlcIndex = eGunIndex - stPileEuCache.eGunIndexBaseCcs;
    
    if(sAdcAppSampleGetCpState(eGunIndex) == eChgCpStateA)
    {
        //拔枪 
        sBmsCcsSetSlacEnable(i32PlcIndex, 0);
        
        sPileEuStateSet(eGunIndex, ePileEuStateAvailable);                      //B1
    }
    
    if(sV2gDinGet_MsgId(i32PlcIndex, &eId) == true)
    {
        if(eId >= eV2gDinMsgIdCableCheck)
        {
            //开始绝缘检测到闭合接触器 都属于桩端暂停：
            //满足所有充电的先决条件，但EVSE不允许充电---要进行绝缘检测和预充电
            sPileEuStateSet(eGunIndex, ePileEuStateSuspendedEvse);              //B5
        }
    }
}






/**********************************************************************************************
* Description                           :   欧标桩 状态机管理 之 Charging 状态管理
* Author                                :   Hall
* modified Date                         :   2024-03-29
* notice                                :   
***********************************************************************************************/
void sPileEuStateCharging(ePileGunIndex_t eGunIndex)
{
    eBmsCcsState_t eState;
    i32  i32PlcIndex = eGunIndex - stPileEuCache.eGunIndexBaseCcs;
    
    eState = sBmsCcsStateGet(i32PlcIndex);
    if((eState == eBmsCcsStateShutdownNormal)                               //预期中止
    || (eState == eBmsCcsStateShutdownEmg)                                  //紧急停机
    || (eState == eBmsCcsStateAbnormal))                                    //其他异常
    {
        sPileEuStateSet(eGunIndex, ePileEuStateFinishing);                  //C6
    }
}








/**********************************************************************************************
* Description                           :   欧标桩 状态机管理 之 SuspendedEV 状态管理
* Author                                :   Hall
* modified Date                         :   2024-04-01
* notice                                :   
***********************************************************************************************/
void sPileEuStateSuspendedEv(ePileGunIndex_t eGunIndex)
{
}








/**********************************************************************************************
* Description                           :   欧标桩 状态机管理 之 SuspendedEVSE 状态管理
* Author                                :   Hall
* modified Date                         :   2024-03-29
* notice                                :   
***********************************************************************************************/
void sPileEuStateSuspendedEvse(ePileGunIndex_t eGunIndex)
{
    eV2gDinMsgId_t eId;
    eBmsCcsState_t eState;
    i32  i32PlcIndex = eGunIndex - stPileEuCache.eGunIndexBaseCcs;
    
    if(sV2gDinGet_MsgId(i32PlcIndex, &eId) == true)
    {
        if((eId >= eV2gDinMsgIdPowerDelivery1) && (eId <= eV2gDinMsgIdCurrentDemand))
        {
            sPileEuStateSet(eGunIndex, ePileEuStateCharging);                   //E3
        }
        
        eState = sBmsCcsStateGet(i32PlcIndex);
        if((eState == eBmsCcsStateShutdownNormal)                               //预期中止
        || (eState == eBmsCcsStateShutdownEmg)                                  //紧急停机
        || (eState == eBmsCcsStateAbnormal))                                    //其他异常
        {
            sPileEuStateSet(eGunIndex, ePileEuStateFinishing);                  //E6
        }
    }
}







/**********************************************************************************************
* Description                           :   欧标桩 状态机管理 之 Finishing 状态管理
* Author                                :   Hall
* modified Date                         :   2024-04-01
* notice                                :   
***********************************************************************************************/
void sPileEuStateFinishing(ePileGunIndex_t eGunIndex)
{
    i32  i32PlcIndex = eGunIndex - stPileEuCache.eGunIndexBaseCcs;
    
    if(sAdcAppSampleGetCpState(eGunIndex) == eChgCpStateA)
    {
        //拔枪 
        sBmsCcsSetSlacEnable(i32PlcIndex, 0);
        
        sPileEuStateSet(eGunIndex, ePileEuStateAvailable);                      //F1
    }
    
}






void sPileEuStateReserved(ePileGunIndex_t eGunIndex);
void sPileEuStateUnavailable(ePileGunIndex_t eGunIndex);
void sPileEuStateFaulted(ePileGunIndex_t eGunIndex);



































