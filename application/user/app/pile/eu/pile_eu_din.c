/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   pile_eu_din.c
* Description                           :   基于din70121的欧标充电桩用户代码
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-02-26
* notice                                :   
****************************************************************************************************/
#include "io_app.h"
#include "iso.h"
#include "mod_app_avg.h"
#include "meter_drv_opt.h"

#include "pile_eu_din.h"

#include "bms_ccs.h"
#include "bms_ccs_v2g_din_api.h"
#include "bms_ccs_v2g_din_basic.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "pile_eu_din";




extern stPileEuCache_t stPileEuCache;





//din70121主循环
void sPileEuDinMainLoop(ePileGunIndex_t eGunIndex);

//din70121缓存数据字段初始化
void sPileEuDinInitV2gData(ePileGunIndex_t eGunIndex);
void sPileEuDinInitV2gData_SAScheduleList(ePileGunIndex_t eGunIndex);

//Plug to charge 插枪无鉴权充电功能
void sPileEuDinPtc(ePileGunIndex_t eGunIndex);

//ISO绝缘检测
void sPileEuDinIsoCheck(ePileGunIndex_t eGunIndex);
f32  sPileEuDinIsoCheckVoltCalc(ePileGunIndex_t eGunIndex);

//precharge 预充电
void sPileEuDinPreCharge(ePileGunIndex_t eGunIndex);

//EVSE Output---Enabled 闭合接触器开始输出
void sPileEuDinChargingStart(ePileGunIndex_t eGunIndex);

//充电中数据更新
void sPileEuDinCharging(ePileGunIndex_t eGunIndex);

//停止充电
void sPileEuDinChargingStop(ePileGunIndex_t eGunIndex);

















/***************************************************************************************************
* Description                           :   基于din70121的欧标充电桩用户代码 主循环
* Author                                :   Hall
* Creat Date                            :   2024-02-26
* notice                                :   
****************************************************************************************************/
void sPileEuDinMainLoop(ePileGunIndex_t eGunIndex)
{
    //v2g协议底层数据初始化
    sPileEuDinInitV2gData(eGunIndex);
    
    //PTC插枪无鉴权充电功能
    sPileEuDinPtc(eGunIndex);
    
    //iso绝缘检测
    sPileEuDinIsoCheck(eGunIndex);
    
    //PreCharge 预充电
    sPileEuDinPreCharge(eGunIndex);
    
    //闭合接触器启动充电---暂时屏蔽了，闭合接触器动作已经挪到预充电阶段
    //sPileEuDinChargingStart(eGunIndex);
    
    //充电过程中数据更新
    sPileEuDinCharging(eGunIndex);
    
    //停充检查
    sPileEuDinChargingStop(eGunIndex);
}








