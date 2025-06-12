/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   bms_ccs_v2g_din_api.c
* Description                           :   DIN70121 协议实现 之 对外的api接口实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-05-24
* notice                                :   
****************************************************************************************************/
#include "bms_ccs_v2g_din_api.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "bms_ccs_v2g_din_api";







extern stV2gDinCache_t *pV2gDinCache[cMse102xDevNum];



//v2g消息 msgId 字段读取
bool sV2gDinGet_MsgId(i32 i32Index, eV2gDinMsgId_t *pId);

//多消息公用的api函数
bool sV2gDinSet_ResponseCode(i32 i32Index, eV2gDinMsgId_t eId, i32 i32ResponseCode);
bool sV2gDinSet_EVSEProcessing(i32 i32Index, eV2gDinMsgId_t eId, i32 i32EvseProcessing);
bool sV2gDinGet_DC_EVStatus(i32 i32Index, struct dinDC_EVStatusType *pDC_EVStatus);
bool sV2gDinGet_EVMaximumCurrentLimit(i32 i32Index, struct dinPhysicalValueType *pEVMaximumCurrentLimit);
bool sV2gDinGet_EVMaximumPowerLimit(i32 i32Index, struct dinPhysicalValueType *pEVMaximumPowerLimit);
bool sV2gDinGet_EVMaximumVoltageLimit(i32 i32Index, struct dinPhysicalValueType *pEVMaximumVoltageLimit);
bool sV2gDinSet_DC_EVSEStatus(i32 i32Index, eV2gDinMsgId_t eId, struct dinDC_EVSEStatusType *pDC_EVSEStatus);
bool sV2gDinSet_EVSEMaximumCurrentLimit(i32 i32Index, eV2gDinMsgId_t eId, struct dinPhysicalValueType *pEVSEMaximumCurrentLimit);
bool sV2gDinSet_EVSEMaximumPowerLimit(i32 i32Index, eV2gDinMsgId_t eId, struct dinPhysicalValueType *pEVSEMaximumPowerLimit);
bool sV2gDinSet_EVSEMaximumVoltageLimit(i32 i32Index, eV2gDinMsgId_t eId, struct dinPhysicalValueType *pEVSEMaximumVoltageLimit);
bool sV2gDinGet_EVTargetVoltage(i32 i32Index, struct dinPhysicalValueType *pEVTargetVoltage);
bool sV2gDinGet_EVTargetCurrent(i32 i32Index, struct dinPhysicalValueType *pEVTargetCurrent);
bool sV2gDinSet_EVSEPresentVoltage(i32 i32Index, eV2gDinMsgId_t eId, struct dinPhysicalValueType *pEVSEPresentVoltage);
bool sV2gDinGet_BulkChargingComplete(i32 i32Index, i32 *pBulkChargingComplete);
bool sV2gDinGet_ChargingComplete(i32 i32Index, i32 *pChargingComplete);

//SupportedAppProtocol
bool sV2gDinGet_SupportedAppProtocol(i32 i32Index);

//SessionSetup
bool sV2gDinGet_EVCCID(i32 i32Index, i32 i32MaxLen, u8 *pEvccId);
bool sV2gDinSet_EVSEID(i32 i32Index, i32 i32Len, const u8 *pEvseId);

//ServiceDiscovery
bool sV2gDinGet_ServiceScope(i32 i32Index, i32 i32MaxLen, i32 *pServiceScope);
bool sV2gDinGet_ServiceCategory(i32 i32Index, i32 *pServiceCategory);
bool sV2gDinSet_PaymentOptions(i32 i32Index, i32 i32PaymentOptionNum, const dinpaymentOptionType *pPaymentOption);
bool sV2gDinSet_ChargeService(i32 i32Index, u16 u16Id, i32 *pName, i32 i32NameLen, i32 i32Category, i32 *pScope, i32 i32ScopeLen, bool bFree, i32 i32EnergyTsfType);

//ServicePaymentSelection
bool sV2gDinGet_SelectedPaymentOption(i32 i32Index, i32 *pSelectedPaymentOption);

//ContractAuthentication---无

//ChargeParameterDiscovery
bool sV2gDinGet_EVRequestedEnergyTransferType(i32 i32Index, i32 *pEvRequestedEnergyTransferType);
bool sV2gDinGet_EVEnergyCapacity(i32 i32Index, struct dinPhysicalValueType *pEVEnergyCapacity);
bool sV2gDinGet_EVEnergyRequest(i32 i32Index, struct dinPhysicalValueType *pEVEnergyRequest);
bool sV2gDinGet_FullSOC(i32 i32Index, i8 *pFullSOC);
bool sV2gDinGet_BulkSOC(i32 i32Index, i8 *pBulkSOC);
bool sV2gDinSet_SAScheduleList(i32 i32Index, bool bClearFlag, struct dinSAScheduleTupleType *pSAScheduleTuple);
bool sV2gDinSet_EVSEMinimumCurrentLimit(i32 i32Index, struct dinPhysicalValueType *pEVSEMinimumCurrentLimit);
bool sV2gDinSet_EVSEMinimumVoltageLimit(i32 i32Index, struct dinPhysicalValueType *pEVSEMinimumVoltageLimit);
bool sV2gDinSet_EVSECurrentRegulationTolerance(i32 i32Index, struct dinPhysicalValueType *pEVSECurrentRegulationTolerance);
bool sV2gDinSet_EVSEPeakCurrentRipple(i32 i32Index, struct dinPhysicalValueType *pEVSEPeakCurrentRipple);
bool sV2gDinSet_EVSEEnergyToBeDelivered(i32 i32Index, struct dinPhysicalValueType *pEVSEEnergyToBeDelivered);

//CableCheck----无,都在公用api里面
//PreCharge-----无,都在公用api里面

//PowerDelivery
bool sV2gDinGet_ReadyToChargeState(i32 i32Index, i32 *pReadyToChargeState);
bool sV2gDinGet_ChargingProfile(i32 i32Index, struct dinChargingProfileType *pChargingProfile);


