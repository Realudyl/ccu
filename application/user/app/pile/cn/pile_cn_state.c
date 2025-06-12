/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   pile_cn_state.c
* Description                           :   国标桩用户程序状态机
* Version                               :   
* Author                                :   haisheng.dang@en-plus.com.cn
* Creat Date                            :   2024-05-20
* notice                                :   
****************************************************************************************************/
#include "pile_cn.h"



//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "pile_cn_state";














extern stPileCnCache_t *pPileCnCache;




//状态描述字符串
u8 u8PileCnStateString[ePrivDrvGunWorkStatusMax + 1][36] = 
{
    "None----------无效状态",
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















ePrivDrvGunWorkStatus_t sPileCnStateGet(ePileGunIndex_t eGunIndex);
bool sPileCnStateSet(ePileGunIndex_t eGunIndex, ePrivDrvGunWorkStatus_t eState);

void sPileCnStateTask(void *pParam);

void sPileCnSelfChecking(ePileGunIndex_t eGunIndex);

void sPileCnStateAvailable(ePileGunIndex_t eGunIndex);
void sPileCnStatePreparing(ePileGunIndex_t eGunIndex);
void sPileCnStateCharging(ePileGunIndex_t eGunIndex);
void sPileCnStateSuspendedEv(ePileGunIndex_t eGunIndex);
void sPileCnStateSuspendedEvse(ePileGunIndex_t eGunIndex);
void sPileCnStateFinishing(ePileGunIndex_t eGunIndex);
void sPileCnStateReserved(ePileGunIndex_t eGunIndex);
void sPileCnStateUnavailable(ePileGunIndex_t eGunIndex);
void sPileCnStateFaulted(ePileGunIndex_t eGunIndex);
































/***************************************************************************************************
* Description                           :   充电桩 状态获取函数
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-20
* notice                                :   
****************************************************************************************************/
ePrivDrvGunWorkStatus_t sPileCnStateGet(ePileGunIndex_t eGunIndex)
{
    return ((eGunIndex >= ePileGunIndexNum) ? ePrivDrvGunWorkStatusMax : pPileCnCache->stChgInfo[eGunIndex].eState);
}







/***************************************************************************************************
* Description                           :   充电桩 状态设置函数
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-20
* notice                                :   
****************************************************************************************************/
bool sPileCnStateSet(ePileGunIndex_t eGunIndex, ePrivDrvGunWorkStatus_t eState)
{
    if((eGunIndex >= ePileGunIndexNum) || (eState >= ePrivDrvGunWorkStatusMax))
    {
        return(false);
    }
    
    if(pPileCnCache->stChgInfo[eGunIndex].eState != eState)
    {
        sPileCnGateSetGunStsReason(eGunIndex);
        
        //有变化才打印和赋值---避免无效的刷屏
        EN_SLOGI(TAG, "充电枪%d, 状态切换:%d, %s", eGunIndex, eState, u8PileCnStateString[eState]);
        pPileCnCache->stChgInfo[eGunIndex].eState = eState;
    }
    
    return(true);
}



























/**********************************************************************************************
* Description                           :   国标桩 状态机任务
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-20
* notice                                :   
***********************************************************************************************/
void sPileCnStateTask(void *pParam)
{
    i32 eGunIndex = (*((i32 *)pParam));
    
    EN_SLOGI(TAG, "任务建立:充电桩状态机管理主任务, 对应枪序号:%d", eGunIndex);
    
    while(1)
    {
        switch(pPileCnCache->stChgInfo[eGunIndex].eState)
        {
        case ePrivDrvGunWorkStatusAvailable:
            sLedAppSet(eGunIndex, eLedColorGreen, eLedSignalTypeLight, cLedUpdateTimeLight);
            sPileCnStateAvailable(eGunIndex);
            break;
        case ePrivDrvGunWorkStatusPreparing:
            sLedAppSet(eGunIndex, eLedColorBlue, eLedSignalTypeBlink, cLedUpdateTimeBlink);
            sPileCnStatePreparing(eGunIndex);
            break;
        case ePrivDrvGunWorkStatusCharging:
            sLedAppSet(eGunIndex, eLedColorBlue, eLedSignalTypeBreath, cLedUpdateTimeBreath);
            sPileCnStateCharging(eGunIndex);
            break;
        case ePrivDrvGunWorkStatusSuspendedByEv:
            sLedAppSet(eGunIndex, eLedColorBlue, eLedSignalTypeBlink, cLedUpdateTimeBlink);
            sPileCnStateSuspendedEv(eGunIndex);
            break;
        case ePrivDrvGunWorkStatusSuspendedByEvse:
            sLedAppSet(eGunIndex, eLedColorBlue, eLedSignalTypeBlink, cLedUpdateTimeBlink);
            sPileCnStateSuspendedEvse(eGunIndex);
            break;
        case ePrivDrvGunWorkStatusFinishing:
            sLedAppSet(eGunIndex, eLedColorBlue, eLedSignalTypeLight, cLedUpdateTimeLight);
            sPileCnStateFinishing(eGunIndex);
            break;
        case ePrivDrvGunWorkStatusReserved:
            sLedAppSet(eGunIndex, eLedColorBlue, eLedSignalTypeBlink, cLedUpdateTimeBlink);
            sPileCnStateReserved(eGunIndex);
            break;
        case ePrivDrvGunWorkStatusUnavailable:
            sLedAppSet(eGunIndex, eLedColorYellow, eLedSignalTypeLight, cLedUpdateTimeLight);
            sPileCnStateUnavailable(eGunIndex);
            break;
        case ePrivDrvGunWorkStatusFaulted:
            sLedAppSet(eGunIndex, eLedColorRed, eLedSignalTypeLight, cLedUpdateTimeLight);
            sPileCnStateFaulted(eGunIndex);
            break;
        default:
            //上电自检
            sPileCnSelfChecking(eGunIndex);
            break;
        }
        
        vTaskDelay(20 / portTICK_RATE_MS);
    }
    
    vTaskDelete(NULL);
}







/**********************************************************************************************
* Description                           :   国标桩 上电自检逻辑
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-20
* notice                                :   
***********************************************************************************************/
void sPileCnSelfChecking(ePileGunIndex_t eGunIndex)
{
    bool bRst;
    
    //等待自检完成
    if(pPileCnCache != NULL)
    {
        bRst = true;
        bRst = bRst & sPileCnCheckELockSt(eGunIndex);
        
        if(bRst == true)
        {
            pPileCnCache->stChgInfo[eGunIndex].eCheckResult = eCheckRstNormal;
            sPileCnStateSet(eGunIndex, ePrivDrvGunWorkStatusAvailable);
        }
    }
}









/**********************************************************************************************
* Description                           :   国标桩 状态机管理 之 Available 状态管理
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-20
* notice                                :   
***********************************************************************************************/
void sPileCnStateAvailable(ePileGunIndex_t eGunIndex)
{
    stPileCnGateData_t *pData = &pPileCnCache->stGateData;
    
    /*************待机可用*************/
    
    //情况1：产生停止充电故障，跳转到故障态
    //情况2：检测到完全插枪，跳转到就绪态
    
    if(sOrderGetStep(eGunIndex) == eOrderStepRun)
    {
        //停止计量
        if(sFaultCodeChkCode(eGunIndex, ePrivDrvFaultCode_ccuELockErr) == true)
        {
            sPileCnGateMakeStopReason(eGunIndex, ePrivDrvStopReasonGunLockAbnormal);
        }
        else
        {
            sPileCnGateClrStopReason(eGunIndex);
        }
        sOrderStop(eGunIndex, pData->eStopReason[eGunIndex]);
        sPileCnGateClrStopReason(eGunIndex);                                    //用完以后给默认值
    }
    
    if((sFaultCodeChkNum(cPrivDrvGunIdChg) != 0) || (sFaultCodeChkNum(sPrivDrvGetGunId(eGunIndex)) != 0))
    {
        sPileCnGateMakeGunStsReason(eGunIndex, ePrivDrvGunWorkStsRsnFault);
        sPileCnStateSet(eGunIndex, ePrivDrvGunWorkStatusFaulted);
    }
    else if(sAdcAppSampleGetCc1State(eGunIndex) == eAdcAppCc1StateU1c)
    {
        //待机
        sPileCnGateMakeGunStsReason(eGunIndex, ePrivDrvGunWorkStsRsnPlugIn);
        sPileCnStateSet(eGunIndex, ePrivDrvGunWorkStatusPreparing);
    }
}








/**********************************************************************************************
* Description                           :   国标桩 状态机管理 之 Preparing 状态管理
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-20
* notice                                :   
***********************************************************************************************/
void sPileCnStatePreparing(ePileGunIndex_t eGunIndex)
{
    /*************插枪就绪*************/
    
    //情况1：产生停止充电故障，跳转到故障态
    //情况2：检测到拔枪操作，跳转到待机态
    //情况3：绝缘检测开始，跳转到桩端暂停态
    
    if((sFaultCodeChkNum(cPrivDrvGunIdChg) != 0) || (sFaultCodeChkNum(sPrivDrvGetGunId(eGunIndex)) != 0))
    {
        sPileCnGateMakeGunStsReason(eGunIndex, ePrivDrvGunWorkStsRsnFault);
        sPileCnStateSet(eGunIndex, ePrivDrvGunWorkStatusFaulted);
    }
    else if(sAdcAppSampleGetCc1State(eGunIndex) != eAdcAppCc1StateU1c)
    {
        sPileCnGateMakeGunStsReason(eGunIndex, ePrivDrvGunWorkStsRsnPlugOut);
        sPileCnStateSet(eGunIndex, ePrivDrvGunWorkStatusAvailable);
    }
    else if((sBmsGbt15StateGet(eGunIndex) >= eBmsGbt15StateCableCheck) && (sBmsGbt15StateGet(eGunIndex) != eBmsGbt15StateFaultAB))
    {
        //开始绝缘检测到闭合接触器 都属于桩端暂停：
        //满足所有充电的先决条件，但EVSE不允许充电---要进行绝缘检测和预充电
        sPileCnStateSet(eGunIndex, ePrivDrvGunWorkStatusSuspendedByEvse);
    }
}






/**********************************************************************************************
* Description                           :   国标桩 状态机管理 之 Charging 状态管理
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-20
* notice                                :   
***********************************************************************************************/
void sPileCnStateCharging(ePileGunIndex_t eGunIndex)
{
    /*************正常充电*************/
    
    //情况1：产生停止充电故障，跳转到故障态
    //情况2：车端暂停，跳转到车端暂停态
    //情况3：车端停止充电，跳转到结束态
    //情况4：车端故障，跳转到故障态
    //情况5：通讯超时，跳转到桩端暂停
    
#if defined(DC480K_G2SF311_600)
    //液冷机启动
    sCoolingAppSetPowerOnOff(eRemoteCtrlStart);
#endif
    
    if((sFaultCodeChkNum(cPrivDrvGunIdChg) != 0) || (sFaultCodeChkNum(sPrivDrvGetGunId(eGunIndex)) != 0))
    {
        sBmsGbt15Set_CstCode(eGunIndex, eReasonFaulted, eFaultOther, eErrNone);
        sPileCnGateMakeGunStsReason(eGunIndex, ePrivDrvGunWorkStsRsnFault);
        sPileCnStateSet(eGunIndex, ePrivDrvGunWorkStatusFaulted);
    }
    else if(sBmsGbt15Get_EvPauseChargeState(eGunIndex) == true)
    {
        sPileCnStateSet(eGunIndex, ePrivDrvGunWorkStatusSuspendedByEv);
    }
    else if(sBmsGbt15StateGet(eGunIndex) == eBmsGbt15StateFinish)
    {
        sPileCnStateSet(eGunIndex, ePrivDrvGunWorkStatusFinishing);
        sPileCnSet_Authorize(eGunIndex, eAuthorizeTypeNone);
    }
    else if(sBmsGbt15StateGet(eGunIndex) == eBmsGbt15StateFaultAB)
    {
        sPileCnGateMakeGunStsReason(eGunIndex, ePrivDrvGunWorkStsRsnFault);
        sPileCnStateSet(eGunIndex, ePrivDrvGunWorkStatusFaulted);
    }
    else if(sBmsGbt15StateGet(eGunIndex) == eBmsGbt15StateFaultC)
    {
        //通讯超时认为是桩端暂停，等待重新握手
        sPileCnStateSet(eGunIndex, ePrivDrvGunWorkStatusSuspendedByEvse);
    }
    
}








/**********************************************************************************************
* Description                           :   国标桩 状态机管理 之 SuspendedEV 状态管理
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-20
* notice                                :   
***********************************************************************************************/
void sPileCnStateSuspendedEv(ePileGunIndex_t eGunIndex)
{
    /*************车端暂停*************/
    
    //情况1：产生停止充电故障，跳转到故障态
    //情况2：车端暂停恢复，跳转到充电中态
    //情况3：车端暂停充电期间车端主动停止充电，跳转到结束态
    //情况4：车端暂停充电超时，跳转到故障态
    //情况5：车端暂停期间，通讯超时，跳转到桩端暂停态
    
    if((sFaultCodeChkNum(cPrivDrvGunIdChg) != 0) || (sFaultCodeChkNum(sPrivDrvGetGunId(eGunIndex)) != 0))
    {
        sPileCnGateMakeGunStsReason(eGunIndex, ePrivDrvGunWorkStsRsnFault);
        sPileCnStateSet(eGunIndex, ePrivDrvGunWorkStatusFaulted);
    }
    else if(sBmsGbt15Get_EvPauseChargeState(eGunIndex) == false)
    {
        //车端暂停是否恢复
        sPileCnStateSet(eGunIndex, ePrivDrvGunWorkStatusCharging);
    }
    else if(sBmsGbt15StateGet(eGunIndex) == eBmsGbt15StateFinish)
    {
        sPileCnStateSet(eGunIndex, ePrivDrvGunWorkStatusFinishing);
        sPileCnSet_Authorize(eGunIndex, eAuthorizeTypeNone);
    }
    else if(sBmsGbt15StateGet(eGunIndex) == eBmsGbt15StateFaultAB)
    {
        sPileCnGateMakeGunStsReason(eGunIndex, ePrivDrvGunWorkStsRsnFault);
        sPileCnStateSet(eGunIndex, ePrivDrvGunWorkStatusFaulted);
    }
    else if(sBmsGbt15StateGet(eGunIndex) == eBmsGbt15StateFaultC)
    {
        //通讯超时认为是桩端暂停，等待重新握手
        sPileCnStateSet(eGunIndex, ePrivDrvGunWorkStatusSuspendedByEvse);
    }
}








/**********************************************************************************************
* Description                           :   国标桩 状态机管理 之 SuspendedEVSE 状态管理
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-20
* notice                                :   
***********************************************************************************************/
void sPileCnStateSuspendedEvse(ePileGunIndex_t eGunIndex)
{
    /*************桩端暂停*************/
    
    //情况1：产生停止充电故障，跳转到故障态
    //情况2：绝缘检测完成，跳转到充电中态
    //情况3：车端主动停止充电，跳转到结束态
    //情况4：车端故障，跳转到故障态
    
    
    if((sFaultCodeChkNum(cPrivDrvGunIdChg) != 0) || (sFaultCodeChkNum(sPrivDrvGetGunId(eGunIndex)) != 0))
    {
        sPileCnGateMakeGunStsReason(eGunIndex, ePrivDrvGunWorkStsRsnFault);
        sPileCnStateSet(eGunIndex, ePrivDrvGunWorkStatusFaulted);
    }
    else if(sBmsGbt15StateGet(eGunIndex) == eBmsGbt15StateWait)
    {
        sPileCnStateSet(eGunIndex, ePrivDrvGunWorkStatusAvailable);
    }
    else if(sBmsGbt15StateGet(eGunIndex) == eBmsGbt15StateCharging)
    {
        sPileCnGateMakeGunStsReason(eGunIndex, ePrivDrvGunWorkStsRsnStartCharging);
        sPileCnStateSet(eGunIndex, ePrivDrvGunWorkStatusCharging);
    }
    else if(sBmsGbt15StateGet(eGunIndex) == eBmsGbt15StateFinish)
    {
        sPileCnStateSet(eGunIndex, ePrivDrvGunWorkStatusFinishing);
        sPileCnSet_Authorize(eGunIndex, eAuthorizeTypeNone);
    }
    else if(sBmsGbt15StateGet(eGunIndex) == eBmsGbt15StateFaultAB)
    {
        sPileCnGateMakeGunStsReason(eGunIndex, ePrivDrvGunWorkStsRsnFault);
        sPileCnStateSet(eGunIndex, ePrivDrvGunWorkStatusFaulted);
    }
    
}







/**********************************************************************************************
* Description                           :   国标桩 状态机管理 之 Finishing 状态管理
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-20
* notice                                :   
***********************************************************************************************/
void sPileCnStateFinishing(ePileGunIndex_t eGunIndex)
{
    stPileCnGateData_t *pData = &pPileCnCache->stGateData;
    
    /*************充电结束*************/
    
    //情况1：产生停止充电故障，跳转到故障态
    //情况2：检测到拔枪操作，返回待机态
    //情况3：检测到授权指令，开启二次充电流程
    
    eBmsGbt15ProtState_t eState;
    
#if defined(DC480K_G2SF311_600)
    //液冷机停止工作
    sCoolingAppSetPowerOnOff(eRemoteCtrlStop);
#endif
    
    if(sOrderGetStep(eGunIndex) == eOrderStepRun)
    {
        //停止计量
        sPileCnGateMakeStopReason(eGunIndex, ePrivDrvStopReasonCar);            //如果未被赋值为其他停充原因，就应该是车端主动停充
        sOrderStop(eGunIndex, pData->eStopReason[eGunIndex]);
        sPileCnGateClrStopReason(eGunIndex);                                    //用完以后给默认值
    }
    
    if((sFaultCodeChkNum(cPrivDrvGunIdChg) != 0) || (sFaultCodeChkNum(sPrivDrvGetGunId(eGunIndex)) != 0))
    {
        //产生故障跳转到Fault
        sPileCnGateMakeGunStsReason(eGunIndex, ePrivDrvGunWorkStsRsnFault);
        sPileCnStateSet(eGunIndex, ePrivDrvGunWorkStatusFaulted);
    }
    else if(sAdcAppSampleGetCc1State(eGunIndex) != eAdcAppCc1StateU1c)
    {
        //无故障拔枪返回待机
        sBmsGbt15Set_Return2Wait(eGunIndex);
        sPileCnStateSet(eGunIndex, ePrivDrvGunWorkStatusAvailable);
        sPileCnSet_Authorize(eGunIndex, eAuthorizeTypeNone);
    }
    else if(sPileCnGet_Authorize(eGunIndex) != eAuthorizeTypeNone)
    {
        //未拔枪再次收到充电鉴权，开启二次充电流程
        
    }
}






/**********************************************************************************************
* Description                           :   国标桩 状态机管理 之 Reserved 状态管理
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-20
* notice                                :   
***********************************************************************************************/
void sPileCnStateReserved(ePileGunIndex_t eGunIndex)
{
    /*************充电预约*************/
    
}






/**********************************************************************************************
* Description                           :   国标桩 状态机管理 之 Unavailable 状态管理
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-20
* notice                                :   
***********************************************************************************************/
void sPileCnStateUnavailable(ePileGunIndex_t eGunIndex)
{
    /*************桩设备禁用*************/
    
    //情况1：掉电恢复
    
}






/**********************************************************************************************
* Description                           :   国标桩 状态机管理 之 Faulted 状态管理
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-20
* notice                                :   
***********************************************************************************************/
void sPileCnStateFaulted(ePileGunIndex_t eGunIndex)
{
    stPileCnGateData_t *pData = &pPileCnCache->stGateData;
    
    /*************桩设备故障*************/
    
    //情况1：故障总数为0，未插枪返回待机态
    
    sPileCnSet_Authorize(eGunIndex, eAuthorizeTypeNone);
    
#if defined(DC480K_G2SF311_600)
    //液冷机停止工作
    sCoolingAppSetPowerOnOff(eRemoteCtrlStop);
#endif
    
    if(sOrderGetStep(eGunIndex) == eOrderStepRun)
    {
        //停止计量
        if(sFaultCodeChkCode(cPrivDrvGunIdChg, ePrivDrvFaultCode_ccuEstopErr) == true)
        {
            sPileCnGateMakeStopReason(eGunIndex, ePrivDrvStopReasonEmgShutdown);
        }
        else if(sFaultCodeChkCode(eGunIndex + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuOutsideVoltHiErr) == true)
        {
            sPileCnGateMakeStopReason(eGunIndex, ePrivDrvStopReasonCarRunning);
        }
        else if(sFaultCodeChkCode(cPrivDrvGunIdChg, ePrivDrvFaultCode_dispenserDoorErr) == true)
        {
            sPileCnGateMakeStopReason(eGunIndex, ePrivDrvStopReasonDoorOpen);
        }
        else
        {
            sPileCnGateMakeStopReason(eGunIndex, ePrivDrvStopReasonFault);
        }
        sOrderStop(eGunIndex, pData->eStopReason[eGunIndex]);
        sPileCnGateClrStopReason(eGunIndex);                                    //用完以后给默认值
    }
    
    //当前系统故障和枪故障数为0，则返回待机态
    if((sFaultCodeChkNum(cPrivDrvGunIdChg) == 0) && (sFaultCodeChkNum(sPrivDrvGetGunId(eGunIndex)) == 0))
    {
        if(sAdcAppSampleGetCc1State(eGunIndex) == eAdcAppCc1StateU1c)
        {
            if(sBmsGbt15StateGet(eGunIndex) != eBmsGbt15StateFinish)
            {
                sPileCnStateSet(eGunIndex, ePrivDrvGunWorkStatusPreparing);
            }
            else
            {
                sPileCnStateSet(eGunIndex, ePrivDrvGunWorkStatusFinishing);
            }
        }
        else
        {
            if(sBmsGbt15StateGet(eGunIndex) != eBmsGbt15StateWait)
            {
                sBmsGbt15Set_Return2Wait(eGunIndex);
            }
            sPileCnStateSet(eGunIndex, ePrivDrvGunWorkStatusAvailable);
        }
    }
}