/***************************************************************************************************
* Description                           :   基于din70121的欧标充电桩用户代码---初始化V2G缓存数据
* Author                                :   Hall
* Creat Date                            :   2024-02-26
* notice                                :   检查缓存数据里的握手信号 有信号代表刚刚握手，缓存数据已经复位
*                                           需要重新初始化一系列字段
*
*                                           充电桩的固有参数，例如最大电压、最大电流...等，都应该在此函数
*                                           内配置，以便发送给车端
****************************************************************************************************/
void sPileEuDinInitV2gData(ePileGunIndex_t eGunIndex)
{
    i32  i32PlcIndex = eGunIndex - stPileEuCache.eGunIndexBaseCcs;
    
    //枪输出能力
    f32 f32UdcMax = 0.0f, f32UdcMin = 0.0f, f32IdcMax = 0.0f, f32Pdc = 0.0f;
    
    u8  u8EvseId[1];
    struct dinPhysicalValueType stValue;
    dinpaymentOptionType i32PaymentOption = dinpaymentOptionType_ExternalPayment;
    struct dinDC_EVSEStatusType stDC_EVSEStatus =
    {
        .EVSEIsolationStatus = dinisolationLevelType_Invalid,
        .EVSEIsolationStatus_isUsed = 1u,
        .EVSEStatusCode = dinDC_EVSEStatusCodeType_EVSE_Ready,
        .NotificationMaxDelay = 0,
        .EVSENotification = dinEVSENotificationType_None,
    };
    
    struct dinSAScheduleTupleType *pSAScheduleTuple = NULL;
    
    
    //sModAppAvgGetGunOutputAbility(eGunIndex, &f32UdcMax, &f32UdcMin, &f32IdcMax, &f32Pdc);
    if(sV2gDinGet_SupportedAppProtocol(i32PlcIndex) == true)
    {
        //字段:EVSEProcessing
        sV2gDinSet_EVSEProcessing(i32PlcIndex, eV2gDinMsgIdMax, dinEVSEProcessingType_Ongoing);
        
        //字段:DC_EVSEStatus
        sV2gDinSet_DC_EVSEStatus(i32PlcIndex, eV2gDinMsgIdMax, &stDC_EVSEStatus);
        
        stDC_EVSEStatus.EVSEStatusCode = dinDC_EVSEStatusCodeType_EVSE_IsolationMonitoringActive;
        sV2gDinSet_DC_EVSEStatus(i32PlcIndex, eV2gDinMsgIdCableCheck, &stDC_EVSEStatus);
        
        stDC_EVSEStatus.EVSEIsolationStatus = dinisolationLevelType_Valid;
        stDC_EVSEStatus.EVSEStatusCode = dinDC_EVSEStatusCodeType_EVSE_Ready;
        sV2gDinSet_DC_EVSEStatus(i32PlcIndex, eV2gDinMsgIdWeldingDetection, &stDC_EVSEStatus);
        
        //字段:最大电流
        sV2gDinSetPhyValue(&stValue, f32IdcMax, dinunitSymbolType_A);
        sV2gDinSet_EVSEMaximumCurrentLimit(i32PlcIndex, eV2gDinMsgIdMax, &stValue);
        
        //字段:最大功率
        sV2gDinSetPhyValue(&stValue, f32Pdc, dinunitSymbolType_W);
        sV2gDinSet_EVSEMaximumPowerLimit  (i32PlcIndex, eV2gDinMsgIdMax, &stValue);
        
        //字段:最大电压
        sV2gDinSetPhyValue(&stValue, f32UdcMax, dinunitSymbolType_V);
        sV2gDinSet_EVSEMaximumVoltageLimit(i32PlcIndex, eV2gDinMsgIdMax, &stValue);
        
        //字段:当前电压
        sV2gDinSetPhyValue(&stValue, 0, dinunitSymbolType_V);
        sV2gDinSet_EVSEPresentVoltage     (i32PlcIndex, eV2gDinMsgIdMax, &stValue);
        
        //字段:EVSEID
        u8EvseId[0] = 0;
        sV2gDinSet_EVSEID(i32PlcIndex, sizeof(u8EvseId), u8EvseId);
        
        //字段:支付选项和充电服务
        sV2gDinSet_PaymentOptions(i32PlcIndex, 1, &i32PaymentOption);
        sV2gDinSet_ChargeService(i32PlcIndex, 0, NULL, 0, dinserviceCategoryType_EVCharging, NULL, 0, false, dinEVSESupportedEnergyTransferType_DC_extended);
        
        //字段:SAScheduleList
        sPileEuDinInitV2gData_SAScheduleList(eGunIndex);
        
        //字段:最小电流
        sV2gDinSetPhyValue(&stValue, 0, dinunitSymbolType_A);
        sV2gDinSet_EVSEMinimumCurrentLimit(i32PlcIndex, &stValue);
        
        //字段:最小电压
        sV2gDinSetPhyValue(&stValue, f32UdcMin, dinunitSymbolType_V);
        sV2gDinSet_EVSEMinimumVoltageLimit(i32PlcIndex, &stValue);
        
        //字段:EVSE调节容差的绝对幅度--可选字段
        sV2gDinSet_EVSECurrentRegulationTolerance(i32PlcIndex, NULL);
        
        //字段:EVSE电流纹波的峰峰值幅度
        sV2gDinSetPhyValue(&stValue, 0, dinunitSymbolType_A);
        sV2gDinSet_EVSEPeakCurrentRipple(i32PlcIndex, &stValue);
        
        //字段:EVSE 提供的能量
        sV2gDinSetPhyValue(&stValue, 0, dinunitSymbolType_Wh);
        sV2gDinSet_EVSEEnergyToBeDelivered(i32PlcIndex, &stValue);
        
        //字段:当前电流
        sV2gDinSetPhyValue(&stValue, 0, dinunitSymbolType_A);
        sV2gDinSet_EVSEPresentCurrent(i32PlcIndex, &stValue);
        
        
        //此刻参数发现的字段已经全部初始化，可以设置 Finished 了
        sV2gDinSet_EVSEProcessing(i32PlcIndex, eV2gDinMsgIdChargeParameterDiscovery, dinEVSEProcessingType_Finished);
        
    }
}