//CurrentDemand
bool sV2gDinGet_RemainingTimeToFullSoC(i32 i32Index, struct dinPhysicalValueType *pRemainingTimeToFullSoC);
bool sV2gDinGet_RemainingTimeToBulkSoC(i32 i32Index, struct dinPhysicalValueType *pRemainingTimeToBulkSoC);
bool sV2gDinSet_EVSEPresentCurrent(i32 i32Index, struct dinPhysicalValueType *pEVSEPresentCurrent);

//WeldingDetection-----无,都在公用api里面
//SessionStop---无





//-----------------------------------------------------------------------------





/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-03-11
* notice                                :   eId 字段
****************************************************************************************************/
bool sV2gDinGet_MsgId(i32 i32Index, eV2gDinMsgId_t *pId)
{
    stV2gDinCache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pId != NULL) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        (*pId) = pCache->eId;
        
        return(true);
    }
    
    return(false);
}









/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-24
* notice                                :   ResponseCode 字段，多个res报文都有本字段, 根据eId去写入
****************************************************************************************************/
bool sV2gDinSet_ResponseCode(i32 i32Index, eV2gDinMsgId_t eId, i32 i32ResponseCode)
{
    stV2gDinCache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (eId <= eV2gDinMsgIdMax) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        switch(eId)
        {
        case eV2gDinMsgIdSessionSetup:
            pCache->stData.stSessionSetupRes.ResponseCode = i32ResponseCode;
            break;
        case eV2gDinMsgIdServiceDiscovery:
            pCache->stData.stServiceDiscoveryRes.ResponseCode = i32ResponseCode;
            break;
        case eV2gDinMsgIdServicePaymentSelection:
            pCache->stData.stServicePaymentSelectionRes.ResponseCode = i32ResponseCode;
            break;
        case eV2gDinMsgIdContractAuthorization:
            pCache->stData.stContractAuthRes.ResponseCode = i32ResponseCode;
            break;
        case eV2gDinMsgIdChargeParameterDiscovery:
            pCache->stData.stChargeParamDiscoveryRes.ResponseCode = i32ResponseCode;
            break;
        case eV2gDinMsgIdCableCheck:
            pCache->stData.stCableCheckRes.ResponseCode = i32ResponseCode;
            break;
        case eV2gDinMsgIdPreCharge:
            pCache->stData.stPreChargeRes.ResponseCode = i32ResponseCode;
            break;
        case eV2gDinMsgIdPowerDelivery1:
        case eV2gDinMsgIdPowerDelivery2:
            pCache->stData.stPowerDeliveryRes.ResponseCode = i32ResponseCode;
            break;
        case eV2gDinMsgIdCurrentDemand:
            pCache->stData.stCurrentDemandRes.ResponseCode = i32ResponseCode;
            break;
        case eV2gDinMsgIdWeldingDetection:
            pCache->stData.stWeldingDetectionRes.ResponseCode = i32ResponseCode;
            break;
        case eV2gDinMsgIdSessionStop:
            pCache->stData.stSessionStopRes.ResponseCode = i32ResponseCode;
            break;
        default:
            break;
        }
        
        return(true);
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-24
* notice                                :   EVSEProcessing 字段，多个res报文都有本字段, 根据eId去写入
****************************************************************************************************/
bool sV2gDinSet_EVSEProcessing(i32 i32Index, eV2gDinMsgId_t eId, i32 i32EvseProcessing)
{
    stV2gDinCache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (eId <= eV2gDinMsgIdMax) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        switch(eId)
        {
        case eV2gDinMsgIdContractAuthorization:
            pCache->stData.stContractAuthRes.EVSEProcessing = i32EvseProcessing;
            break;
        case eV2gDinMsgIdChargeParameterDiscovery:
            pCache->stData.stChargeParamDiscoveryRes.EVSEProcessing = i32EvseProcessing;
            break;
        case eV2gDinMsgIdCableCheck:
            pCache->stData.stCableCheckRes.EVSEProcessing = i32EvseProcessing;
            break;
        case eV2gDinMsgIdMax:
            //定义 eId 为此值时表示把所有相关字段全部赋值，初始化缓存数据字段时 可以一次调用函数初始化全部字段
            pCache->stData.stContractAuthRes.EVSEProcessing = i32EvseProcessing;
            pCache->stData.stChargeParamDiscoveryRes.EVSEProcessing = i32EvseProcessing;
            pCache->stData.stCableCheckRes.EVSEProcessing = i32EvseProcessing;
            break;
        default:
            break;
        }
        
        return(true);
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-24
* notice                                :   多个消息都有 DC_EVStatus 字段
****************************************************************************************************/
bool sV2gDinGet_DC_EVStatus(i32 i32Index, struct dinDC_EVStatusType *pDC_EVStatus)
{
    stV2gDinCache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pDC_EVStatus != NULL) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        memcpy(pDC_EVStatus, &pCache->stDataPublic.DC_EVStatus, sizeof(struct dinDC_EVStatusType));
        
        return(true);
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-24
* notice                                :   多个消息都有 EVMaximumCurrentLimit 字段
****************************************************************************************************/
bool sV2gDinGet_EVMaximumCurrentLimit(i32 i32Index, struct dinPhysicalValueType *pEVMaximumCurrentLimit)
{
    stV2gDinCache_t *pCache = NULL;
    
    
    if((i32Index < cMse102xDevNum) && (pEVMaximumCurrentLimit != NULL) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        if(pCache->stDataPublic.EVMaximumCurrentLimit_isUsed == 1u)
        {
            memcpy(pEVMaximumCurrentLimit, &pCache->stDataPublic.EVMaximumCurrentLimit, sizeof(struct dinPhysicalValueType));
            
            return(true);
        }
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-24
* notice                                :   多个消息都有 EVMaximumPowerLimit 字段
****************************************************************************************************/
bool sV2gDinGet_EVMaximumPowerLimit(i32 i32Index, struct dinPhysicalValueType *pEVMaximumPowerLimit)
{
    stV2gDinCache_t *pCache = NULL;
    
    
    if((i32Index < cMse102xDevNum) && (pEVMaximumPowerLimit != NULL) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        if(pCache->stDataPublic.EVMaximumPowerLimit_isUsed == 1u)
        {
            memcpy(pEVMaximumPowerLimit, &pCache->stDataPublic.EVMaximumPowerLimit, sizeof(struct dinPhysicalValueType));
            
            return(true);
        }
    }
    
    return(false);
}







/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-25
* notice                                :   多个消息都有 EVMaximumVoltageLimit 字段
****************************************************************************************************/
bool sV2gDinGet_EVMaximumVoltageLimit(i32 i32Index, struct dinPhysicalValueType *pEVMaximumVoltageLimit)
{
    stV2gDinCache_t *pCache = NULL;
    
    
    if((i32Index < cMse102xDevNum) && (pEVMaximumVoltageLimit != NULL) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        if(pCache->stDataPublic.EVMaximumVoltageLimit_isUsed == 1u)
        {
            memcpy(pEVMaximumVoltageLimit, &pCache->stDataPublic.EVMaximumVoltageLimit, sizeof(struct dinPhysicalValueType));
            
            return(true);
        }
    }
    
    return(false);
}








/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-25
* notice                                :   多个消息都有 DC_EVSEStatus 字段, 根据eId去写入
****************************************************************************************************/
bool sV2gDinSet_DC_EVSEStatus(i32 i32Index, eV2gDinMsgId_t eId, struct dinDC_EVSEStatusType *pDC_EVSEStatus)
{
    stV2gDinCache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pDC_EVSEStatus != NULL) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        switch(eId)
        {
        case eV2gDinMsgIdChargeParameterDiscovery:
            memcpy(&pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.DC_EVSEStatus, pDC_EVSEStatus, sizeof(struct dinDC_EVSEStatusType));
            pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter_isUsed = 1u;
            break;
        case eV2gDinMsgIdCableCheck:
            memcpy(&pCache->stData.stCableCheckRes.DC_EVSEStatus, pDC_EVSEStatus, sizeof(struct dinDC_EVSEStatusType));
            break;
        case eV2gDinMsgIdPreCharge:
            memcpy(&pCache->stData.stPreChargeRes.DC_EVSEStatus, pDC_EVSEStatus, sizeof(struct dinDC_EVSEStatusType));
            break;
        case eV2gDinMsgIdPowerDelivery1:
        case eV2gDinMsgIdPowerDelivery2:
            memcpy(&pCache->stData.stPowerDeliveryRes.DC_EVSEStatus, pDC_EVSEStatus, sizeof(struct dinDC_EVSEStatusType));
            pCache->stData.stPowerDeliveryRes.DC_EVSEStatus_isUsed = 1u;
            break;
        case eV2gDinMsgIdCurrentDemand:
            memcpy(&pCache->stData.stCurrentDemandRes.DC_EVSEStatus, pDC_EVSEStatus, sizeof(struct dinDC_EVSEStatusType));
            break;
        case eV2gDinMsgIdWeldingDetection:
            memcpy(&pCache->stData.stWeldingDetectionRes.DC_EVSEStatus, pDC_EVSEStatus, sizeof(struct dinDC_EVSEStatusType));
            break;
        case eV2gDinMsgIdMax:
            //定义 eId 为此值时表示把所有相关字段全部赋值，初始化缓存数据字段时 可以一次调用函数初始化全部字段
            pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter_isUsed = 1u;
            memcpy(&pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.DC_EVSEStatus, pDC_EVSEStatus, sizeof(struct dinDC_EVSEStatusType));
            
            memcpy(&pCache->stData.stCableCheckRes.DC_EVSEStatus,       pDC_EVSEStatus, sizeof(struct dinDC_EVSEStatusType));
            memcpy(&pCache->stData.stPreChargeRes.DC_EVSEStatus,        pDC_EVSEStatus, sizeof(struct dinDC_EVSEStatusType));
            
            pCache->stData.stPowerDeliveryRes.DC_EVSEStatus_isUsed = 1u;
            memcpy(&pCache->stData.stPowerDeliveryRes.DC_EVSEStatus,    pDC_EVSEStatus, sizeof(struct dinDC_EVSEStatusType));
            
            memcpy(&pCache->stData.stCurrentDemandRes.DC_EVSEStatus,    pDC_EVSEStatus, sizeof(struct dinDC_EVSEStatusType));
            memcpy(&pCache->stData.stWeldingDetectionRes.DC_EVSEStatus, pDC_EVSEStatus, sizeof(struct dinDC_EVSEStatusType));
            
            break;
        default:
            break;
        }
        
        return(true);
    }
    
    return(false);
}








/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-25
* notice                                :   ChargeParameterDiscoveryRes.DC_EVSEChargeParameter.EVSEMaximumCurrentLimit
*                                           CurrentDemandRes.EVSEMaximumCurrentLimit
****************************************************************************************************/
bool sV2gDinSet_EVSEMaximumCurrentLimit(i32 i32Index, eV2gDinMsgId_t eId, struct dinPhysicalValueType *pEVSEMaximumCurrentLimit)
{
    bool bRst = false;
    stV2gDinCache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        switch(eId)
        {
        case eV2gDinMsgIdChargeParameterDiscovery:
            if(pEVSEMaximumCurrentLimit != NULL)
            {
                bRst = true;
                memcpy(&pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEMaximumCurrentLimit, pEVSEMaximumCurrentLimit, sizeof(struct dinPhysicalValueType));
                pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter_isUsed = 1u;
            }
            break;
        case eV2gDinMsgIdCurrentDemand:
            bRst = true;
            if(pEVSEMaximumCurrentLimit != NULL)
            {
                memcpy(&pCache->stData.stCurrentDemandRes.EVSEMaximumCurrentLimit, pEVSEMaximumCurrentLimit, sizeof(struct dinPhysicalValueType));
                pCache->stData.stCurrentDemandRes.EVSEMaximumCurrentLimit_isUsed = 1u;
            }
            else
            {
                pCache->stData.stCurrentDemandRes.EVSEMaximumCurrentLimit_isUsed = 0u;
            }
            break;
        case eV2gDinMsgIdMax:
            //定义 eId 为此值时表示把所有相关字段全部赋值，初始化缓存数据字段时 可以一次调用函数初始化全部字段
            if(pEVSEMaximumCurrentLimit != NULL)
            {
                bRst = true;
                pCache->stData.stCurrentDemandRes.EVSEMaximumCurrentLimit_isUsed = 1u;
                pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter_isUsed = 1u;
                memcpy(&pCache->stData.stCurrentDemandRes.EVSEMaximumCurrentLimit,                               pEVSEMaximumCurrentLimit, sizeof(struct dinPhysicalValueType));
                memcpy(&pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEMaximumCurrentLimit, pEVSEMaximumCurrentLimit, sizeof(struct dinPhysicalValueType));
            }
            break;
        default:
            break;
        }
    }
    
    return(bRst);
}







/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-25
* notice                                :   ChargeParameterDiscoveryRes.DC_EVSEChargeParameter.EVSEMaximumPowerLimit
*                                           CurrentDemandRes.EVSEMaximumPowerLimit
****************************************************************************************************/
bool sV2gDinSet_EVSEMaximumPowerLimit(i32 i32Index, eV2gDinMsgId_t eId, struct dinPhysicalValueType *pEVSEMaximumPowerLimit)
{
    stV2gDinCache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        switch(eId)
        {
        case eV2gDinMsgIdChargeParameterDiscovery:
            pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter_isUsed = 1u;
            if(pEVSEMaximumPowerLimit != NULL)
            {
                memcpy(&pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEMaximumPowerLimit, pEVSEMaximumPowerLimit, sizeof(struct dinPhysicalValueType));
                pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEMaximumPowerLimit_isUsed = 1u;
            }
            else
            {
                pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEMaximumPowerLimit_isUsed = 0u;
            }
            break;
        case eV2gDinMsgIdCurrentDemand:
            if(pEVSEMaximumPowerLimit != NULL)
            {
                memcpy(&pCache->stData.stCurrentDemandRes.EVSEMaximumPowerLimit, pEVSEMaximumPowerLimit, sizeof(struct dinPhysicalValueType));
                pCache->stData.stCurrentDemandRes.EVSEMaximumPowerLimit_isUsed = 1u;
            }
            else
            {
                pCache->stData.stCurrentDemandRes.EVSEMaximumPowerLimit_isUsed = 0u;
            }
            break;
        case eV2gDinMsgIdMax:
            //定义 eId 为此值时表示把所有相关字段全部赋值，初始化缓存数据字段时 可以一次调用函数初始化全部字段
            pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter_isUsed = 1u;
            if(pEVSEMaximumPowerLimit != NULL)
            {
                pCache->stData.stCurrentDemandRes.EVSEMaximumPowerLimit_isUsed = 1u;
                pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEMaximumPowerLimit_isUsed = 1u;
                memcpy(&pCache->stData.stCurrentDemandRes.EVSEMaximumPowerLimit,                                pEVSEMaximumPowerLimit, sizeof(struct dinPhysicalValueType));
                memcpy(&pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEMaximumPowerLimit,  pEVSEMaximumPowerLimit, sizeof(struct dinPhysicalValueType));
            }
            else
            {
                pCache->stData.stCurrentDemandRes.EVSEMaximumPowerLimit_isUsed = 0u;
                pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEMaximumPowerLimit_isUsed = 0u;
            }
            break;
        default:
            break;
        }
        
        return(true);
    }
    
    return(false);
}







