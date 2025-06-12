/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   bms_gbt_15_basic.c
* Description                           :   实现协议的基础功能，例如充电桩参数与BMS参数是否匹配等
* Version                               :   
* Author                                :   haisheng.dang@en-plus.com.cn
* Creat Date                            :   2024-04-17
* notice                                :   
****************************************************************************************************/

#include "bms_gbt_15.h"
#include "bms_gbt_15_basic.h"






//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "bms_gbt_15_basic";







extern stBmsGbt15Cache_t *pBmsGbt15Cache[ePileGunIndexNum];




u8  sBmsGbt15GetPgnCode(u32 u32CanId);
u32 sBmsGbt15GetMsgCanId(u8 u8Pgn);

eBmsGbt15EvParamCheck_t sBmsGbt15EvParamMatchCheck(ePileGunIndex_t eGunIndex);

bool sBmsGbt15CheckBsmFrameIsNormal(ePileGunIndex_t eGunIndex);
eBmsGbt15StopReasonType_t sBmsGbt15CheckBstFrameIsNormal(ePileGunIndex_t eGunIndex);








/***************************************************************************************************
* Description                           :   获取canId中的PGN码，并返回
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-22
* notice                                :   
*                                           
****************************************************************************************************/
u8 sBmsGbt15GetPgnCode(u32 u32CanId)
{
    return ((u32CanId >> 16) & 0xFF);
}







/***************************************************************************************************
* Description                           :   获取发送给bms报文的canId
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-22
* notice                                :   
*                                           
****************************************************************************************************/
u32 sBmsGbt15GetMsgCanId(u8 u8Pgn)
{
    unBmsGbt15CanId_t unCanId;
    
    unCanId.u32Value = 0;
    
    //桩设备地址
    unCanId.stValue.bAddSrc             = eBmsGbt15SeccAddr;
    //车bms地址
    unCanId.stValue.bAddDst             = eBmsGbt15EvccAddr;
    //PGN码
    unCanId.stValue.bPgn                = u8Pgn;
    
    //bit29 数据帧
    unCanId.stValue.ebit29              = eBmsGbt15CanIdBit29DataFrame;
    //bit30 
    unCanId.stValue.ebit30              = eBmsGbt15CanIdBit30Data;
    //bit31 扩展帧
    unCanId.stValue.ebit31              = eBmsGbt15CanIdBit31Extended;
    
    
    switch(u8Pgn)
    {
        case eBmsGbt15PgnChm:
        case eBmsGbt15PgnCrm:
        case eBmsGbt15PgnCts:
        case eBmsGbt15PgnCml:
        case eBmsGbt15PgnCcs:
        case eBmsGbt15PgnCsd:
            unCanId.stValue.bPri        = 6;
            break;
        case eBmsGbt15PgnCro:
        case eBmsGbt15PgnCst:
            unCanId.stValue.bPri        = 4;
            break;
        case eBmsGbt15PgnCem:
            unCanId.stValue.bPri        = 2;
            break;
        case eBmsGbt15PgnCm:
        case eBmsGbt15PgnDt:
            unCanId.stValue.bPri        = 7;
            break;
        default:
            break;
    }
    
    return unCanId.u32Value;
}







/***************************************************************************************************
* Description                           :   检查车辆充电参数与充电桩参数是否匹配
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-26
* notice                                :   
*                                           
****************************************************************************************************/
eBmsGbt15EvParamCheck_t sBmsGbt15EvParamMatchCheck(ePileGunIndex_t eGunIndex)
{
    stBmsGbt15Cache_t *pCache = pBmsGbt15Cache[eGunIndex];
    
    //判断车端最大允许充电电压是否小于充电桩最小输出电压
    if(pCache->stEvccInfo.stBcpData.u16AllowMaxVol < pCache->stSeccInfo.stCmlData.u16MinOutVol)
    {
        EN_SLOGI(TAG, "bmsDrv:枪%d的Bcp最大允许充电电压小于充电桩最小输出电压，停止充电！！！", eGunIndex);
        return eBmsGbt15EvParamCheckFail;
    }
    
    //判断车端最大允许充电电流是否小于充电桩最小输出电流
    if(pCache->stEvccInfo.stBcpData.u16AllowMaxCur < pCache->stSeccInfo.stCmlData.u16MinOutCur)
    {
        EN_SLOGI(TAG, "bmsDrv:枪%d的Bcp最大允许充电电流小于充电桩最小输出电流，停止充电！！！", eGunIndex);
        return eBmsGbt15EvParamCheckFail;
    }
    
    return eBmsGbt15EvParamCheckNormal;
}








/***************************************************************************************************
* Description                           :   检查Bsm报文中电池的参数是否正常
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-24
* notice                                :   
*                                           
****************************************************************************************************/
bool sBmsGbt15CheckBsmFrameIsNormal(ePileGunIndex_t eGunIndex)
{
    stBmsGbt15Bsm_t *pBsm = &pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBsmData;
    
    if((pBsm->stBatFaultCode.bBatVolFault     == 1)
    || (pBsm->stBatFaultCode.bBatVolFault     == 2)
    || (pBsm->stBatFaultCode.bBatSocFault     == 1)
    || (pBsm->stBatFaultCode.bBatSocFault     == 2)
    || (pBsm->stBatFaultCode.bBatCurFault     == 1)
    || (pBsm->stBatFaultCode.bBatTempFault    == 1)
    || (pBsm->stBatFaultCode.bBatIsulstatus   == 1)
    || (pBsm->stBatFaultCode.bBatConnStatus   == 1))
    {
        return false;
    }
    
    return true;
}






/***************************************************************************************************
* Description                           :   检查Bst报文中停止原因参数是否正常
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-25
* notice                                :   
*                                           
****************************************************************************************************/
eBmsGbt15StopReasonType_t sBmsGbt15CheckBstFrameIsNormal(ePileGunIndex_t eGunIndex)
{
    stBmsGbt15Bst_t *pBst = &pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBstData;
    
    if((pBst->unStopReason.stReason.bSocFull       == 1)
    || (pBst->unStopReason.stReason.bBatVolTarget  == 1)
    || (pBst->unStopReason.stReason.bCellVolTarget == 1)
    || (pBst->unStopReason.stReason.bSeccStop      == 1))
    {
        return eBmsGbt15StopReasonNormal;
    }
    else if((pBst->unStopFault.stFault.bInsulFault        == 1)
         || (pBst->unStopFault.stFault.bOutConntFault     == 1)
         || (pBst->unStopFault.stFault.bComponConntFault  == 1)
         || (pBst->unStopFault.stFault.bChgConntFault     == 1)
         || (pBst->unStopFault.stFault.bBatOverTemp       == 1)
         || (pBst->unStopFault.stFault.bHighVolRelay      == 1)
         || (pBst->unStopFault.stFault.bCC2CheckFault     == 1)
         || (pBst->unStopFault.stFault.bOtherFault        == 1))
    {
        return eBmsGbt15StopReasonDanger;
    }
    else if((pBst->unStopErr.stErr.bOverCur     == 1)
         || (pBst->unStopErr.stErr.bVolAbnormal == 1))
    {
        return eBmsGbt15StopReasonDanger;
    }
    
    //经过讨论确认BST报文不再判断停充原因
    return eBmsGbt15StopReasonNormal;
}