/***************************************************************************************************
* Description                           :   基于din70121的欧标充电桩用户代码---初始化V2G缓存数据 之 SAScheduleList 字段
* Author                                :   Hall
* Creat Date                            :   2024-02-28
* notice                                :   struct dinSAScheduleTupleType 字段占用2K多ram 用动态分配的方式以便节省ram
*                                           这个字段的处理代码比较繁琐，单独用一个函数实现
****************************************************************************************************/
void sPileEuDinInitV2gData_SAScheduleList(ePileGunIndex_t eGunIndex)
{
    i32  i32PlcIndex = eGunIndex - stPileEuCache.eGunIndexBaseCcs;
    
    static struct dinSAScheduleTupleType *pSAScheduleTuple = NULL;
    
    if(pSAScheduleTuple == NULL)
    {
        pSAScheduleTuple = (struct dinSAScheduleTupleType *)MALLOC(sizeof(struct dinSAScheduleTupleType));
    }
    
    if(pSAScheduleTuple != NULL)
    {
        memset(pSAScheduleTuple, 0, sizeof(struct dinSAScheduleTupleType));
        
        *pSAScheduleTuple = (struct dinSAScheduleTupleType)
        {
            .SAScheduleTupleID = 0,
            .SalesTariff_isUsed = 0u,
            .PMaxSchedule = 
            {
                .PMaxScheduleID = 0,
                .PMaxScheduleEntry = 
                {
                    .arrayLen = 1,
                    .array[0] = 
                    {
                        .TimeInterval_isUsed = 0u,
                        .RelativeTimeInterval_isUsed = 1u,
                        .PMax = 0x7ffe,
                        .RelativeTimeInterval = 
                        {
                            .start = 0,
                            .duration_isUsed = 1u,
                            .duration = 24 * 3600,
                        },
                        
                    },
                },
            },
        };
        
        
        sV2gDinSet_SAScheduleList(i32PlcIndex, true, pSAScheduleTuple);
    }
}







/***************************************************************************************************
* Description                           :   基于din70121的欧标充电桩用户代码 Plug to charge 插枪无鉴权充电功能
* Author                                :   Hall
* Creat Date                            :   2024-03-11
* notice                                :   此功能需要根据配置参数开启，相关代码等待完善 目前无脑开启
****************************************************************************************************/
void sPileEuDinPtc(ePileGunIndex_t eGunIndex)
{
    eV2gDinMsgId_t eId;
    i32  i32PlcIndex = eGunIndex - stPileEuCache.eGunIndexBaseCcs;
    
    if(sV2gDinGet_MsgId(i32PlcIndex, &eId) == true)
    {
        if(eId == eV2gDinMsgIdContractAuthorization)
        {
            sV2gDinSet_EVSEProcessing(i32PlcIndex, eV2gDinMsgIdContractAuthorization, dinEVSEProcessingType_Finished);
        }
    }
}