/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-25
* notice                                :   ChargeParameterDiscoveryRes.DC_EVSEChargeParameter.EVSEMaximumVoltageLimit
*                                           CurrentDemandRes.EVSEMaximumVoltageLimit
****************************************************************************************************/
bool sV2gDinSet_EVSEMaximumVoltageLimit(i32 i32Index, eV2gDinMsgId_t eId, struct dinPhysicalValueType *pEVSEMaximumVoltageLimit)
{
    bool bRst = false;
    stV2gDinCache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        switch(eId)
        {
        case eV2gDinMsgIdChargeParameterDiscovery:
            if(pEVSEMaximumVoltageLimit != NULL)
            {
                bRst = true;
                memcpy(&pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEMaximumVoltageLimit, pEVSEMaximumVoltageLimit, sizeof(struct dinPhysicalValueType));
                pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter_isUsed = 1u;
            }
            break;
        case eV2gDinMsgIdCurrentDemand:
            bRst = true;
            if(pEVSEMaximumVoltageLimit != NULL)
            {
                memcpy(&pCache->stData.stCurrentDemandRes.EVSEMaximumVoltageLimit, pEVSEMaximumVoltageLimit, sizeof(struct dinPhysicalValueType));
                pCache->stData.stCurrentDemandRes.EVSEMaximumVoltageLimit_isUsed = 1u;
            }
            else
            {
                pCache->stData.stCurrentDemandRes.EVSEMaximumVoltageLimit_isUsed = 0u;
            }
            break;
        case eV2gDinMsgIdMax:
            //定义 eId 为此值时表示把所有相关字段全部赋值，初始化缓存数据字段时 可以一次调用函数初始化全部字段
            if(pEVSEMaximumVoltageLimit != NULL)
            {
                bRst = true;
                pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter_isUsed = 1u;
                pCache->stData.stCurrentDemandRes.EVSEMaximumVoltageLimit_isUsed = 1u;
                memcpy(&pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEMaximumVoltageLimit,    pEVSEMaximumVoltageLimit, sizeof(struct dinPhysicalValueType));
                memcpy(&pCache->stData.stCurrentDemandRes.EVSEMaximumVoltageLimit,                                  pEVSEMaximumVoltageLimit, sizeof(struct dinPhysicalValueType));
            }
            break;
        default:
            break;
        }
    }
    
    return(bRst);
}