/***************************************************************************************************
* Description                           :   基于din70121的欧标充电桩用户代码 iso绝缘检测功能
* Author                                :   Hall
* Creat Date                            :   2024-03-12
* notice                                :   
****************************************************************************************************/
void sPileEuDinIsoCheck(ePileGunIndex_t eGunIndex)
{
    eV2gDinMsgId_t eId;
    static eV2gDinMsgId_t eIdOld[ePileGunIndexNum];
    eIsoMechinaStatus_t *pIsoRst;
    
    i32  i32PlcIndex = eGunIndex - stPileEuCache.eGunIndexBaseCcs;
    struct dinDC_EVSEStatusType stDC_EVSEStatus =
    {
        .EVSEIsolationStatus = dinisolationLevelType_Invalid,
        .EVSEIsolationStatus_isUsed = 1u,
        .EVSEStatusCode = dinDC_EVSEStatusCodeType_EVSE_Ready,
        .NotificationMaxDelay = 0,
        .EVSENotification = dinEVSENotificationType_None,
    };
    
    
    //1:如果V2G会话结束 需要判断ISO检测逻辑是否需要关闭
    if(sBmsCcsStateGet(i32PlcIndex) != eBmsCcsStateV2g)
    {
        eIdOld[eGunIndex] = eV2gDinMsgIdSupportedAppProtocol;
        pIsoRst = sIosCheck(eGunIndex, eIsoCheckBeforeCharge);
        if(pIsoRst->eIsoCheckStatus != eIsoCheckIdel)
        {
            EN_SLOGD(TAG, "欧标充电桩,枪%d, V2G会话已经结束, 停止ISO检测逻辑!!", eGunIndex);
            sIsoStop(eGunIndex, eIsoCheckBeforeCharge);
        }
        
        return;
    }
    
    //2:ISO检测执行
    if((sV2gDinGet_MsgId(i32PlcIndex, &eId) == true) && (eId == eV2gDinMsgIdCableCheck))
    {
        //2.1:首次进入时需要启动一下ISO资源
        if(eIdOld[eGunIndex] != eId)
        {
            sIsoStart(eGunIndex, sPileEuDinIsoCheckVoltCalc(eGunIndex));
        }
        
        //2.2:持续调用并检查ISO检测结果
        pIsoRst = sIosCheck(eGunIndex, eIsoCheckBeforeCharge);
        if(pIsoRst->eIsoCheckStatus == eIsoCheckIdel)
        {
            if((pIsoRst->eIsoRst == eIsoOk) || (pIsoRst->eIsoRst == eIsoIa))
            {
                stDC_EVSEStatus.EVSEIsolationStatus = dinisolationLevelType_Valid;
                stDC_EVSEStatus.EVSEStatusCode = dinDC_EVSEStatusCodeType_EVSE_Ready;
                sV2gDinSet_ResponseCode(i32PlcIndex, eV2gDinMsgIdCableCheck, dinresponseCodeType_OK);
            }
            else
            {
                stDC_EVSEStatus.EVSEIsolationStatus = dinisolationLevelType_Fault;
                stDC_EVSEStatus.EVSEStatusCode = dinDC_EVSEStatusCodeType_EVSE_Shutdown;
                sV2gDinSet_ResponseCode(i32PlcIndex, eV2gDinMsgIdCableCheck, dinresponseCodeType_FAILED);
            }
            
            sV2gDinSet_EVSEProcessing(i32PlcIndex, eV2gDinMsgIdCableCheck, dinEVSEProcessingType_Finished);
        }
        else
        {
            stDC_EVSEStatus.EVSEIsolationStatus = dinisolationLevelType_Invalid;
            stDC_EVSEStatus.EVSEStatusCode = dinDC_EVSEStatusCodeType_EVSE_IsolationMonitoringActive;
            sV2gDinSet_EVSEProcessing(i32PlcIndex, eV2gDinMsgIdCableCheck, dinEVSEProcessingType_Ongoing);
        }
        
        //ISO绝缘检测的结果出来以后，更新以下消息的此字段
        //等到充电中绝缘检测时再对其更新
        sV2gDinSet_DC_EVSEStatus(i32PlcIndex, eV2gDinMsgIdCableCheck,       &stDC_EVSEStatus);
        sV2gDinSet_DC_EVSEStatus(i32PlcIndex, eV2gDinMsgIdPreCharge,        &stDC_EVSEStatus);
        sV2gDinSet_DC_EVSEStatus(i32PlcIndex, eV2gDinMsgIdPowerDelivery1,   &stDC_EVSEStatus);
        sV2gDinSet_DC_EVSEStatus(i32PlcIndex, eV2gDinMsgIdCurrentDemand,    &stDC_EVSEStatus);
        
        eIdOld[eGunIndex] = eId;
    }
    
}








/***************************************************************************************************
* Description                           :   基于din70121的欧标充电桩用户代码 计算ISO绝缘检测电压
* Author                                :   Hall
* Creat Date                            :   2024-04-16
* notice                                :   
****************************************************************************************************/
f32 sPileEuDinIsoCheckVoltCalc(ePileGunIndex_t eGunIndex)
{
    f32 f32Uiso;
    
    f32 f32UdcOutMax = 0.0f;                                                           //枪最大输出电压
    f32 f32UevMax;                                                              //车端允许最大电压
    struct dinPhysicalValueType stValue;
    
    sV2gDinGet_EVMaximumVoltageLimit(eGunIndex, &stValue);
    f32UevMax = sV2gDinGetPhyValue(&stValue, NULL);
    
    //sModAppAvgGetGunOutputAbility(eGunIndex, &f32UdcOutMax, NULL, NULL, NULL);
    
    if(f32UevMax <= 500)
    {
        f32Uiso = ((f32UevMax + 50) < 500) ? (f32UevMax + 50) : 500;
        f32Uiso = (f32Uiso < f32UdcOutMax) ? f32Uiso : f32UdcOutMax;
    }
    else
    {
        f32Uiso = ((f32UevMax * 1.1f) < f32UdcOutMax) ? (f32UevMax * 1.1f) : f32UdcOutMax;
    }
    
    return(f32Uiso);
}









/***************************************************************************************************
* Description                           :   基于din70121的欧标充电桩用户代码 预充电
* Author                                :   Hall
* Creat Date                            :   2024-03-12
* notice                                :   
****************************************************************************************************/
void sPileEuDinPreCharge(ePileGunIndex_t eGunIndex)
{
    f32  f32UdcIn = 0;                                                          //内侧电压
    f32  f32UdcOut = 0;                                                         //外侧电压
    f32  f32IdcOut;
    
    eV2gDinMsgId_t eId;
    static eV2gDinMsgId_t eIdOld[ePileGunIndexNum];
    
    i32  i32PlcIndex = eGunIndex - stPileEuCache.eGunIndexBaseCcs;
    
    f32 f32UdcTarget, f32IdcTarget;
    struct dinPhysicalValueType stUdcOut, stUdcTarget, stIdcTarget;
    
    
    //获取内外侧电压值Udc
    sAdcAppSampleGetGunDcOut(eGunIndex, &f32UdcOut);
    sMeterDrvOptGetUdcIdcPdc(eGunIndex, &f32UdcIn, NULL, NULL);
    
    
    //1:如果V2G会话结束且枪输出有电压，需要关闭枪输出
    if(sBmsCcsStateGet(i32PlcIndex) != eBmsCcsStateV2g)
    {
        eIdOld[eGunIndex] = eV2gDinMsgIdSupportedAppProtocol;
        if(f32UdcIn > cPileEuUdcValue1)
        {
            sModAppAvgClose(eGunIndex);
        }
        
        return;
    }
    
    
    if((sV2gDinGet_MsgId(i32PlcIndex, &eId) == true) && (eId == eV2gDinMsgIdPreCharge))
    {
        if(f32UdcOut < cPileEuUdcValue1)
        {
            if(sAdcAppSampleGetFbStateK(eGunIndex) == eAdcAppFbState00)
            {
                if(eIdOld[eGunIndex] != eId)
                {
                    //先闭合桩端接触器
                    sIoSetDrK(eGunIndex, true);
                    
                    //开始升压
                    sV2gDinGet_EVTargetVoltage(i32PlcIndex, &stUdcTarget);
                    sV2gDinGet_EVTargetCurrent(i32PlcIndex, &stIdcTarget);
                    f32UdcTarget = sV2gDinGetPhyValue(&stUdcTarget, NULL);
                    f32IdcTarget = sV2gDinGetPhyValue(&stIdcTarget, NULL);
                    
                    if(sModAppAvgOpen(eGunIndex) == true)
                    {
                        sModAppAvgSetOutput(eGunIndex, f32UdcTarget, f32IdcTarget);
                    }
                }
            }
            else
            {
                //桩端粘连 要报故障
            }
        }
        else
        {
            //车端粘连 要报故障
        }
        
        sAdcAppSampleGetGunDcOut(eGunIndex, &f32UdcOut);
        sV2gDinSetPhyValue(&stUdcOut, f32UdcOut, dinunitSymbolType_V);
        sV2gDinSet_EVSEPresentVoltage(i32PlcIndex, eV2gDinMsgIdPreCharge, &stUdcOut);
        
        eIdOld[eGunIndex] = eId;
    }
}