/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-25
* notice                                :   
****************************************************************************************************/
bool sV2gDinGet_EVTargetVoltage(i32 i32Index, struct dinPhysicalValueType *pEVTargetVoltage)
{
    stV2gDinCache_t *pCache = NULL;
    
    
    if((i32Index < cMse102xDevNum) && (pEVTargetVoltage != NULL) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        memcpy(pEVTargetVoltage, &pCache->stDataPublic.EVTargetVoltage, sizeof(struct dinPhysicalValueType));
            
        return(true);
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-25
* notice                                :   
****************************************************************************************************/
bool sV2gDinGet_EVTargetCurrent(i32 i32Index, struct dinPhysicalValueType *pEVTargetCurrent)
{
    stV2gDinCache_t *pCache = NULL;
    
    
    if((i32Index < cMse102xDevNum) && (pEVTargetCurrent != NULL) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        memcpy(pEVTargetCurrent, &pCache->stDataPublic.EVTargetCurrent, sizeof(struct dinPhysicalValueType));
            
        return(true);
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-25
* notice                                :   
*                                           stCurrentDemandRes.EVSEPresentVoltage
*                                           stWeldingDetectionRes.EVSEPresentVoltage
****************************************************************************************************/
bool sV2gDinSet_EVSEPresentVoltage(i32 i32Index, eV2gDinMsgId_t eId, struct dinPhysicalValueType *pEVSEPresentVoltage)
{
    stV2gDinCache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pEVSEPresentVoltage != NULL) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        switch(eId)
        {
        case eV2gDinMsgIdPreCharge:
            memcpy(&pCache->stData.stPreChargeRes.EVSEPresentVoltage,        pEVSEPresentVoltage, sizeof(struct dinPhysicalValueType));
            break;
        case eV2gDinMsgIdCurrentDemand:
            memcpy(&pCache->stData.stCurrentDemandRes.EVSEPresentVoltage,    pEVSEPresentVoltage, sizeof(struct dinPhysicalValueType));
            break;
        case eV2gDinMsgIdWeldingDetection:
            memcpy(&pCache->stData.stWeldingDetectionRes.EVSEPresentVoltage, pEVSEPresentVoltage, sizeof(struct dinPhysicalValueType));
            break;
        case eV2gDinMsgIdMax:
            memcpy(&pCache->stData.stPreChargeRes.EVSEPresentVoltage,        pEVSEPresentVoltage, sizeof(struct dinPhysicalValueType));
            memcpy(&pCache->stData.stCurrentDemandRes.EVSEPresentVoltage,    pEVSEPresentVoltage, sizeof(struct dinPhysicalValueType));
            memcpy(&pCache->stData.stWeldingDetectionRes.EVSEPresentVoltage, pEVSEPresentVoltage, sizeof(struct dinPhysicalValueType));
            break;
        default:
            break;
        }
        
        return(true);
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-25
* notice                                :   
****************************************************************************************************/
bool sV2gDinGet_BulkChargingComplete(i32 i32Index, i32 *pBulkChargingComplete)
{
    stV2gDinCache_t *pCache = NULL;
    
    
    if((i32Index < cMse102xDevNum) && (pBulkChargingComplete != NULL) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        if(pCache->stDataPublic.BulkChargingComplete_isUsed == 1u)
        {
            (*pBulkChargingComplete) = pCache->stDataPublic.BulkChargingComplete;
            
            return(true);
        }
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-25
* notice                                :   
****************************************************************************************************/
bool sV2gDinGet_ChargingComplete(i32 i32Index, i32 *pChargingComplete)
{
    stV2gDinCache_t *pCache = NULL;
    
    
    if((i32Index < cMse102xDevNum) && (pChargingComplete != NULL) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        (*pChargingComplete) = pCache->stDataPublic.ChargingComplete;
        
        return(true);
    }
    
    return(false);
}









/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-02-26
* notice                                :   握手信号 xSemHandshake
****************************************************************************************************/
bool sV2gDinGet_SupportedAppProtocol(i32 i32Index)
{
    bool bRst = pdFALSE;
    stV2gDinCache_t *pCache = NULL;
    
    
    if((i32Index < cMse102xDevNum) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        bRst = xSemaphoreTake(pCache->xSemHandshake, 0);
    }
    
    return((bRst == pdTRUE) ? true : false);
}












/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-24
* notice                                :   SessionSetup Req.EVCCID 字段
****************************************************************************************************/
bool sV2gDinGet_EVCCID(i32 i32Index, i32 i32MaxLen, u8 *pEvccId)
{
    i32  i32Size;
    stV2gDinCache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pEvccId != NULL) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        i32Size = pCache->stData.stSessionSetupReq.EVCCID.bytesLen;
        if(i32Size <= i32MaxLen)
        {
            //EVCCID.bytes 是 u8 类型  可以这样拷贝
            memcpy(pEvccId, pCache->stData.stSessionSetupReq.EVCCID.bytes, i32Size);
            
            return(true);
        }
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-24
* notice                                :   SessionSetup Res.EVSEID 字段
****************************************************************************************************/
bool sV2gDinSet_EVSEID(i32 i32Index, i32 i32Len, const u8 *pEvseId)
{
    i32  i32Size;
    stV2gDinCache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        i32Size = sizeof(pCache->stData.stSessionSetupRes.EVSEID.bytes);
        if(i32Size >= i32Len)
        {
            //EVSEID.bytes 是 u8 类型  可以这样拷贝
            pCache->stData.stSessionSetupRes.EVSEID.bytesLen = i32Len;
            memset(pCache->stData.stSessionSetupRes.EVSEID.bytes, 0, i32Size);
            if(pEvseId != NULL)
            {
                memcpy(pCache->stData.stSessionSetupRes.EVSEID.bytes, pEvseId, i32Len);
            }
            
            return(true);
        }
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-24
* notice                                :   ServiceDiscovery Req.ServiceScope 字段
*
*                                           pServiceScope 是32位的Unicode字符串 
****************************************************************************************************/
bool sV2gDinGet_ServiceScope(i32 i32Index, i32 i32MaxLen, i32 *pServiceScope)
{
    i32  i32Size;
    stV2gDinCache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pServiceScope != NULL) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        i32Size = pCache->stData.stServiceDiscoveryReq.ServiceScope.charactersLen * sizeof(exi_string_character_t);
        if((pCache->stData.stServiceDiscoveryReq.ServiceScope_isUsed == 1u) && (i32Size <= i32MaxLen))
        {
            //i32Size 已经考虑了 characters 的宽度为32位的情景
            memcpy(pServiceScope, pCache->stData.stServiceDiscoveryReq.ServiceScope.characters, i32Size);
            
            return(true);
        }
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-24
* notice                                :   ServiceDiscovery Req.ServiceCategory 字段
****************************************************************************************************/
bool sV2gDinGet_ServiceCategory(i32 i32Index, i32 *pServiceCategory)
{
    stV2gDinCache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pServiceCategory != NULL) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        if(pCache->stData.stServiceDiscoveryReq.ServiceCategory_isUsed == 1u)
        {
            (*pServiceCategory) = pCache->stData.stServiceDiscoveryReq.ServiceCategory;
            
            return(true);
        }
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-24
* notice                                :   ServiceDiscovery Res.PaymentOptions 字段
****************************************************************************************************/
bool sV2gDinSet_PaymentOptions(i32 i32Index, i32 i32PaymentOptionNum, const dinpaymentOptionType *pPaymentOption)
{
    i32  i, i32Size;
    stV2gDinCache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pPaymentOption != NULL) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        i32Size = sizeof(pCache->stData.stServiceDiscoveryRes.PaymentOptions.PaymentOption.array);
        if(i32PaymentOptionNum <= dinPaymentOptionsType_PaymentOption_ARRAY_SIZE)
        {
            pCache->stData.stServiceDiscoveryRes.PaymentOptions.PaymentOption.arrayLen = i32PaymentOptionNum;
            memset(pCache->stData.stServiceDiscoveryRes.PaymentOptions.PaymentOption.array, 0, i32Size);
            
            for(i = 0; i < i32PaymentOptionNum; i++)
            {
                pCache->stData.stServiceDiscoveryRes.PaymentOptions.PaymentOption.array[i] = pPaymentOption[i];
            }
            
            return(true);
        }
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-24
* notice                                :   ServiceDiscovery Res.ChargeService 字段
*
*                                           i32Index            :设备序号
*                                           u16Id               :ChargeService.ServiceTag.ServiceID
*                                           pName+i32NameLen    :ChargeService.ServiceTag.ServiceName
*                                           i32Category         :ChargeService.ServiceTag.ServiceCategory
*                                           pScope+i32ScopeLen  :ChargeService.ServiceTag.ServiceScope
*                                           bFree               :ChargeService.FreeService
*                                           i32EnergyTsfType    :ChargeService.EnergyTransferType
*
*                                           pName和pScope 是32位字符串, 支持Unicode编码
****************************************************************************************************/
bool sV2gDinSet_ChargeService(i32 i32Index, u16 u16Id, i32 *pName, i32 i32NameLen, i32 i32Category, i32 *pScope, i32 i32ScopeLen, bool bFree, i32 i32EnergyTsfType)
{
    i32  i, i32Size;
    stV2gDinCache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        pCache->stData.stServiceDiscoveryRes.ChargeService.ServiceTag.ServiceID = u16Id;
        
        i32Size = sizeof(pCache->stData.stServiceDiscoveryRes.ChargeService.ServiceTag.ServiceName.characters);
        if((pName != NULL) && (i32NameLen <= dinServiceTagType_ServiceName_CHARACTERS_SIZE))
        {
            pCache->stData.stServiceDiscoveryRes.ChargeService.ServiceTag.ServiceName.charactersLen = i32NameLen;
            memset(pCache->stData.stServiceDiscoveryRes.ChargeService.ServiceTag.ServiceName.characters, 0, i32Size);
            for(i = 0; i < i32NameLen; i++)
            {
                pCache->stData.stServiceDiscoveryRes.ChargeService.ServiceTag.ServiceName.characters[i] = pName[i];
            }
        }
        
        pCache->stData.stServiceDiscoveryRes.ChargeService.ServiceTag.ServiceCategory = i32Category;
        
        i32Size = sizeof(pCache->stData.stServiceDiscoveryRes.ChargeService.ServiceTag.ServiceName.characters);
        if((pScope != NULL) && (i32ScopeLen <= dinServiceTagType_ServiceScope_CHARACTERS_SIZE))
        {
            pCache->stData.stServiceDiscoveryRes.ChargeService.ServiceTag.ServiceScope.charactersLen = i32ScopeLen;
            memset(pCache->stData.stServiceDiscoveryRes.ChargeService.ServiceTag.ServiceScope.characters, 0, i32Size);
            for(i = 0; i < i32ScopeLen; i++)
            {
                pCache->stData.stServiceDiscoveryRes.ChargeService.ServiceTag.ServiceScope.characters[i] = pScope[i];
            }
        }
        
        pCache->stData.stServiceDiscoveryRes.ChargeService.FreeService = bFree;
        pCache->stData.stServiceDiscoveryRes.ChargeService.EnergyTransferType = i32EnergyTsfType;
        
        return(true);
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-24
* notice                                :   ServicePaymentSelection Req.SelectedPaymentOption 字段
****************************************************************************************************/
bool sV2gDinGet_SelectedPaymentOption(i32 i32Index, i32 *pSelectedPaymentOption)
{
    stV2gDinCache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pSelectedPaymentOption != NULL) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        (*pSelectedPaymentOption) = pCache->stData.stServicePaymentSelectionReq.SelectedPaymentOption;
        
        return(true);
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-24
* notice                                :   stChargeParamDiscoveryReq.EVRequestedEnergyTransferType 字段
****************************************************************************************************/
bool sV2gDinGet_EVRequestedEnergyTransferType(i32 i32Index, i32 *pEvRequestedEnergyTransferType)
{
    stV2gDinCache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pEvRequestedEnergyTransferType != NULL) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        (*pEvRequestedEnergyTransferType) = pCache->stData.stChargeParamDiscoveryReq.EVRequestedEnergyTransferType;
        
        return(true);
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-25
* notice                                :   stChargeParamDiscoveryReq.DC_EVChargeParameter.EVEnergyCapacity
****************************************************************************************************/
bool sV2gDinGet_EVEnergyCapacity(i32 i32Index, struct dinPhysicalValueType *pEVEnergyCapacity)
{
    stV2gDinCache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pEVEnergyCapacity != NULL) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        if(pCache->eId >= eV2gDinMsgIdChargeParameterDiscovery)
        {
            if((pCache->stData.stChargeParamDiscoveryReq.DC_EVChargeParameter_isUsed == 1u)
            && (pCache->stData.stChargeParamDiscoveryReq.DC_EVChargeParameter.EVEnergyCapacity_isUsed == 1u))
            {
                memcpy(pEVEnergyCapacity, &pCache->stData.stChargeParamDiscoveryReq.DC_EVChargeParameter.EVEnergyCapacity, sizeof(struct dinPhysicalValueType));
                
                return(true);
            }
        }
        
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-25
* notice                                :   stChargeParamDiscoveryReq.DC_EVChargeParameter.EVEnergyRequest
****************************************************************************************************/
bool sV2gDinGet_EVEnergyRequest(i32 i32Index, struct dinPhysicalValueType *pEVEnergyRequest)
{
    stV2gDinCache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pEVEnergyRequest != NULL) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        if(pCache->eId >= eV2gDinMsgIdChargeParameterDiscovery)
        {
            if((pCache->stData.stChargeParamDiscoveryReq.DC_EVChargeParameter_isUsed == 1u)
            && (pCache->stData.stChargeParamDiscoveryReq.DC_EVChargeParameter.EVEnergyRequest_isUsed == 1u))
            {
                memcpy(pEVEnergyRequest, &pCache->stData.stChargeParamDiscoveryReq.DC_EVChargeParameter.EVEnergyRequest, sizeof(struct dinPhysicalValueType));
                
                return(true);
            }
        }
        
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-25
* notice                                :   stChargeParamDiscoveryReq.DC_EVChargeParameter.FullSOC
****************************************************************************************************/
bool sV2gDinGet_FullSOC(i32 i32Index, i8 *pFullSOC)
{
    stV2gDinCache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pFullSOC != NULL) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        if(pCache->eId >= eV2gDinMsgIdChargeParameterDiscovery)
        {
            if((pCache->stData.stChargeParamDiscoveryReq.DC_EVChargeParameter_isUsed == 1u)
            && (pCache->stData.stChargeParamDiscoveryReq.DC_EVChargeParameter.FullSOC_isUsed == 1u))
            {
                (*pFullSOC) = pCache->stData.stChargeParamDiscoveryReq.DC_EVChargeParameter.FullSOC;
                
                return(true);
            }
        }
        
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-25
* notice                                :   stChargeParamDiscoveryReq.DC_EVChargeParameter.BulkSOC
****************************************************************************************************/
bool sV2gDinGet_BulkSOC(i32 i32Index, i8 *pBulkSOC)
{
    stV2gDinCache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pBulkSOC != NULL) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        if(pCache->eId >= eV2gDinMsgIdChargeParameterDiscovery)
        {
            if((pCache->stData.stChargeParamDiscoveryReq.DC_EVChargeParameter_isUsed == 1u)
            && (pCache->stData.stChargeParamDiscoveryReq.DC_EVChargeParameter.BulkSOC_isUsed == 1u))
            {
                (*pBulkSOC) = pCache->stData.stChargeParamDiscoveryReq.DC_EVChargeParameter.BulkSOC;
                
                return(true);
            }
        }
        
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-25
* notice                                :   stChargeParamDiscoveryRes.SAScheduleList 字段
*
*                                           struct dinSAScheduleListType    :11624字节
*                                           struct dinSAScheduleTupleType   :2324 字节
*                                           可以看到, SAScheduleList字段 高达11624字节，为节省ram
*                                           函数不直接传递完整的SAScheduleList字段，而是传递其成员
*                                           SAScheduleTuple 只有2324字节，可以接受
*
*                                           bClearFlag:如果为true，则在赋值前先清除 SAScheduleList
****************************************************************************************************/
bool sV2gDinSet_SAScheduleList(i32 i32Index, bool bClearFlag, struct dinSAScheduleTupleType *pSAScheduleTuple)
{
    i32  i32ArrayIndex;
    stV2gDinCache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        if(bClearFlag == true)
        {
            //本函数逻辑相当于对SAScheduleList字段的 SAScheduleTuple[n]元素 做了一个增加接口函数
            //但是删除函数很难实现，因为移动 SAScheduleTuple[n] 的逻辑很麻烦, 
            //为了程序的简洁，就增加了 bClearFlag 标志 可以清除所有的 SAScheduleTuple[] 元素，不必实现删除逻辑
            memset(&pCache->stData.stChargeParamDiscoveryRes.SAScheduleList, 0, sizeof(struct dinSAScheduleListType));
        }
        
        if(pSAScheduleTuple != NULL)
        {
            i32ArrayIndex = pCache->stData.stChargeParamDiscoveryRes.SAScheduleList.SAScheduleTuple.arrayLen;
            if(i32ArrayIndex < (dinSAScheduleListType_SAScheduleTuple_ARRAY_SIZE - 1))
            {
                memcpy(&pCache->stData.stChargeParamDiscoveryRes.SAScheduleList.SAScheduleTuple.array[i32ArrayIndex], pSAScheduleTuple, sizeof(struct dinSAScheduleTupleType));
                pCache->stData.stChargeParamDiscoveryRes.SAScheduleList.SAScheduleTuple.arrayLen++;
            }
            pCache->stData.stChargeParamDiscoveryRes.SAScheduleList_isUsed = 1u;
        }
        
        return(true);
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-25
* notice                                :   stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEMinimumCurrentLimit 字段
****************************************************************************************************/
bool sV2gDinSet_EVSEMinimumCurrentLimit(i32 i32Index, struct dinPhysicalValueType *pEVSEMinimumCurrentLimit)
{
    stV2gDinCache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pEVSEMinimumCurrentLimit != NULL) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        memcpy(&pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEMinimumCurrentLimit, pEVSEMinimumCurrentLimit, sizeof(struct dinPhysicalValueType));
        pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter_isUsed = 1u;
        
        return(true);
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-25
* notice                                :   stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEMinimumVoltageLimit 字段
****************************************************************************************************/
bool sV2gDinSet_EVSEMinimumVoltageLimit(i32 i32Index, struct dinPhysicalValueType *pEVSEMinimumVoltageLimit)
{
    stV2gDinCache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pEVSEMinimumVoltageLimit != NULL) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        memcpy(&pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEMinimumVoltageLimit, pEVSEMinimumVoltageLimit, sizeof(struct dinPhysicalValueType));
        pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter_isUsed = 1u;
        
        return(true);
    }
    
    return(false);
}







/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-25
* notice                                :   stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSECurrentRegulationTolerance 字段
****************************************************************************************************/
bool sV2gDinSet_EVSECurrentRegulationTolerance(i32 i32Index, struct dinPhysicalValueType *pEVSECurrentRegulationTolerance)
{
    stV2gDinCache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter_isUsed = 1u;
        if(pEVSECurrentRegulationTolerance != NULL)
        {
            memcpy(&pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSECurrentRegulationTolerance, pEVSECurrentRegulationTolerance, sizeof(struct dinPhysicalValueType));
            pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSECurrentRegulationTolerance_isUsed = 1u;
        }
        else
        {
            pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSECurrentRegulationTolerance_isUsed = 0u;
        }
        
        return(true);
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-25
* notice                                :   stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEPeakCurrentRipple 字段
****************************************************************************************************/
bool sV2gDinSet_EVSEPeakCurrentRipple(i32 i32Index, struct dinPhysicalValueType *pEVSEPeakCurrentRipple)
{
    stV2gDinCache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pEVSEPeakCurrentRipple != NULL) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        memcpy(&pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEPeakCurrentRipple, pEVSEPeakCurrentRipple, sizeof(struct dinPhysicalValueType));
        pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter_isUsed = 1u;
        
        return(true);
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-25
* notice                                :   stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEEnergyToBeDelivered 字段
****************************************************************************************************/
bool sV2gDinSet_EVSEEnergyToBeDelivered(i32 i32Index, struct dinPhysicalValueType *pEVSEEnergyToBeDelivered)
{
    stV2gDinCache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter_isUsed = 1u;
        if(pEVSEEnergyToBeDelivered != NULL)
        {
            memcpy(&pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEEnergyToBeDelivered, pEVSEEnergyToBeDelivered, sizeof(struct dinPhysicalValueType));
            pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEEnergyToBeDelivered_isUsed = 1u;
        }
        else
        {
            pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEEnergyToBeDelivered_isUsed = 0u;
        }
        
        return(true);
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-25
* notice                                :   stPowerDeliveryReq.ReadyToChargeState
****************************************************************************************************/
bool sV2gDinGet_ReadyToChargeState(i32 i32Index, i32 *pReadyToChargeState)
{
    stV2gDinCache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pReadyToChargeState != NULL) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        if(pCache->eId >= eV2gDinMsgIdPowerDelivery1)
        {
            (*pReadyToChargeState) = pCache->stData.stPowerDeliveryReq.ReadyToChargeState;
            
            return(true);
        }
        
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-25
* notice                                :   stPowerDeliveryReq.ReadyToChargeState
****************************************************************************************************/
bool sV2gDinGet_ChargingProfile(i32 i32Index, struct dinChargingProfileType *pChargingProfile)
{
    stV2gDinCache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pChargingProfile != NULL) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        if(pCache->eId >= eV2gDinMsgIdPowerDelivery1)
        {
            if(pCache->stData.stPowerDeliveryReq.ChargingProfile_isUsed == 1u)
            {
                memcpy(pChargingProfile, &pCache->stData.stPowerDeliveryReq.ChargingProfile, sizeof(struct dinChargingProfileType));
                
                return(true);
            }
        }
        
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-25
* notice                                :   stCurrentDemandReq.RemainingTimeToFullSoC
****************************************************************************************************/
bool sV2gDinGet_RemainingTimeToFullSoC(i32 i32Index, struct dinPhysicalValueType *pRemainingTimeToFullSoC)
{
    stV2gDinCache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pRemainingTimeToFullSoC != NULL) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        if(pCache->stData.stCurrentDemandReq.RemainingTimeToFullSoC_isUsed == 1u)
        {
            memcpy(pRemainingTimeToFullSoC, &pCache->stData.stCurrentDemandReq.RemainingTimeToFullSoC, sizeof(struct dinPhysicalValueType));
            
            return(true);
        }
        
    }
    
    return(false);
}







/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-25
* notice                                :   stCurrentDemandReq.RemainingTimeToBulkSoC
****************************************************************************************************/
bool sV2gDinGet_RemainingTimeToBulkSoC(i32 i32Index, struct dinPhysicalValueType *pRemainingTimeToBulkSoC)
{
    stV2gDinCache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pRemainingTimeToBulkSoC != NULL) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        if(pCache->stData.stCurrentDemandReq.RemainingTimeToBulkSoC_isUsed == 1u)
        {
            memcpy(pRemainingTimeToBulkSoC, &pCache->stData.stCurrentDemandReq.RemainingTimeToBulkSoC, sizeof(struct dinPhysicalValueType));
            
            return(true);
        }
        
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   DIN70121 协议 可读可写数据的 读写api接口函数
* Author                                :   Hall
* Creat Date                            :   2024-01-25
* notice                                :   stCurrentDemandRes.EVSEPresentCurrent
****************************************************************************************************/
bool sV2gDinSet_EVSEPresentCurrent(i32 i32Index, struct dinPhysicalValueType *pEVSEPresentCurrent)
{
    stV2gDinCache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pEVSEPresentCurrent != NULL) && (pV2gDinCache[i32Index] != NULL))
    {
        pCache = pV2gDinCache[i32Index];
        memcpy(&pCache->stData.stCurrentDemandRes.EVSEPresentCurrent, pEVSEPresentCurrent, sizeof(struct dinPhysicalValueType));
        
        return(true);
    }
    
    return(false);
}


































