/***************************************************************************************************
* Description                           :   基于din70121的欧标充电桩用户代码 闭合接触器开始输出
* Author                                :   Hall
* Creat Date                            :   2024-03-28
* notice                                :   对应时序图上的 EVSE Output---Enabled 信号
*
*                                           闭合接触器需要分两种情况
*                                           1：电阻负载，必须确保外侧电压接近0v，才可以闭合
*                                           2：电池负载，必须确保内侧电压低于外侧电压不超过10V，才可以闭合
*                                           如果不满足以上条件，则不能闭合接触器，并报错
*                                           同时将 CurrentDemandRes 消息的 ResponseCode 设置 FAILED 值以便结束充电
****************************************************************************************************/
void sPileEuDinChargingStart(ePileGunIndex_t eGunIndex)
{
    f32  f32UdcIn = 0;                                                          //内侧电压
    f32  f32UdcOut = 0;                                                         //外侧电压
    i32  i32ReadyToChargeState;
    eV2gDinMsgId_t eId;
    i32  i32PlcIndex = eGunIndex - stPileEuCache.eGunIndexBaseCcs;
    
    
    //获取内外侧电压值Udc
    sAdcAppSampleGetGunDcOut(eGunIndex, &f32UdcOut);
    sMeterDrvOptGetUdcIdcPdc(eGunIndex, &f32UdcIn, NULL, NULL);
    
    if((sV2gDinGet_MsgId(i32PlcIndex, &eId) == true)
    && (sV2gDinGet_ReadyToChargeState(i32PlcIndex, &i32ReadyToChargeState) == true))
    {
        if((eId == eV2gDinMsgIdPowerDelivery1)
        && (i32ReadyToChargeState == 1) 
        && (sAdcAppSampleGetFbStateK(eGunIndex) != eAdcAppFbState11))
        {
            if(1)//当前写死电阻载
            {
                if(f32UdcOut < 10)
                {
                    sIoSetDrK(eGunIndex, true);
                }
            }
            else if(0)//电池载
            {
                if(((f32UdcOut - f32UdcIn) > 0) && ((f32UdcOut - f32UdcIn) < 10))
                {
                    sIoSetDrK(eGunIndex, true);
                }
            }
        }
    }
}










/***************************************************************************************************
* Description                           :   基于din70121的欧标充电桩用户代码 充电中数据更新逻辑
* Author                                :   Hall
* Creat Date                            :   2024-03-28
* notice                                :   
****************************************************************************************************/
void sPileEuDinCharging(ePileGunIndex_t eGunIndex)
{
    eV2gDinMsgId_t eId;
    i32  i32PlcIndex = eGunIndex - stPileEuCache.eGunIndexBaseCcs;
    
    
    //枪输出能力
    f32 f32UdcMax = 0.0f, f32UdcMin = 0.0f, f32IdcMax = 0.0f;
    
    //需求电压电流
    f32 f32UdcTarget, f32IdcTarget;
    struct dinPhysicalValueType stUdcTarget, stIdcTarget;
    
    //实际输出电压电流
    f32 f32UdcOut, f32IdcOut;
    struct dinPhysicalValueType stUdcOut ,stIdcOut;
    
    
    
    //获取枪输出能力
    //sModAppAvgGetGunOutputAbility(eGunIndex, &f32UdcMax, &f32UdcMin, &f32IdcMax, NULL);
    if(sV2gDinGet_MsgId(i32PlcIndex, &eId) == true)
    {
        if(eId == eV2gDinMsgIdCurrentDemand)
        {
            //收到的需求电压电流向枪输出设置
            sV2gDinGet_EVTargetVoltage(i32PlcIndex, &stUdcTarget);
            sV2gDinGet_EVTargetCurrent(i32PlcIndex, &stIdcTarget);
            f32UdcTarget = sV2gDinGetPhyValue(&stUdcTarget, NULL);
            f32IdcTarget = sV2gDinGetPhyValue(&stIdcTarget, NULL);
            f32UdcTarget = (f32UdcTarget < f32UdcMax) ? f32UdcTarget : f32UdcMax;
            f32UdcTarget = (f32UdcTarget > f32UdcMin) ? f32UdcTarget : f32UdcMin;
            f32IdcTarget = (f32IdcTarget < f32IdcMax) ? f32IdcTarget : f32IdcMax;
            sModAppAvgSetOutput(eGunIndex, f32UdcTarget, f32IdcTarget);
            
            
            //枪实际输出值更新给车端
            if(0)//(sModAppAvgGetGunOutput(eGunIndex, &f32UdcOut, &f32IdcOut) == true)
            {
                sV2gDinSetPhyValue(&stUdcOut, f32UdcOut, dinunitSymbolType_V);
                sV2gDinSetPhyValue(&stIdcOut, f32IdcOut, dinunitSymbolType_A);
                sV2gDinSet_EVSEPresentVoltage(i32PlcIndex, eV2gDinMsgIdCurrentDemand, &stUdcOut);
                sV2gDinSet_EVSEPresentCurrent(i32PlcIndex, &stIdcOut);
            }
        }
    }
}








/***************************************************************************************************
* Description                           :   基于din70121的欧标充电桩用户代码 停充检查及执行
* Author                                :   Hall
* Creat Date                            :   2024-03-29
* notice                                :   
*                                           分4种情况：
*                                           1： 车端正常停充
*                                               检测到 eV2gDinMsgIdPowerDelivery2 消息关模块输出，判输出电流小于5A断接触器
*
*                                           2： 车端紧急停充
*
*                                           3： 桩端正常停充
*                                               eV2gDinMsgIdCurrentDemand 消息里最大电流值调整到5A，
*                                               模块最大输出电流限值到5A，检查实际输出电流，小于5A则
*                                               EVSEStatusCode 置 dinDC_EVSEStatusCodeType_EVSE_Shutdown
*                                               然后等待车端发送 eV2gDinMsgIdPowerDelivery2 消息
*
*                                           4： 桩端紧急停充
****************************************************************************************************/
void sPileEuDinChargingStop(ePileGunIndex_t eGunIndex)
{
    f32 f32Idc;
    eV2gDinMsgId_t eId;
    i32  i32PlcIndex = eGunIndex - stPileEuCache.eGunIndexBaseCcs;
    
    
    if(sV2gDinGet_MsgId(i32PlcIndex, &eId) == true)
    {
        //情况1：车端正常停充
        if((eId >= eV2gDinMsgIdPowerDelivery2)
        && (sAdcAppSampleGetFbStateK(eGunIndex) == eAdcAppFbState11))
        {
            sModAppAvgClose(eGunIndex);
            sMeterDrvOptGetUdcIdcPdc(eGunIndex, NULL, &f32Idc, NULL);
            if(f32Idc < 5)
            {
                sIoSetDrK(eGunIndex, false);
            }
        }
    }
}








































