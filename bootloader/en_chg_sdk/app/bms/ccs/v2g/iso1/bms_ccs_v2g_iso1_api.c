/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     bms_ccs_v2g_iso1_api.c
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2024-3-19
 * @Attention             :     
 * @Brief                 :     ISO15118 协议实现 之 对外的api接口实现
 * 
 * @History:
 * 
 * 1.@Date: 2024-3-19
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#include "bms_ccs_v2g_iso1_api.h"








//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "bms_ccs_v2g_iso1_api";







extern stV2gIso1Cache_t *pV2gIso1Cache[cMse102xDevNum];



//v2g消息 msgId 字段读取
bool sV2gIso1Get_MsgId(i32 i32Index, eV2gIso1MsgId_t *pId);

//多消息公用的api函数
bool sV2gIso1Set_ResponseCode(i32 i32Index, eV2gIso1MsgId_t eId, i32 i32ResponseCode);
bool sV2gIso1Set_EVSEID(i32 i32Index, eV2gIso1MsgId_t eId, i32 i32Len, const i32 *pEvseId);
bool sV2gIso1Set_EVSEProcessing(i32 i32Index, eV2gIso1MsgId_t eId, i32 i32EvseProcessing);
bool sV2gIso1Get_ServiceId(i32 i32Index, i32 *pServiceId);
bool sV2gIso1Get_DC_EVStatus(i32 i32Index, struct iso1DC_EVStatusType *pDC_EVStatus);
bool sV2gIso1Get_EVMaximumCurrentLimit(i32 i32Index, struct iso1PhysicalValueType *pEVMaximumCurrentLimit);
bool sV2gIso1Get_EVMaximumPowerLimit(i32 i32Index, struct iso1PhysicalValueType *pEVMaximumPowerLimit);
bool sV2gIso1Get_EVMaximumVoltageLimit(i32 i32Index, struct iso1PhysicalValueType *pEVMaximumVoltageLimit);
bool sV2gIso1Set_AC_EVSEStatus(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1AC_EVSEStatusType *pAC_EVSEStatus);
bool sV2gIso1Set_EVSEMaxCurrent(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1PhysicalValueType *pEVSEMaxCurrent);
bool sV2gIso1Set_DC_EVSEStatus(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1DC_EVSEStatusType *pDC_EVSEStatus);
bool sV2gIso1Set_EVSEMaximumCurrentLimit(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1PhysicalValueType *pEVSEMaximumCurrentLimit);
bool sV2gIso1Set_EVSEMaximumPowerLimit(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1PhysicalValueType *pEVSEMaximumPowerLimit);
bool sV2gIso1Set_EVSEMaximumVoltageLimit(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1PhysicalValueType *pEVSEMaximumVoltageLimit);
bool sV2gIso1Get_EVTargetVoltage(i32 i32Index, struct iso1PhysicalValueType *pEVTargetVoltage);
bool sV2gIso1Get_EVTargetCurrent(i32 i32Index, struct iso1PhysicalValueType *pEVTargetCurrent);
bool sV2gIso1Set_EVSEPresentVoltage(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1PhysicalValueType *pEVSEPresentVoltage);
bool sV2gIso1Get_BulkChargingComplete(i32 i32Index, i32 *pBulkChargingComplete);
bool sV2gIso1Get_ChargingComplete(i32 i32Index, i32 *pChargingComplete);
bool sV2gIso1Get_Id(i32 i32Index, eV2gIso1MsgId_t eId, i32 *pLen, i32 *pId);
bool sV2gIso1Get_ListOfRootCertificateIDs(i32 i32Index, eV2gIso1MsgId_t eId, struct iso1ListOfRootCertificateIDsType *pListOfRootCertificateIDs);
bool sV2gIso1Get_ContractSignatureCertChain(i32 i32Index, eV2gIso1MsgId_t eId, struct iso1CertificateChainType *pSubCertificateChain);
bool sV2gIso1Get_eMAID(i32 i32Index, eV2gIso1MsgId_t eId, i32 *pEMAIDLen, i32 *pEMAID);
bool sV2gIso1Set_SAProvisioningCertificateChain(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1CertificateChainType *pCertificateChain);
bool sV2gIso1Set_ContractSignatureCertChain(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1CertificateChainType *pCertificateChain);
bool sV2gIso1Set_ContractSignatureEncryptedPrivateKey(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1ContractSignatureEncryptedPrivateKeyType* pContractSignatureEncryptedPrivateKey);
bool sV2gIso1Set_DHpublickey(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1DiffieHellmanPublickeyType* pDiffieHellmanPublickey);
bool sV2gIso1Set_eMAID(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1EMAIDType* pEMAID);
bool sV2gIso1Set_MeterInfo(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1MeterInfoType* pMeterInfo);
bool sV2gIso1Set_ReceiptRequired(i32 i32Index, eV2gIso1MsgId_t eId, bool bReceiptRequired);



//SupportedAppProtocol
bool sV2gIso1Get_SupportedAppProtocol(i32 i32Index);

//SessionSetup
bool sV2gIso1Get_EVCCID(i32 i32Index, i32 *pLen, u8 *pEvccId);

//ServiceDiscovery
bool sV2gIso1Get_ServiceScope(i32 i32Index, i32 *pScopeLen, i32 *pServiceScope);
bool sV2gIso1Get_ServiceCategory(i32 i32Index, i32 *pServiceCategory);
bool sV2gIso1Set_PaymentOptionList(i32 i32Index, i32 i32PaymentOptionNum, const iso1paymentOptionType *pPaymentOption);
bool sV2gIso1Set_ChargeService(i32 i32Index, u16 u16ServiceId, const i32 *pName, i32 i32NameLen, i32 i32Category, const i32 *pScope, i32 i32ScopeLen, bool bFree, const u8 *pEnergyTsfMode, u8 u8ModeNum);
bool sV2gIso1Set_ServiceList(i32 i32Index, i32 i32ServiceNum, const struct iso1ServiceType *pService);

//ServiceDetail
bool sV2gIso1Set_ServiceParameterList(i32 i32Index, i32 i32ParameterSetNum, const struct iso1ParameterSetType *pParameterSet);

//PaymentServiceSelection
bool sV2gIso1Get_SelectedPaymentOption(i32 i32Index, i32 *pSelectedPaymentOption);

//CertificateInstallation
bool sV2gIso1Get_OEMProvisioningCert(i32 i32Index, i32 *pLen, u8 *pProvisioningCert);

//CertificateUpdate
bool sV2gIso1Set_RetryCounter(i32 i32Index, i16 i16RetryCounter);

//PaymentDetails
bool sV2gIso1Set_GenChallenge(i32 i32Index, i32 i32Len, const u8* pGenChallenge);

//ContractAuthentication---无

//ChargeParameterDiscovery
bool sV2gIso1Get_RequestedEnergyTransferMode(i32 i32Index, i32 *pRequestedEnergyTransferMode);
bool sV2gIso1Get_MaxEntriesSAScheduleTuple(i32 i32Index, i32 *pMaxEntriesSAScheduleTuple);
bool sV2gIso1Get_DepartureTime(i32 i32Index, i32 *pDepartureTime);
bool sV2gIso1Get_EAmount(i32 i32Index, struct iso1PhysicalValueType *pEAmount);
bool sV2gIso1Get_EVMaxVoltage(i32 i32Index, struct iso1PhysicalValueType *pEVMaxVoltage);
bool sV2gIso1Get_EVMaxCurrent(i32 i32Index, struct iso1PhysicalValueType *pEVMaxCurrent);
bool sV2gIso1Get_EVMinCurrent(i32 i32Index, struct iso1PhysicalValueType *pEVMinCurrent);
bool sV2gIso1Get_EVEnergyCapacity(i32 i32Index, struct iso1PhysicalValueType *pEVEnergyCapacity);
bool sV2gIso1Get_EVEnergyRequest(i32 i32Index, struct iso1PhysicalValueType *pEVEnergyRequest);
bool sV2gIso1Get_FullSOC(i32 i32Index, i8 *pFullSOC);
bool sV2gIso1Get_BulkSOC(i32 i32Index, i8 *pBulkSOC);
bool sV2gIso1Set_SAScheduleList(i32 i32Index, bool bClearFlag, const struct iso1SAScheduleTupleType *pSAScheduleTuple);
bool sV2gIso1Set_EVSENominalVoltage(i32 i32Index, const struct iso1PhysicalValueType *pEVSENominalVoltage);
bool sV2gIso1Set_EVSEMinimumCurrentLimit(i32 i32Index, const struct iso1PhysicalValueType *pEVSEMinimumCurrentLimit);
bool sV2gIso1Set_EVSEMinimumVoltageLimit(i32 i32Index, const struct iso1PhysicalValueType *pEVSEMinimumVoltageLimit);
bool sV2gIso1Set_EVSECurrentRegulationTolerance(i32 i32Index, const struct iso1PhysicalValueType *pEVSECurrentRegulationTolerance);
bool sV2gIso1Set_EVSEPeakCurrentRipple(i32 i32Index, const struct iso1PhysicalValueType *pEVSEPeakCurrentRipple);
bool sV2gIso1Set_EVSEEnergyToBeDelivered(i32 i32Index, const struct iso1PhysicalValueType *pEVSEEnergyToBeDelivered);

//CableCheck----无,都在公用api里面

//PreCharge-----无,都在公用api里面

//PowerDelivery
bool sV2gIso1Get_ChargeProgress(i32 i32Index, i32 *pReadyToChargeState);
bool sV2gIso1Get_ChargingProfile(i32 i32Index, struct iso1ChargingProfileType *pChargingProfile);

//ChargingStatus----无,都在公用api里面

//CurrentDemand
bool sV2gIso1Get_RemainingTimeToFullSoC(i32 i32Index, struct iso1PhysicalValueType *pRemainingTimeToFullSoC);
bool sV2gIso1Get_RemainingTimeToBulkSoC(i32 i32Index, struct iso1PhysicalValueType *pRemainingTimeToBulkSoC);
bool sV2gIso1Set_EVSEPresentCurrent(i32 i32Index, const struct iso1PhysicalValueType *pEVSEPresentCurrent);
bool sV2gIso1Set_EVSEVoltageLimitAchieved(i32 i32Index, bool bAchievedFlag);
bool sV2gIso1Set_EVSECurrentLimitAchieved(i32 i32Index, bool bAchievedFlag);
bool sV2gIso1Set_EVSEPowerLimitAchieved(i32 i32Index, bool bAchievedFlag);

//MeteringReceipt-----无,都在公用api里面

//WeldingDetection-----无,都在公用api里面

//SessionStop
bool sV2gIso1Get_ChargingSession(i32 i32Index, i8 *pChargingSession);





//-----------------------------------------------------------------------------












/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_MsgId
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月19日  14:46:55
 * @Description    :      ISO15118 协议 获取 eId 字段 api接口函数
 * @Input          :      i32Index          外设号
 * @Input          :      pId               消息ID
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_MsgId(i32 i32Index, eV2gIso1MsgId_t *pId)
{
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pId != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        (*pId) = pCache->eId;
        
        return(true);
    }
    
    return(false);
}














/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_ResponseCode
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月19日  15:37:00
 * @Description    :      ISO15118 协议 设置 ResponseCode 字段 api接口函数
 * @Input          :      i32Index              外设号
 * @Input          :      eId                   消息ID
 * @Input          :      i32ResponseCode       消息回复码
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Set_ResponseCode(i32 i32Index, eV2gIso1MsgId_t eId, i32 i32ResponseCode)
{
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (eId <= eV2gIso1MsgIdMax) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        switch(eId)
        {
            case eV2gIso1MsgIdSessionSetup:
                pCache->stData.stSessionSetupRes.ResponseCode = i32ResponseCode;
                break;
            case eV2gIso1MsgIdServiceDiscovery:
                pCache->stData.stServiceDiscoveryRes.ResponseCode = i32ResponseCode;
                break;
            case eV2gIso1MsgIdServiceDetail:
                pCache->stData.stServiceDetailRes.ResponseCode = i32ResponseCode;
                break;
            case eV2gIso1MsgIdPaymentServiceSelection:
                pCache->stData.stPaymentServiceSelectionRes.ResponseCode = i32ResponseCode;
                break;
            case eV2gIso1MsgIdCertificateInstallation:
                pCache->stData.stCertificateInstallationRes.ResponseCode = i32ResponseCode;
                break;
            case eV2gIso1MsgIdCertificateUpdate:
                pCache->stData.stCertificateUpdateRes.ResponseCode = i32ResponseCode;
                break;
            case eV2gIso1MsgIdAuthorization:
                pCache->stData.stAuthorizationRes.ResponseCode = i32ResponseCode;
                break;
            case eV2gIso1MsgIdChargeParameterDiscovery:
                pCache->stData.stChargeParamDiscoveryRes.ResponseCode = i32ResponseCode;
                break;
            case eV2gIso1MsgIdCableCheck:
                pCache->stData.stCableCheckRes.ResponseCode = i32ResponseCode;
                break;
            case eV2gIso1MsgIdPreCharge:
                pCache->stData.stPreChargeRes.ResponseCode = i32ResponseCode;
                break;
            case eV2gIso1MsgIdPowerDeliveryStart:
            case eV2gIso1MsgIdPowerDeliveryRenegotiate:
            case eV2gIso1MsgIdPowerDeliveryStop:
                pCache->stData.stPowerDeliveryRes.ResponseCode = i32ResponseCode;
                break;
            case eV2gIso1MsgIdChargingStatus:
                pCache->stData.stChargingStatusRes.ResponseCode = i32ResponseCode;
                break;
            case eV2gIso1MsgIdCurrentDemand:
                pCache->stData.stCurrentDemandRes.ResponseCode = i32ResponseCode;
                break;
            case eV2gIso1MsgIdMeteringReceipt:
                pCache->stData.stMeteringReceiptRes.ResponseCode = i32ResponseCode;
                break;
            case eV2gIso1MsgIdWeldingDetection:
                pCache->stData.stWeldingDetectionRes.ResponseCode = i32ResponseCode;
                break;
            case eV2gIso1MsgIdSessionStop:
                pCache->stData.stSessionStopRes.ResponseCode = i32ResponseCode;
                break;
            default:
                break;
        }
        
        return(true);
    }
    
    return(false);
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_EVSEID
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月19日  20:46:12
 * @Description    :      ISO15118 协议 设置 EVSEID 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      eId                       消息ID
 * @Input          :      i32Len                    EVSEID长度
 * @Input          :      pEvseId                   指向要写入的EVSEID的i32指针(32位字符串, 支持Unicode编码)
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Set_EVSEID(i32 i32Index, eV2gIso1MsgId_t eId, i32 i32Len, const i32 *pEvseId)
{
    bool bRst = false;
    i32  i, i32Size;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (eId <= eV2gIso1MsgIdMax) && (i32Len > 0) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        switch(eId)
        {
            case eV2gIso1MsgIdSessionSetup:
                i32Size = sizeof(pCache->stData.stSessionSetupRes.EVSEID.characters);
                memset(pCache->stData.stSessionSetupRes.EVSEID.characters, 0, i32Size);
                pCache->stData.stSessionSetupRes.EVSEID.charactersLen = i32Len;
                for(i = 0; i < i32Len; i++)
                {
                    pCache->stData.stSessionSetupRes.EVSEID.characters[i] = pEvseId[i];
                }
                break;
                
            case eV2gIso1MsgIdChargingStatus:
                i32Size = sizeof(pCache->stData.stChargingStatusRes.EVSEID.characters);
                memset(pCache->stData.stChargingStatusRes.EVSEID.characters, 0, i32Size);
                pCache->stData.stChargingStatusRes.EVSEID.charactersLen = i32Len;
                for(i = 0; i < i32Len; i++)
                {
                    pCache->stData.stChargingStatusRes.EVSEID.characters[i] = pEvseId[i];
                }
                break;
                
            case eV2gIso1MsgIdCurrentDemand:
                i32Size = sizeof(pCache->stData.stCurrentDemandRes.EVSEID.characters);
                memset(pCache->stData.stCurrentDemandRes.EVSEID.characters, 0, i32Size);
                pCache->stData.stCurrentDemandRes.EVSEID.charactersLen = i32Len;
                for(i = 0; i < i32Len; i++)
                {
                    pCache->stData.stCurrentDemandRes.EVSEID.characters[i] = pEvseId[i];
                }
                break;
                
            case eV2gIso1MsgIdMax:
                i32Size = sizeof(pCache->stData.stSessionSetupRes.EVSEID.characters);
                memset(pCache->stData.stSessionSetupRes.EVSEID.characters, 0, i32Size);
                memset(pCache->stData.stChargingStatusRes.EVSEID.characters, 0, i32Size);
                memset(pCache->stData.stCurrentDemandRes.EVSEID.characters, 0, i32Size);
                pCache->stData.stSessionSetupRes.EVSEID.charactersLen = i32Len;
                pCache->stData.stChargingStatusRes.EVSEID.charactersLen = i32Len;
                pCache->stData.stCurrentDemandRes.EVSEID.charactersLen = i32Len;
                for(i = 0; i < i32Len; i++)
                {
                    pCache->stData.stSessionSetupRes.EVSEID.characters[i] = pEvseId[i];
                    pCache->stData.stChargingStatusRes.EVSEID.characters[i] = pEvseId[i];
                    pCache->stData.stCurrentDemandRes.EVSEID.characters[i] = pEvseId[i];
                }
                break;
                
            default:
                break;
        }
        bRst = true;
    }
    
    return(bRst);
}












/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_EVSEProcessing
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月19日  16:02:22
 * @Description    :      ISO15118 协议 设置 EVSEProcessing 字段 api接口函数
 * @Input          :      i32Index              外设号
 * @Input          :      eId                   消息ID
 * @Input          :      i32EvseProcessing     EVSE处理状态
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Set_EVSEProcessing(i32 i32Index, eV2gIso1MsgId_t eId, i32 i32EvseProcessing)
{
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (eId <= eV2gIso1MsgIdMax) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        switch(eId)
        {
            case eV2gIso1MsgIdAuthorization:
                pCache->stData.stAuthorizationRes.EVSEProcessing = i32EvseProcessing;
                break;
            case eV2gIso1MsgIdChargeParameterDiscovery:
                pCache->stData.stChargeParamDiscoveryRes.EVSEProcessing = i32EvseProcessing;
                break;
            case eV2gIso1MsgIdCableCheck:
                pCache->stData.stCableCheckRes.EVSEProcessing = i32EvseProcessing;
                break;
            case eV2gIso1MsgIdMax:
                //定义 eId 为此值时表示把所有相关字段全部赋值，初始化缓存数据字段时 可以一次调用函数初始化全部字段
                pCache->stData.stAuthorizationRes.EVSEProcessing = i32EvseProcessing;
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








/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_ServiceId
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月21日  15:12:01
 * @Description    :      ISO15118 协议 获取 ServiceId 字段 api接口函数
 * @Input          :      i32Index              外设号
 * @Input          :      pServiceId            指向储存pServiceId的i32指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_ServiceId(i32 i32Index, i32 *pServiceId)
{
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pServiceId != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        (*pServiceId) = pCache->stDataPublic.ServiceID;
        
        return(true);
    }
    
    return(false);
}









/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_DC_EVStatus
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月19日  16:08:00
 * @Description    :      ISO15118 协议 获取 DC_EVStatus 字段 api接口函数
 * @Input          :      i32Index              外设号
 * @Input          :      pDC_EVStatus          指向储存的DC_EVStatus结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_DC_EVStatus(i32 i32Index, struct iso1DC_EVStatusType *pDC_EVStatus)
{
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pDC_EVStatus != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        memcpy(pDC_EVStatus, &pCache->stDataPublic.DC_EVStatus, sizeof(struct iso1DC_EVStatusType));
        
        return(true);
    }
    
    return(false);
}












/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_EVMaximumCurrentLimit
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月19日  16:17:30
 * @Description    :      ISO15118 协议 获取 EVMaximumCurrentLimit 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      pEVMaximumCurrentLimit    指向储存的PhysicalValue结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_EVMaximumCurrentLimit(i32 i32Index, struct iso1PhysicalValueType *pEVMaximumCurrentLimit)
{
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pEVMaximumCurrentLimit != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        if(pCache->stDataPublic.EVMaximumCurrentLimit_isUsed == 1u)
        {
            memcpy(pEVMaximumCurrentLimit, &pCache->stDataPublic.EVMaximumCurrentLimit, sizeof(struct iso1PhysicalValueType));
            
            return(true);
        }
    }
    
    return(false);
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_EVMaximumPowerLimit
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月19日  17:35:53
 * @Description    :      ISO15118 协议 获取 EVMaximumPowerLimit 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      pEVMaximumPowerLimit      指向储存的PhysicalValue结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_EVMaximumPowerLimit(i32 i32Index, struct iso1PhysicalValueType *pEVMaximumPowerLimit)
{
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pEVMaximumPowerLimit != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        if(pCache->stDataPublic.EVMaximumPowerLimit_isUsed == 1u)
        {
            memcpy(pEVMaximumPowerLimit, &pCache->stDataPublic.EVMaximumPowerLimit, sizeof(struct iso1PhysicalValueType));
            
            return(true);
        }
    }
    
    return(false);
}













/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_EVMaximumVoltageLimit
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月19日  17:39:32
 * @Description    :      ISO15118 协议 获取 EVMaximumVoltageLimit 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      pEVMaximumVoltageLimit    指向储存的PhysicalValue结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_EVMaximumVoltageLimit(i32 i32Index, struct iso1PhysicalValueType *pEVMaximumVoltageLimit)
{
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pEVMaximumVoltageLimit != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        if(pCache->stDataPublic.EVMaximumVoltageLimit_isUsed == 1u)
        {
            memcpy(pEVMaximumVoltageLimit, &pCache->stDataPublic.EVMaximumVoltageLimit, sizeof(struct iso1PhysicalValueType));
            
            return(true);
        }
    }
    
    return(false);
}











/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_AC_EVSEStatus
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月23日  17:06:03
 * @Description    :      ISO15118 协议 设置 AC_EVSEStatus 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      eId                       消息ID
 * @Input          :      pAC_EVSEStatus            指向要写入的AC_EVSEStatus结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Set_AC_EVSEStatus(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1AC_EVSEStatusType *pAC_EVSEStatus)
{
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pAC_EVSEStatus != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        switch(eId)
        {
            case eV2gIso1MsgIdChargeParameterDiscovery:
                if(pAC_EVSEStatus != NULL)
                {
                    pCache->stData.stChargeParamDiscoveryRes.AC_EVSEChargeParameter_isUsed = 1u;
                    memcpy(&pCache->stData.stChargeParamDiscoveryRes.AC_EVSEChargeParameter.AC_EVSEStatus, pAC_EVSEStatus, sizeof(struct iso1AC_EVSEStatusType));
                }
                break;
            case eV2gIso1MsgIdPowerDeliveryStart:
            case eV2gIso1MsgIdPowerDeliveryRenegotiate:
            case eV2gIso1MsgIdPowerDeliveryStop:
                if(pAC_EVSEStatus != NULL)
                {
                    memcpy(&pCache->stData.stPowerDeliveryRes.AC_EVSEStatus, pAC_EVSEStatus, sizeof(struct iso1AC_EVSEStatusType));
                    pCache->stData.stPowerDeliveryRes.AC_EVSEStatus_isUsed = 1u;
                }
                else
                {
                    pCache->stData.stPowerDeliveryRes.AC_EVSEStatus_isUsed = 0u;
                }
                break;
            case eV2gIso1MsgIdChargingStatus:
                memcpy(&pCache->stData.stChargingStatusRes.AC_EVSEStatus, pAC_EVSEStatus, sizeof(struct iso1AC_EVSEStatusType));
                break;
            case eV2gIso1MsgIdMeteringReceipt:
                if(pAC_EVSEStatus != NULL)
                {
                    memcpy(&pCache->stData.stMeteringReceiptRes.AC_EVSEStatus, pAC_EVSEStatus, sizeof(struct iso1AC_EVSEStatusType));
                    pCache->stData.stMeteringReceiptRes.AC_EVSEStatus_isUsed = 1u;
                }
                else
                {
                    pCache->stData.stMeteringReceiptRes.AC_EVSEStatus_isUsed = 0u;
                }
                break;
            case eV2gIso1MsgIdMax:
                //定义 eId 为此值时表示把所有相关字段全部赋值，初始化缓存数据字段时 可以一次调用函数初始化全部字段
                pCache->stData.stChargeParamDiscoveryRes.AC_EVSEChargeParameter_isUsed = 1u;
                if(pAC_EVSEStatus != NULL)
                {
                    memcpy(&pCache->stData.stChargeParamDiscoveryRes.AC_EVSEChargeParameter.AC_EVSEStatus, pAC_EVSEStatus, sizeof(struct iso1AC_EVSEStatusType));
                    memcpy(&pCache->stData.stPowerDeliveryRes.AC_EVSEStatus, pAC_EVSEStatus, sizeof(struct iso1AC_EVSEStatusType));
                    memcpy(&pCache->stData.stChargingStatusRes.AC_EVSEStatus, pAC_EVSEStatus, sizeof(struct iso1AC_EVSEStatusType));
                    memcpy(&pCache->stData.stMeteringReceiptRes.AC_EVSEStatus, pAC_EVSEStatus, sizeof(struct iso1AC_EVSEStatusType));
                    pCache->stData.stPowerDeliveryRes.AC_EVSEStatus_isUsed = 1u;
                    pCache->stData.stMeteringReceiptRes.AC_EVSEStatus_isUsed = 1u;
                }
                else
                {
                    pCache->stData.stChargeParamDiscoveryRes.AC_EVSEChargeParameter_isUsed = 0u;
                    pCache->stData.stPowerDeliveryRes.AC_EVSEStatus_isUsed = 0u;
                    pCache->stData.stMeteringReceiptRes.AC_EVSEStatus_isUsed = 0u;
                }
                break;
            default:
                return(false);
        }
        
        return(true);
    }
    
    return(false);
}











/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_EVSEMaxCurrent
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月23日  17:12:59
 * @Description    :      ISO15118 协议 设置 EVSEMaxCurrent 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      eId                       消息ID
 * @Input          :      pEVSEMaxCurrent           指向要写入的PhysicalValue结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Set_EVSEMaxCurrent(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1PhysicalValueType *pEVSEMaxCurrent)
{
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        switch(eId)
        {
            case eV2gIso1MsgIdChargeParameterDiscovery:
                if(pEVSEMaxCurrent != NULL)
                {
                    memcpy(&pCache->stData.stChargeParamDiscoveryRes.AC_EVSEChargeParameter.EVSEMaxCurrent, pEVSEMaxCurrent, sizeof(struct iso1PhysicalValueType));
                    pCache->stData.stChargeParamDiscoveryRes.AC_EVSEChargeParameter_isUsed = 1u;
                }
                break;
            case eV2gIso1MsgIdChargingStatus:
                if(pEVSEMaxCurrent != NULL)
                {
                    memcpy(&pCache->stData.stChargingStatusRes.EVSEMaxCurrent, pEVSEMaxCurrent, sizeof(struct iso1PhysicalValueType));
                    pCache->stData.stChargingStatusRes.EVSEMaxCurrent_isUsed = 1u;
                }
                else
                {
                    pCache->stData.stChargingStatusRes.EVSEMaxCurrent_isUsed = 0u;
                }
                break;
            case eV2gIso1MsgIdMax:
                //定义 eId 为此值时表示把所有相关字段全部赋值，初始化缓存数据字段时 可以一次调用函数初始化全部字段
                pCache->stData.stChargeParamDiscoveryRes.AC_EVSEChargeParameter_isUsed = 1u;
                if(pEVSEMaxCurrent != NULL)
                {
                    memcpy(&pCache->stData.stChargeParamDiscoveryRes.AC_EVSEChargeParameter.EVSEMaxCurrent, pEVSEMaxCurrent, sizeof(struct iso1PhysicalValueType));
                    memcpy(&pCache->stData.stChargingStatusRes.EVSEMaxCurrent,                              pEVSEMaxCurrent, sizeof(struct iso1PhysicalValueType));
                    pCache->stData.stChargingStatusRes.EVSEMaxCurrent_isUsed = 1u;
                }
                else
                {
                    pCache->stData.stChargeParamDiscoveryRes.AC_EVSEChargeParameter_isUsed = 0u;
                    pCache->stData.stChargingStatusRes.EVSEMaxCurrent_isUsed = 0u;
                }
                break;
            default:
                return(false);
        }
        
        return(true);
    }
    
    return(false);
}

















/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_DC_EVSEStatus
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月19日  17:46:08
 * @Description    :      ISO15118 协议 设置 DC_EVSEStatus 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      eId                       消息ID
 * @Input          :      pDC_EVSEStatus            指向要写入的DC_EVSEStatus结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Set_DC_EVSEStatus(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1DC_EVSEStatusType *pDC_EVSEStatus)
{
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pDC_EVSEStatus != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        switch(eId)
        {
            case eV2gIso1MsgIdChargeParameterDiscovery:
                if(pDC_EVSEStatus != NULL)
                {
                    memcpy(&pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.DC_EVSEStatus, pDC_EVSEStatus, sizeof(struct iso1DC_EVSEStatusType));
                    pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter_isUsed = 1u;
                }
                break;
            case eV2gIso1MsgIdCableCheck:
                memcpy(&pCache->stData.stCableCheckRes.DC_EVSEStatus, pDC_EVSEStatus, sizeof(struct iso1DC_EVSEStatusType));
                break;
            case eV2gIso1MsgIdPreCharge:
                memcpy(&pCache->stData.stPreChargeRes.DC_EVSEStatus, pDC_EVSEStatus, sizeof(struct iso1DC_EVSEStatusType));
                break;
            case eV2gIso1MsgIdPowerDeliveryStart:
            case eV2gIso1MsgIdPowerDeliveryRenegotiate:
            case eV2gIso1MsgIdPowerDeliveryStop:
                if(pDC_EVSEStatus != NULL)
                {
                    memcpy(&pCache->stData.stPowerDeliveryRes.DC_EVSEStatus, pDC_EVSEStatus, sizeof(struct iso1DC_EVSEStatusType));
                    pCache->stData.stPowerDeliveryRes.DC_EVSEStatus_isUsed = 1u;
                }
                else
                {
                    pCache->stData.stPowerDeliveryRes.DC_EVSEStatus_isUsed = 0u;
                }
                break;
            case eV2gIso1MsgIdCurrentDemand:
                memcpy(&pCache->stData.stCurrentDemandRes.DC_EVSEStatus, pDC_EVSEStatus, sizeof(struct iso1DC_EVSEStatusType));
                break;
            case eV2gIso1MsgIdWeldingDetection:
                memcpy(&pCache->stData.stWeldingDetectionRes.DC_EVSEStatus, pDC_EVSEStatus, sizeof(struct iso1DC_EVSEStatusType));
                break;
            case eV2gIso1MsgIdMeteringReceipt:
                if(pDC_EVSEStatus != NULL)
                {
                    memcpy(&pCache->stData.stMeteringReceiptRes.DC_EVSEStatus, pDC_EVSEStatus, sizeof(struct iso1DC_EVSEStatusType));
                    pCache->stData.stMeteringReceiptRes.DC_EVSEStatus_isUsed = 1u;
                }
                else
                {
                    pCache->stData.stMeteringReceiptRes.DC_EVSEStatus_isUsed = 0u;
                }
                break;
            case eV2gIso1MsgIdMax:
                //定义 eId 为此值时表示把所有相关字段全部赋值，初始化缓存数据字段时 可以一次调用函数初始化全部字段
                pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter_isUsed = 1u;
                if(pDC_EVSEStatus != NULL)
                {
                    memcpy(&pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.DC_EVSEStatus, pDC_EVSEStatus, sizeof(struct iso1DC_EVSEStatusType));
                    memcpy(&pCache->stData.stCableCheckRes.DC_EVSEStatus,       pDC_EVSEStatus, sizeof(struct iso1DC_EVSEStatusType));
                    memcpy(&pCache->stData.stPreChargeRes.DC_EVSEStatus,        pDC_EVSEStatus, sizeof(struct iso1DC_EVSEStatusType));
                    memcpy(&pCache->stData.stPowerDeliveryRes.DC_EVSEStatus,    pDC_EVSEStatus, sizeof(struct iso1DC_EVSEStatusType));
                    memcpy(&pCache->stData.stCurrentDemandRes.DC_EVSEStatus,    pDC_EVSEStatus, sizeof(struct iso1DC_EVSEStatusType));
                    memcpy(&pCache->stData.stMeteringReceiptRes.DC_EVSEStatus,  pDC_EVSEStatus, sizeof(struct iso1DC_EVSEStatusType));
                    memcpy(&pCache->stData.stWeldingDetectionRes.DC_EVSEStatus, pDC_EVSEStatus, sizeof(struct iso1DC_EVSEStatusType));
                    pCache->stData.stPowerDeliveryRes.DC_EVSEStatus_isUsed = 1u;
                    pCache->stData.stMeteringReceiptRes.DC_EVSEStatus_isUsed = 1u;
                }
                else
                {
                    pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter_isUsed = 0u;
                    pCache->stData.stPowerDeliveryRes.DC_EVSEStatus_isUsed = 0u;
                    pCache->stData.stMeteringReceiptRes.DC_EVSEStatus_isUsed = 0u;
                }
                break;
            default:
                return(false);
        }
        
        return(true);
    }
    
    return(false);
}












/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_EVSEMaximumCurrentLimit
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月19日  17:57:02
 * @Description    :      ISO15118 协议 设置 EVSEMaximumCurrentLimit 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      eId                       消息ID
 * @Input          :      pEVSEMaximumCurrentLimit  指向要写入的PhysicalValue结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Set_EVSEMaximumCurrentLimit(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1PhysicalValueType *pEVSEMaximumCurrentLimit)
{
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        switch(eId)
        {
            case eV2gIso1MsgIdChargeParameterDiscovery:
                if(pEVSEMaximumCurrentLimit != NULL)
                {
                    memcpy(&pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEMaximumCurrentLimit, pEVSEMaximumCurrentLimit, sizeof(struct iso1PhysicalValueType));
                    pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter_isUsed = 1u;
                }
                break;
            case eV2gIso1MsgIdCurrentDemand:
                if(pEVSEMaximumCurrentLimit != NULL)
                {
                    memcpy(&pCache->stData.stCurrentDemandRes.EVSEMaximumCurrentLimit, pEVSEMaximumCurrentLimit, sizeof(struct iso1PhysicalValueType));
                    pCache->stData.stCurrentDemandRes.EVSEMaximumCurrentLimit_isUsed = 1u;
                }
                else
                {
                    pCache->stData.stCurrentDemandRes.EVSEMaximumCurrentLimit_isUsed = 0u;
                }
                break;
            case eV2gIso1MsgIdMax:
                //定义 eId 为此值时表示把所有相关字段全部赋值，初始化缓存数据字段时 可以一次调用函数初始化全部字段
                pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter_isUsed = 1u;
                if(pEVSEMaximumCurrentLimit != NULL)
                {
                    memcpy(&pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEMaximumCurrentLimit, pEVSEMaximumCurrentLimit, sizeof(struct iso1PhysicalValueType));
                    memcpy(&pCache->stData.stCurrentDemandRes.EVSEMaximumCurrentLimit,                               pEVSEMaximumCurrentLimit, sizeof(struct iso1PhysicalValueType));
                    pCache->stData.stCurrentDemandRes.EVSEMaximumCurrentLimit_isUsed = 1u;
                }
                else
                {
                    pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter_isUsed = 0u;
                    pCache->stData.stCurrentDemandRes.EVSEMaximumCurrentLimit_isUsed = 0u;
                }
                break;
            default:
                break;
        }
        
        return(true);
    }
    
    return(false);
}












/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_EVSEMaximumPowerLimit
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月19日  19:15:43
 * @Description    :      ISO15118 协议 设置 EVSEMaximumPowerLimit 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      eId                       消息ID
 * @Input          :      pEVSEMaximumPowerLimit    指向要写入的PhysicalValue结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Set_EVSEMaximumPowerLimit(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1PhysicalValueType *pEVSEMaximumPowerLimit)
{
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        switch(eId)
        {
        case eV2gIso1MsgIdChargeParameterDiscovery:
            if(pEVSEMaximumPowerLimit != NULL)
            {
                memcpy(&pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEMaximumPowerLimit, pEVSEMaximumPowerLimit, sizeof(struct iso1PhysicalValueType));
                pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter_isUsed = 1u;
            }
            break;
        case eV2gIso1MsgIdCurrentDemand:
            if(pEVSEMaximumPowerLimit != NULL)
            {
                memcpy(&pCache->stData.stCurrentDemandRes.EVSEMaximumPowerLimit, pEVSEMaximumPowerLimit, sizeof(struct iso1PhysicalValueType));
                pCache->stData.stCurrentDemandRes.EVSEMaximumPowerLimit_isUsed = 1u;
            }
            else
            {
                pCache->stData.stCurrentDemandRes.EVSEMaximumPowerLimit_isUsed = 0u;
            }
            break;
        case eV2gIso1MsgIdMax:
            //定义 eId 为此值时表示把所有相关字段全部赋值，初始化缓存数据字段时 可以一次调用函数初始化全部字段
            pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter_isUsed = 1u;
            if(pEVSEMaximumPowerLimit != NULL)
            {
                memcpy(&pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEMaximumPowerLimit,  pEVSEMaximumPowerLimit, sizeof(struct iso1PhysicalValueType));
                memcpy(&pCache->stData.stCurrentDemandRes.EVSEMaximumPowerLimit,                                pEVSEMaximumPowerLimit, sizeof(struct iso1PhysicalValueType));
                pCache->stData.stCurrentDemandRes.EVSEMaximumPowerLimit_isUsed = 1u;
            }
            else
            {
                pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter_isUsed = 0u;
                pCache->stData.stCurrentDemandRes.EVSEMaximumPowerLimit_isUsed = 0u;
            }
            break;
        default:
            break;
        }
        
        return(true);
    }
    
    return(false);
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_EVSEMaximumVoltageLimit
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月19日  19:28:41
 * @Description    :      ISO15118 协议 设置 EVSEMaximumVoltageLimit 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      eId                       消息ID
 * @Input          :      pEVSEMaximumVoltageLimit  指向要写入的PhysicalValue结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Set_EVSEMaximumVoltageLimit(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1PhysicalValueType *pEVSEMaximumVoltageLimit)
{
    bool bRst = false;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        switch(eId)
        {
            case eV2gIso1MsgIdChargeParameterDiscovery:
                if(pEVSEMaximumVoltageLimit != NULL)
                {
                    memcpy(&pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEMaximumVoltageLimit, pEVSEMaximumVoltageLimit, sizeof(struct iso1PhysicalValueType));
                    pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter_isUsed = 1u;
                }
                break;
            case eV2gIso1MsgIdCurrentDemand:
                if(pEVSEMaximumVoltageLimit != NULL)
                {
                    memcpy(&pCache->stData.stCurrentDemandRes.EVSEMaximumVoltageLimit, pEVSEMaximumVoltageLimit, sizeof(struct iso1PhysicalValueType));
                    pCache->stData.stCurrentDemandRes.EVSEMaximumVoltageLimit_isUsed = 1u;
                }
                else
                {
                    pCache->stData.stCurrentDemandRes.EVSEMaximumVoltageLimit_isUsed = 0u;
                }
                break;
            case eV2gIso1MsgIdMax:
                //定义 eId 为此值时表示把所有相关字段全部赋值，初始化缓存数据字段时 可以一次调用函数初始化全部字段
                if(pEVSEMaximumVoltageLimit != NULL)
                {
                    bRst = true;
                    memcpy(&pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEMaximumVoltageLimit,    pEVSEMaximumVoltageLimit, sizeof(struct iso1PhysicalValueType));
                    memcpy(&pCache->stData.stCurrentDemandRes.EVSEMaximumVoltageLimit,                                  pEVSEMaximumVoltageLimit, sizeof(struct iso1PhysicalValueType));
                    pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter_isUsed = 1u;
                    pCache->stData.stCurrentDemandRes.EVSEMaximumVoltageLimit_isUsed = 1u;
                }
                else
                {
                    pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter_isUsed = 0u;
                    pCache->stData.stCurrentDemandRes.EVSEMaximumVoltageLimit_isUsed = 0u;
                }
                break;
            default:
                break;
        }
        
        return(true);
    }
    
    return(false);
}











/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_EVTargetVoltage
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月19日  19:34:22
 * @Description    :      ISO15118 协议 获取 EVTargetVoltage 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      pEVTargetVoltage          指向储存的PhysicalValue结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_EVTargetVoltage(i32 i32Index, struct iso1PhysicalValueType *pEVTargetVoltage)
{
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pEVTargetVoltage != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        memcpy(pEVTargetVoltage, &pCache->stDataPublic.EVTargetVoltage, sizeof(struct iso1PhysicalValueType));
        
        return(true);
    }
    
    return(false);
}













/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_EVTargetCurrent
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月19日  19:48:24
 * @Description    :      ISO15118 协议 获取 EVTargetCurrent 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      pEVTargetCurrent          指向储存的PhysicalValue结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_EVTargetCurrent(i32 i32Index, struct iso1PhysicalValueType *pEVTargetCurrent)
{
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pEVTargetCurrent != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        memcpy(pEVTargetCurrent, &pCache->stDataPublic.EVTargetCurrent, sizeof(struct iso1PhysicalValueType));
            
        return(true);
    }
    
    return(false);
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_EVSEPresentVoltage
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月19日  19:51:38
 * @Description    :      ISO15118 协议 设置 EVSEPresentVoltage 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      eId                       消息ID
 * @Input          :      pEVSEPresentVoltage       指向储存的PhysicalValue结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Set_EVSEPresentVoltage(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1PhysicalValueType *pEVSEPresentVoltage)
{
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pEVSEPresentVoltage != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        switch(eId)
        {
            case eV2gIso1MsgIdPreCharge:
                memcpy(&pCache->stData.stPreChargeRes.EVSEPresentVoltage,        pEVSEPresentVoltage, sizeof(struct iso1PhysicalValueType));
                break;
            case eV2gIso1MsgIdCurrentDemand:
                memcpy(&pCache->stData.stCurrentDemandRes.EVSEPresentVoltage,    pEVSEPresentVoltage, sizeof(struct iso1PhysicalValueType));
                break;
            case eV2gIso1MsgIdWeldingDetection:
                memcpy(&pCache->stData.stWeldingDetectionRes.EVSEPresentVoltage, pEVSEPresentVoltage, sizeof(struct iso1PhysicalValueType));
                break;
            case eV2gIso1MsgIdMax:
                memcpy(&pCache->stData.stPreChargeRes.EVSEPresentVoltage,        pEVSEPresentVoltage, sizeof(struct iso1PhysicalValueType));
                memcpy(&pCache->stData.stCurrentDemandRes.EVSEPresentVoltage,    pEVSEPresentVoltage, sizeof(struct iso1PhysicalValueType));
                memcpy(&pCache->stData.stWeldingDetectionRes.EVSEPresentVoltage, pEVSEPresentVoltage, sizeof(struct iso1PhysicalValueType));
                break;
            default:
                break;
        }
        
        return(true);
    }
    
    return(false);
}











/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_BulkChargingComplete
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月19日  19:56:29
 * @Description    :      ISO15118 协议 获取 BulkChargingComplete 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      pBulkChargingComplete     指向储存BulkChargingComplete的i32指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_BulkChargingComplete(i32 i32Index, i32 *pBulkChargingComplete)
{
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pBulkChargingComplete != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        if(pCache->stDataPublic.BulkChargingComplete_isUsed == 1u)
        {
            (*pBulkChargingComplete) = pCache->stDataPublic.BulkChargingComplete;
            
            return(true);
        }
    }
    
    return(false);
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_ChargingComplete
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月19日  19:59:59
 * @Description    :      ISO15118 协议 获取 ChargingComplete 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      pChargingComplete         指向储存ChargingComplete的i32指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_ChargingComplete(i32 i32Index, i32 *pChargingComplete)
{
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pChargingComplete != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        (*pChargingComplete) = pCache->stDataPublic.ChargingComplete;
        
        return(true);
    }
    
    return(false);
}











/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_Id
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月21日  20:45:42
 * @Description    :      ISO15118 协议 获取 Id 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      eId                       消息ID
 * @Input          :      pLen                      指向储存Id长度的i32指针
 * @Input          :      pId                       指向储存Id的i32指针(32位字符串, 支持Unicode编码)
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_Id(i32 i32Index, eV2gIso1MsgId_t eId, i32 *pLen, i32 *pId)
{
    bool bRst = false;
    i32 i;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (eId <= eV2gIso1MsgIdMax) && (pLen != NULL) && (pId != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        switch(eId)
        {
            case eV2gIso1MsgIdCertificateInstallation:
                (*pLen) = pCache->stData.stCertificateInstallationReq.Id.charactersLen;
                for(i = 0; i < (*pLen); i++)
                {
                    pId[i] = pCache->stData.stCertificateInstallationReq.Id.characters[i];
                }
                bRst = true;
                break;
                
            case eV2gIso1MsgIdCertificateUpdate:
                (*pLen) = pCache->stData.stCertificateUpdateReq.Id.charactersLen;
                for(i = 0; i < (*pLen); i++)
                {
                    pId[i] = pCache->stData.stCertificateUpdateReq.Id.characters[i];
                }
                bRst = true;
                break;
                
            default:
                break;
        }
    }
    
    return(bRst);
}























/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_ListOfRootCertificateIDs
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月22日  16:12:01
 * @Description    :      ISO15118 协议 获取 ListOfRootCertificateIDs 字段 api接口函数
 * @Input          :      i32Index                      外设号
 * @Input          :      eId                           消息ID
 * @Input          :      pListOfRootCertificateIDs     指向储存ListOfRootCertificateIDs的结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_ListOfRootCertificateIDs(i32 i32Index, eV2gIso1MsgId_t eId, struct iso1ListOfRootCertificateIDsType *pListOfRootCertificateIDs)
{
    bool bRst = false;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (eId <= eV2gIso1MsgIdMax) && (pListOfRootCertificateIDs != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        switch(eId)
        {
            case eV2gIso1MsgIdCertificateInstallation:
                memcpy(pListOfRootCertificateIDs, &pCache->stData.stCertificateInstallationReq.ListOfRootCertificateIDs, sizeof(struct iso1ListOfRootCertificateIDsType));
                bRst = true;
                break;
                
            case eV2gIso1MsgIdCertificateUpdate:
                memcpy(pListOfRootCertificateIDs, &pCache->stData.stCertificateUpdateReq.ListOfRootCertificateIDs, sizeof(struct iso1ListOfRootCertificateIDsType));
                bRst = true;
                break;
                
            default:
                break;
        }
    }
    
    return(bRst);
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_ContractSignatureCertChain
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月22日  15:53:45
 * @Description    :      ISO15118 协议 获取 ContractSignatureCertChain 字段 api接口函数
 * @Input          :      i32Index                      外设号
 * @Input          :      pSubCertificateChain          指向储存CertificateChain的结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_ContractSignatureCertChain(i32 i32Index, eV2gIso1MsgId_t eId, struct iso1CertificateChainType *pSubCertificateChain)
{
    bool bRst = false;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (eId <= eV2gIso1MsgIdMax) && (pSubCertificateChain != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        if(eId == eV2gIso1MsgIdCertificateUpdate)
        {
            memcpy(pSubCertificateChain, &pCache->stData.stCertificateUpdateReq.ContractSignatureCertChain, sizeof(struct iso1CertificateChainType));
            bRst = true;
        }
        else if(eId == eV2gIso1MsgIdPaymentDetails)
        {
            memcpy(pSubCertificateChain, &pCache->stData.stPaymentDetailsReq.ContractSignatureCertChain, sizeof(struct iso1CertificateChainType));
            bRst = true;
        }
    }
    
    return(bRst);
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_eMAID
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月22日  17:10:36
 * @Description    :      ISO15118 协议 获取 eMAID 字段 api接口函数
 * @Input          :      i32Index                      外设号
 * @Input          :      pEMAIDLen                     指向储存eMAID长度的i32指针
 * @Input          :      pEMAID                        指向储存eMAID的i32指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_eMAID(i32 i32Index, eV2gIso1MsgId_t eId, i32 *pEMAIDLen, i32 *pEMAID)
{
    bool bRst = false;
    i32 i;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (eId <= eV2gIso1MsgIdMax) && (pEMAIDLen != NULL) && (pEMAID != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        if(eId == eV2gIso1MsgIdCertificateUpdate)
        {
            (*pEMAIDLen) = pCache->stData.stCertificateUpdateReq.eMAID.charactersLen;
            for(i = 0; i < (*pEMAIDLen); i++)
            {
                pEMAID[i] = pCache->stData.stCertificateUpdateReq.eMAID.characters[i];
            }
            bRst = true;
        }
        else if(eId == eV2gIso1MsgIdPaymentDetails)
        {
            (*pEMAIDLen) = pCache->stData.stPaymentDetailsReq.eMAID.charactersLen;
            for(i = 0; i < (*pEMAIDLen); i++)
            {
                pEMAID[i] = pCache->stData.stPaymentDetailsReq.eMAID.characters[i];
            }
            bRst = true;
        }
    }
    
    return(bRst);
}














/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_SAProvisioningCertificateChain
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月22日  11:23:39
 * @Description    :      ISO15118 协议 设置 SAProvisioningCertificateChain 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      eId                       消息ID
 * @Input          :      pCertificateChain         指向要写入的CertificateChain的结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Set_SAProvisioningCertificateChain(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1CertificateChainType *pCertificateChain)
{
    bool bRst = false;
    i32 i, i32Size;
    struct iso1CertificateChainType *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (eId <= eV2gIso1MsgIdMax) && (pCertificateChain != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        if(eId == eV2gIso1MsgIdCertificateInstallation)
        {
            pCache = &pV2gIso1Cache[i32Index]->stData.stCertificateInstallationRes.SAProvisioningCertificateChain;
        }
        else if(eId == eV2gIso1MsgIdCertificateUpdate)
        {
            pCache = &pV2gIso1Cache[i32Index]->stData.stCertificateUpdateRes.SAProvisioningCertificateChain;
        }
        
        i32Size = sizeof(struct iso1CertificateChainType);
        memset(pCache, 0, i32Size);
        
        if((pCertificateChain->Id.charactersLen > 0)
        && (pCertificateChain->Id.charactersLen <= iso1CertificateChainType_Id_CHARACTERS_SIZE))
        {
            pCache->Id_isUsed = 1;
            pCache->Id.charactersLen = pCertificateChain->Id.charactersLen;
            for(i = 0; i < pCertificateChain->Id.charactersLen; i++)
            {
                pCache->Id.characters[i] = pCertificateChain->Id.characters[i];
            }
        }
        else
        {
            pCache->Id_isUsed = 0;
        }
        
        if((pCertificateChain->Certificate.bytesLen > 0)
        && (pCertificateChain->Certificate.bytesLen <= iso1CertificateChainType_Certificate_BYTES_SIZE))
        {
            pCache->Certificate.bytesLen = pCertificateChain->Certificate.bytesLen;
            memcpy(pCache->Certificate.bytes, pCertificateChain->Certificate.bytes, pCertificateChain->Certificate.bytesLen);
            bRst = true;
        }
        
        if((pCertificateChain->SubCertificates.Certificate.arrayLen > 0)
        && (pCertificateChain->SubCertificates.Certificate.arrayLen <= iso1SubCertificatesType_Certificate_ARRAY_SIZE))
        {
            pCache->SubCertificates_isUsed = 1;
            pCache->SubCertificates.Certificate.arrayLen = pCertificateChain->SubCertificates.Certificate.arrayLen;
            for(i = 0; i < pCertificateChain->SubCertificates.Certificate.arrayLen; i++)
            {
                if((pCertificateChain->SubCertificates.Certificate.array[i].bytesLen > 0)
                && (pCertificateChain->SubCertificates.Certificate.array[i].bytesLen <= iso1SubCertificatesType_Certificate_BYTES_SIZE))
                {
                    pCache->SubCertificates.Certificate.array[i].bytesLen = pCertificateChain->SubCertificates.Certificate.array[i].bytesLen;
                    memcpy(pCache->SubCertificates.Certificate.array[i].bytes, pCertificateChain->SubCertificates.Certificate.array[i].bytes, pCertificateChain->SubCertificates.Certificate.array[i].bytesLen);
                }
            }
        }
        else
        {
            pCache->SubCertificates_isUsed = 0;
        }
    }
    
    return(bRst);
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_ContractSignatureCertChain
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月22日  11:27:41
 * @Description    :      ISO15118 协议 设置 ContractSignatureCertChain 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      eId                       消息ID
 * @Input          :      pSubCertificateChain      指向要写入的CertificateChain的结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Set_ContractSignatureCertChain(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1CertificateChainType *pSubCertificateChain)
{
    bool bRst = false;
    i32 i, i32Size;
    struct iso1CertificateChainType *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (eId <= eV2gIso1MsgIdMax) && (pSubCertificateChain != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        if(eId == eV2gIso1MsgIdCertificateInstallation)
        {
            pCache = &pV2gIso1Cache[i32Index]->stData.stCertificateInstallationRes.ContractSignatureCertChain;
        }
        else if(eId == eV2gIso1MsgIdCertificateUpdate)
        {
            pCache = &pV2gIso1Cache[i32Index]->stData.stCertificateUpdateRes.ContractSignatureCertChain;
        }
        
        i32Size = sizeof(struct iso1CertificateChainType);
        memset(pCache, 0, i32Size);
        
        if((pSubCertificateChain->Id.charactersLen > 0)
        && (pSubCertificateChain->Id.charactersLen <= iso1CertificateChainType_Id_CHARACTERS_SIZE))
        {
            pCache->Id_isUsed = 1;
            pCache->Id.charactersLen = pSubCertificateChain->Id.charactersLen;
            for(i = 0; i < pSubCertificateChain->Id.charactersLen; i++)
            {
                pCache->Id.characters[i] = pSubCertificateChain->Id.characters[i];
            }
        }
        else
        {
            pCache->Id_isUsed = 0;
        }
        
        if((pSubCertificateChain->Certificate.bytesLen > 0)
        && (pSubCertificateChain->Certificate.bytesLen <= iso1CertificateChainType_Certificate_BYTES_SIZE))
        {
            pCache->Certificate.bytesLen = pSubCertificateChain->Certificate.bytesLen;
            memcpy(pCache->Certificate.bytes, pSubCertificateChain->Certificate.bytes, pSubCertificateChain->Certificate.bytesLen);
            bRst = true;
        }
        
        if((pSubCertificateChain->SubCertificates.Certificate.arrayLen > 0)
        && (pSubCertificateChain->SubCertificates.Certificate.arrayLen <= iso1SubCertificatesType_Certificate_ARRAY_SIZE))
        {
            pCache->SubCertificates_isUsed = 1;
            pCache->SubCertificates.Certificate.arrayLen = pSubCertificateChain->SubCertificates.Certificate.arrayLen;
            for(i = 0; i < pSubCertificateChain->SubCertificates.Certificate.arrayLen; i++)
            {
                if((pSubCertificateChain->SubCertificates.Certificate.array[i].bytesLen > 0)
                && (pSubCertificateChain->SubCertificates.Certificate.array[i].bytesLen <= iso1SubCertificatesType_Certificate_BYTES_SIZE))
                {
                    pCache->SubCertificates.Certificate.array[i].bytesLen = pSubCertificateChain->SubCertificates.Certificate.array[i].bytesLen;
                    memcpy(pCache->SubCertificates.Certificate.array[i].bytes, pSubCertificateChain->SubCertificates.Certificate.array[i].bytes, pSubCertificateChain->SubCertificates.Certificate.array[i].bytesLen);
                }
            }
        }
        else
        {
            pCache->SubCertificates_isUsed = 0;
        }
    }
    
    return(bRst);
}











/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_ContractSignatureEncryptedPrivateKey
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月22日  16:29:15
 * @Description    :      ISO15118 协议 设置 ContractSignatureEncryptedPrivateKey 字段 api接口函数
 * @Input          :      i32Index                                  外设号
 * @Input          :      eId                                       消息ID
 * @Input          :      pContractSignatureEncryptedPrivateKey     指向要写入ContractSignatureEncryptedPrivateKey的结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Set_ContractSignatureEncryptedPrivateKey(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1ContractSignatureEncryptedPrivateKeyType* pContractSignatureEncryptedPrivateKey)
{
    bool bRst = false;
    i32 i, i32Size;
    struct iso1ContractSignatureEncryptedPrivateKeyType *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (eId <= eV2gIso1MsgIdMax) && (pContractSignatureEncryptedPrivateKey != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        if(eId == eV2gIso1MsgIdCertificateInstallation)
        {
            pCache = &pV2gIso1Cache[i32Index]->stData.stCertificateInstallationRes.ContractSignatureEncryptedPrivateKey;
        }
        else if(eId == eV2gIso1MsgIdCertificateUpdate)
        {
            pCache = &pV2gIso1Cache[i32Index]->stData.stCertificateUpdateRes.ContractSignatureEncryptedPrivateKey;
        }
        
        i32Size = sizeof(struct iso1ContractSignatureEncryptedPrivateKeyType);
        memset(pCache, 0, i32Size);
        
        if((pContractSignatureEncryptedPrivateKey->Id.charactersLen > 0)
        && (pContractSignatureEncryptedPrivateKey->Id.charactersLen <= iso1ContractSignatureEncryptedPrivateKeyType_Id_CHARACTERS_SIZE))
        {
            pCache->Id.charactersLen = pContractSignatureEncryptedPrivateKey->Id.charactersLen;
            for(i = 0; i < pContractSignatureEncryptedPrivateKey->Id.charactersLen; i++)
            {
                pCache->Id.characters[i] = pContractSignatureEncryptedPrivateKey->Id.characters[i];
            }
            bRst = true;
        }
        
        if(bRst == true)
        {
            bRst = false;
            if((pContractSignatureEncryptedPrivateKey->CONTENT.bytesLen > 0)
            && (pContractSignatureEncryptedPrivateKey->CONTENT.bytesLen <= iso1ContractSignatureEncryptedPrivateKeyType_CONTENT_BYTES_SIZE))
            {
                pCache->CONTENT.bytesLen = pContractSignatureEncryptedPrivateKey->CONTENT.bytesLen;
                memcpy(pCache->CONTENT.bytes, pContractSignatureEncryptedPrivateKey->CONTENT.bytes, pContractSignatureEncryptedPrivateKey->CONTENT.bytesLen);
                bRst = true;
            }
        }
    }
    
    return(bRst);
}












/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_DHpublickey
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月22日  16:58:09
 * @Description    :      ISO15118 协议 设置 DHpublickey 字段 api接口函数
 * @Input          :      i32Index                      外设号
 * @Input          :      eId                           消息ID
 * @Input          :      pDiffieHellmanPublickey       指向要写入DiffieHellmanPublickey的结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Set_DHpublickey(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1DiffieHellmanPublickeyType* pDiffieHellmanPublickey)
{
    bool bRst = false;
    i32 i, i32Size;
    struct iso1DiffieHellmanPublickeyType *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (eId <= eV2gIso1MsgIdMax) && (pDiffieHellmanPublickey != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        if(eId == eV2gIso1MsgIdCertificateInstallation)
        {
            pCache = &pV2gIso1Cache[i32Index]->stData.stCertificateInstallationRes.DHpublickey;
        }
        else if(eId == eV2gIso1MsgIdCertificateUpdate)
        {
            pCache = &pV2gIso1Cache[i32Index]->stData.stCertificateUpdateRes.DHpublickey;
        }
        
        i32Size = sizeof(struct iso1DiffieHellmanPublickeyType);
        memset(pCache, 0, i32Size);
        
        if((pDiffieHellmanPublickey->Id.charactersLen > 0)
        && (pDiffieHellmanPublickey->Id.charactersLen <= iso1DiffieHellmanPublickeyType_Id_CHARACTERS_SIZE))
        {
            pCache->Id.charactersLen = pDiffieHellmanPublickey->Id.charactersLen;
            for(i = 0; i < pDiffieHellmanPublickey->Id.charactersLen; i++)
            {
                pCache->Id.characters[i] = pDiffieHellmanPublickey->Id.characters[i];
            }
            bRst = true;
        }
        
        if(bRst == true)
        {
            bRst = false;
            if((pDiffieHellmanPublickey->CONTENT.bytesLen > 0)
            && (pDiffieHellmanPublickey->CONTENT.bytesLen <= iso1DiffieHellmanPublickeyType_CONTENT_BYTES_SIZE))
            {
                pCache->CONTENT.bytesLen = pDiffieHellmanPublickey->CONTENT.bytesLen;
                memcpy(pCache->CONTENT.bytes, pDiffieHellmanPublickey->CONTENT.bytes, pDiffieHellmanPublickey->CONTENT.bytesLen);
                bRst = true;
            }
        }
    }
    
    return(bRst);
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_eMAID
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月22日  16:59:06
 * @Description    :      ISO15118 协议 设置 eMAID 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      eId                       消息ID
 * @Input          :      pEMAID                    指向要写入EMAID的结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Set_eMAID(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1EMAIDType* pEMAID)
{
    bool bRst = false;
    i32 i, i32Size;
    struct iso1EMAIDType *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (eId <= eV2gIso1MsgIdMax) && (pEMAID != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        if(eId == eV2gIso1MsgIdCertificateInstallation)
        {
            pCache = &pV2gIso1Cache[i32Index]->stData.stCertificateInstallationRes.eMAID;
        }
        else if(eId == eV2gIso1MsgIdCertificateUpdate)
        {
            pCache = &pV2gIso1Cache[i32Index]->stData.stCertificateUpdateRes.eMAID;
        }
        
        i32Size = sizeof(struct iso1EMAIDType);
        memset(pCache, 0, i32Size);
        
        if((pEMAID->Id.charactersLen > 0)
        && (pEMAID->Id.charactersLen <= iso1EMAIDType_Id_CHARACTERS_SIZE))
        {
            pCache->Id.charactersLen = pEMAID->Id.charactersLen;
            for(i = 0; i < pEMAID->Id.charactersLen; i++)
            {
                pCache->Id.characters[i] = pEMAID->Id.characters[i];
            }
            bRst = true;
        }
        
        if(bRst == true)
        {
            bRst = false;
            if((pEMAID->CONTENT.charactersLen > 0)
            && (pEMAID->CONTENT.charactersLen <= iso1EMAIDType_CONTENT_CHARACTERS_SIZE))
            {
                pCache->CONTENT.charactersLen = pEMAID->CONTENT.charactersLen;
                for(i = 0; i < pEMAID->CONTENT.charactersLen; i++)
                {
                    pCache->CONTENT.characters[i] = pEMAID->CONTENT.characters[i];
                }
                bRst = true;
            }
        }
    }
    
    return(bRst);
}









/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_MeterInfo
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月25日  11:43:57
 * @Description    :      ISO15118 协议 设置 MeterInfo 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      eId                       消息ID
 * @Input          :      pMeterInfo                指向要写入MeterInfo的结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Set_MeterInfo(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1MeterInfoType* pMeterInfo)
{
    bool bRst = false;
    i32  i32Size;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (eId <= eV2gIso1MsgIdMax) && (pMeterInfo != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        
        i32Size = sizeof(struct iso1MeterInfoType);
        if(eId == eV2gIso1MsgIdChargingStatus)
        {
            memset(&pCache->stData.stChargingStatusRes.MeterInfo, 0, i32Size);
            memcpy(&pCache->stData.stChargingStatusRes.MeterInfo, pMeterInfo, i32Size);
            bRst = true;
        }
        else if(eId == eV2gIso1MsgIdCurrentDemand)
        {
            memset(&pCache->stData.stCurrentDemandRes.MeterInfo, 0, i32Size);
            memcpy(&pCache->stData.stCurrentDemandRes.MeterInfo, pMeterInfo, i32Size);
            bRst = true;
        }
    }
    
    return(bRst);
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_ReceiptRequired
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月25日  11:44:02
 * @Description    :      ISO15118 协议 设置 ReceiptRequired 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      eId                       消息ID
 * @Input          :      bReceiptRequired          表示EVCC需要发送MeteringReceiptReq消息
 *                                                  true : 需要发送     false : 不需要发送
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Set_ReceiptRequired(i32 i32Index, eV2gIso1MsgId_t eId, bool bReceiptRequired)
{
    bool bRst = false;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (eId <= eV2gIso1MsgIdMax) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        if(eId == eV2gIso1MsgIdChargingStatus)
        {
            pCache->stData.stChargingStatusRes.ReceiptRequired = bReceiptRequired;
            bRst = true;
        }
        else if(eId == eV2gIso1MsgIdCurrentDemand)
        {
            pCache->stData.stCurrentDemandRes.ReceiptRequired = bReceiptRequired;
            bRst = true;
        }
    }
    
    return(bRst);
}











/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_SupportedAppProtocol
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月19日  20:01:58
 * @Description    :      ISO15118 协议 获取 车&桩握手信号 api接口函数
 * @Input          :      i32Index                  外设号
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_SupportedAppProtocol(i32 i32Index)
{
    bool bRst = pdFALSE;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        bRst = xSemaphoreTake(pCache->xSemHandshake, 0);
    }
    
    return((bRst == pdTRUE) ? true : false);
}









/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_EVCCID
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月19日  20:05:44
 * @Description    :      ISO15118 协议 获取 EVCCID 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      pLen                      指向储存EVCCID长度的i32指针
 * @Input          :      pEvccId                   指向储存EVCCID的u8指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_EVCCID(i32 i32Index, i32 *pLen, u8 *pEvccId)
{
    bool bRst = false;
    i32  i32Size;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pLen != NULL) && (pEvccId != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        (*pLen) = pCache->stData.stSessionSetupReq.EVCCID.bytesLen;
        //EVCCID.bytes 是 u8 类型  可以这样拷贝
        memcpy(pEvccId, pCache->stData.stSessionSetupReq.EVCCID.bytes, (*pLen));
        bRst = true;
    }
    
    return(bRst);
}









/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_ServiceScope
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月19日  20:51:53
 * @Description    :      ISO15118 协议 获取 ServiceScope 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      pScopeLen                 指向储存ServiceScope长度的i32指针
 * @Input          :      pServiceScope             指向储存ServiceScope的i32指针(32位字符串, 支持Unicode编码)
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_ServiceScope(i32 i32Index, i32 *pScopeLen, i32 *pServiceScope)
{
    bool bRst = false;
    i32  i;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pScopeLen != NULL) && (pServiceScope != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        if(pCache->stData.stServiceDiscoveryReq.ServiceScope_isUsed == 1u)
        {
            (*pScopeLen) = pCache->stData.stServiceDiscoveryReq.ServiceScope.charactersLen;
            for(i = 0; i < (*pScopeLen); i++)
            {
                pServiceScope[i] = pCache->stData.stServiceDiscoveryReq.ServiceScope.characters[i];
            }
            bRst = true;
        }
    }
    
    return(bRst);
}









/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_ServiceCategory
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月19日  20:54:22
 * @Description    :      ISO15118 协议 获取 ServiceCategory 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      pServiceCategory          指向储存ServiceCategory的i32指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_ServiceCategory(i32 i32Index, i32 *pServiceCategory)
{
    bool bRst = false;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pServiceCategory != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        if(pCache->stData.stServiceDiscoveryReq.ServiceCategory_isUsed == 1u)
        {
            (*pServiceCategory) = pCache->stData.stServiceDiscoveryReq.ServiceCategory;
            bRst = true;
        }
    }
    
    return(bRst);
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_PaymentOptionList
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月19日  20:58:33
 * @Description    :      ISO15118 协议 设置 PaymentOptionList 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      i32PaymentOptionNum       支付方式数量
 * @Input          :      pPaymentOption            指向要写入的paymentOption结构体的指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Set_PaymentOptionList(i32 i32Index, i32 i32PaymentOptionNum, const iso1paymentOptionType *pPaymentOption)
{
    bool bRst = false;
    i32  i, i32Size;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (i32PaymentOptionNum > 0) && (pPaymentOption != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        i32Size = sizeof(pCache->stData.stServiceDiscoveryRes.PaymentOptionList.PaymentOption.array);
        if(i32PaymentOptionNum <= iso1PaymentOptionListType_PaymentOption_ARRAY_SIZE)
        {
            pCache->stData.stServiceDiscoveryRes.PaymentOptionList.PaymentOption.arrayLen = i32PaymentOptionNum;
            memset(pCache->stData.stServiceDiscoveryRes.PaymentOptionList.PaymentOption.array, 0, i32Size);
            
            for(i = 0; i < i32PaymentOptionNum; i++)
            {
                pCache->stData.stServiceDiscoveryRes.PaymentOptionList.PaymentOption.array[i] = pPaymentOption[i];
            }
            
            bRst = true;
        }
    }
    
    return(bRst);
}











/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_ChargeService
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月20日  16:22:51
 * @Description    :      ISO15118 协议 设置 ChargeService 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      u16ServiceId              ServiceID字段
 * @Input          :      pName                     指向要写入的ServiceName服务名称的i32指针(32位字符串, 支持Unicode编码)
 * @Input          :      i32NameLen                ServiceName长度
 * @Input          :      i32Category               ServiceCategory
 * @Input          :      pScope                    指向要写入的ServiceScope服务范围的i32指针(32位字符串, 支持Unicode编码)
 * @Input          :      i32ScopeLen               ServiceScope长度
 * @Input          :      bFree                     免费服务标志
 * @Input          :      pEnergyTsfMode            指向要写入的EnergyTransferMode能量传输模式的u8指针
 * @Input          :      u8ModeNum                 EnergyTransferMode数量
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Set_ChargeService(i32 i32Index, u16 u16ServiceId, const i32 *pName, i32 i32NameLen, i32 i32Category, const i32 *pScope, i32 i32ScopeLen, bool bFree, const u8 *pEnergyTsfMode, u8 u8ModeNum)
{
    i32  i, i32Size;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        pCache->stData.stServiceDiscoveryRes.ChargeService.ServiceID = u16ServiceId;
        
        i32Size = sizeof(pCache->stData.stServiceDiscoveryRes.ChargeService.ServiceName.characters);
        if((pName != NULL) && (i32NameLen > 0) && (i32NameLen <= iso1ChargeServiceType_ServiceName_CHARACTERS_SIZE))
        {
            pCache->stData.stServiceDiscoveryRes.ChargeService.ServiceName_isUsed = 1;
            pCache->stData.stServiceDiscoveryRes.ChargeService.ServiceName.charactersLen = i32NameLen;
            memset(pCache->stData.stServiceDiscoveryRes.ChargeService.ServiceName.characters, 0, i32Size);
            for(i = 0; i < i32NameLen; i++)
            {
                pCache->stData.stServiceDiscoveryRes.ChargeService.ServiceName.characters[i] = pName[i];
            }
        }
        
        pCache->stData.stServiceDiscoveryRes.ChargeService.ServiceCategory = i32Category;
        
        i32Size = sizeof(pCache->stData.stServiceDiscoveryRes.ChargeService.ServiceName.characters);
        if((pScope != NULL) && (i32ScopeLen > 0) && (i32ScopeLen <= iso1ChargeServiceType_ServiceScope_CHARACTERS_SIZE))
        {
            pCache->stData.stServiceDiscoveryRes.ChargeService.ServiceScope_isUsed = 1;
            pCache->stData.stServiceDiscoveryRes.ChargeService.ServiceScope.charactersLen = i32ScopeLen;
            memset(pCache->stData.stServiceDiscoveryRes.ChargeService.ServiceScope.characters, 0, i32Size);
            for(i = 0; i < i32ScopeLen; i++)
            {
                pCache->stData.stServiceDiscoveryRes.ChargeService.ServiceScope.characters[i] = pScope[i];
            }
        }
        
        pCache->stData.stServiceDiscoveryRes.ChargeService.FreeService = bFree;
        
        i32Size = sizeof(pCache->stData.stServiceDiscoveryRes.ChargeService.SupportedEnergyTransferMode.EnergyTransferMode.array);
        if((pEnergyTsfMode != NULL) && (u8ModeNum > 0) && (u8ModeNum <= iso1SupportedEnergyTransferModeType_EnergyTransferMode_ARRAY_SIZE))
        {
            pCache->stData.stServiceDiscoveryRes.ChargeService.SupportedEnergyTransferMode.EnergyTransferMode.arrayLen = u8ModeNum;
            memset(pCache->stData.stServiceDiscoveryRes.ChargeService.SupportedEnergyTransferMode.EnergyTransferMode.array, 0, i32Size);
            for(i = 0; i < u8ModeNum; i++)
            {
                pCache->stData.stServiceDiscoveryRes.ChargeService.SupportedEnergyTransferMode.EnergyTransferMode.array[i] = pEnergyTsfMode[i];
            }
        }
        
        return(true);
    }
    
    return(false);
}









/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_ServiceList
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月21日  10:01:05
 * @Description    :      ISO15118 协议 设置 ServiceList 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      i32ServiceNum             服务列表数量
 * @Input          :      pServiceType              指向要写入的ServiceType的结构体指针
 * @Return         :      
 * @Notice         :      <ISO15118-2:2014.pdf> 8.6.3.6 Selection of services 有具体讲解该如何填写该字段
 *                        Table 105 — Definition of ServiceID, Service Category, Service Name, and Service Scope
*******************************************************************************/
bool sV2gIso1Set_ServiceList(i32 i32Index, i32 i32ServiceNum, const struct iso1ServiceType *pServiceType)
{
    bool bRst = false;
    i32  i, i32Size;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        i32Size = sizeof(pCache->stData.stServiceDiscoveryRes.ServiceList.Service);
        memset(&pCache->stData.stServiceDiscoveryRes.ServiceList.Service, 0, i32Size);
        if((pServiceType != NULL) && (i32ServiceNum > 0) && (i32ServiceNum <= iso1ServiceListType_Service_ARRAY_SIZE))
        {
            pCache->stData.stServiceDiscoveryRes.ServiceList.Service.arrayLen = i32ServiceNum;
            for(i = 0; i < i32ServiceNum; i++)
            {
                pCache->stData.stServiceDiscoveryRes.ServiceList.Service.array[i] = pServiceType[i];
            }
            pCache->stData.stServiceDiscoveryRes.ServiceList_isUsed = 1;
            bRst = true;
        }
        else
        {
            pCache->stData.stServiceDiscoveryRes.ServiceList_isUsed = 0;
        }
    }
    
    return(bRst);
}











/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_ServiceParameterList
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月21日  14:45:02
 * @Description    :      ISO15118 协议 设置 ServiceParameterList 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      i32ParameterSetNum        服务参数数量
 * @Input          :      pParameterSet             指向要写入的ParameterSet的结构体指针
 * @Return         :      
 * @Notice         :      <ISO15118-2:2014.pdf> 8.6.3.6 Selection of services 有具体讲解该如何填写该字段
 *                        Table 106 — ServiceParameterList for certificate service
 *                        Table 107 — ServiceParameterList for internet access service
*******************************************************************************/
bool sV2gIso1Set_ServiceParameterList(i32 i32Index, i32 i32ParameterSetNum, const struct iso1ParameterSetType *pParameterSet)
{
    bool bRst = false;
    i32  i, i32Size;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        i32Size = sizeof(pCache->stData.stServiceDetailRes.ServiceParameterList);
        memset(&pCache->stData.stServiceDetailRes.ServiceParameterList, 0, i32Size);
        if((pParameterSet != NULL) && (i32ParameterSetNum > 0) && (i32ParameterSetNum <= iso1ServiceParameterListType_ParameterSet_ARRAY_SIZE))
        {
            pCache->stData.stServiceDetailRes.ServiceParameterList.ParameterSet.arrayLen = i32ParameterSetNum;
            for(i = 0; i < i32ParameterSetNum; i++)
            {
                pCache->stData.stServiceDetailRes.ServiceParameterList.ParameterSet.array[i] = pParameterSet[i];
            }
            pCache->stData.stServiceDetailRes.ServiceParameterList_isUsed = 1;
            bRst = true;
        }
        else
        {
            pCache->stData.stServiceDetailRes.ServiceParameterList_isUsed = 0;
        }
    }
    
    return(bRst);
}











/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_SelectedPaymentOption
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月20日  17:31:01
 * @Description    :      ISO15118 协议 获取 SelectedPaymentOption 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      pSelectedPaymentOption    指向储存SelectedPaymentOption的i32指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_SelectedPaymentOption(i32 i32Index, i32 *pSelectedPaymentOption)
{
    bool bRst = false;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pSelectedPaymentOption != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        (*pSelectedPaymentOption) = pCache->stData.stPaymentServiceSelectionReq.SelectedPaymentOption;
        
        bRst = true;
    }
    
    return(bRst);
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_OEMProvisioningCert
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月21日  20:57:49
 * @Description    :      ISO15118 协议 获取 OEMProvisioningCert 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      pLen                      指向储存ProvisioningCert长度的i32指针
 * @Input          :      pProvisioningCert         指向储存ProvisioningCert的u8指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_OEMProvisioningCert(i32 i32Index, i32 *pLen, u8 *pProvisioningCert)
{
    bool bRst = false;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pLen != NULL) && (pProvisioningCert != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        (*pLen) = pCache->stData.stCertificateInstallationReq.OEMProvisioningCert.bytesLen;
        memcpy(pProvisioningCert, pCache->stData.stCertificateInstallationReq.OEMProvisioningCert.bytes, (*pLen));
        bRst = true;
    }
    
    return(bRst);
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_RetryCounter
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月22日  18:10:39
 * @Description    :      ISO15118 协议 设置 RetryCounter 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      i16RetryCounter           需要等待n天重新获取证书
 * @Return         :      
 * @Notice         :      RetryCounter >  0 : RetryCounter天后
                          RetryCounter =  0 : 立刻
                          RetryCounter = -1 : 不用重新获取
*******************************************************************************/
bool sV2gIso1Set_RetryCounter(i32 i32Index, i16 i16RetryCounter)
{
    bool bRst = false;
    i32 i;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        
        //<ISO15118-2:2014.pdf> Table 43
        if((pCache->stData.stCertificateUpdateRes.ResponseCode == iso1responseCodeType_FAILED_NoCertificateAvailable)
        || (pCache->stData.stCertificateUpdateRes.ResponseCode == iso1responseCodeType_FAILED_ContractCanceled))
        {
            pCache->stData.stCertificateUpdateRes.RetryCounter = i16RetryCounter;
            pCache->stData.stCertificateUpdateRes.RetryCounter_isUsed = 1;
            bRst = true;
        }
    }
    
    return(bRst);
}












/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_GenChallenge
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月23日  14:21:16
 * @Description    :      ISO15118 协议 设置 GenChallenge 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      i32Len                    GenChallenge长度
 * @Input          :      pGenChallenge             指向要写入的GenChallenge的u8指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Set_GenChallenge(i32 i32Index, i32 i32Len, const u8* pGenChallenge)
{
    bool bRst = false;
    i32 i;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        if((i32Len > 0) && (i32Len <= iso1PaymentDetailsResType_GenChallenge_BYTES_SIZE))
        {
            pCache->stData.stPaymentDetailsRes.GenChallenge.bytesLen = i32Len;
            memcpy(pCache->stData.stPaymentDetailsRes.GenChallenge.bytes, pGenChallenge, i32Len);
            bRst = true;
        }
    }
    
    return(bRst);
}














/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_RequestedEnergyTransferMode
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月20日  17:34:10
 * @Description    :      ISO15118 协议 获取 RequestedEnergyTransferMode 字段 api接口函数
 * @Input          :      i32Index                          外设号
 * @Input          :      pRequestedEnergyTransferMode      指向储存RequestedEnergyTransferMode的i32指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_RequestedEnergyTransferMode(i32 i32Index, i32 *pRequestedEnergyTransferMode)
{
    bool bRst = false;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pRequestedEnergyTransferMode != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        (*pRequestedEnergyTransferMode) = pCache->stData.stChargeParamDiscoveryReq.RequestedEnergyTransferMode;
        bRst = true;
    }
    
    return(bRst);
}












/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_MaxEntriesSAScheduleTuple
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月27日  15:47:10
 * @Description    :      ISO15118 协议 获取 MaxEntriesSAScheduleTuple 字段 api接口函数
 * @Input          :      i32Index                          外设号
 * @Input          :      pMaxEntriesSAScheduleTuple        指向储存pMaxEntriesSAScheduleTuple的i32指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_MaxEntriesSAScheduleTuple(i32 i32Index, i32 *pMaxEntriesSAScheduleTuple)
{
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pMaxEntriesSAScheduleTuple != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        
        if(pCache->eId >= eV2gIso1MsgIdChargeParameterDiscovery)
        {
            if(pCache->stData.stChargeParamDiscoveryReq.MaxEntriesSAScheduleTuple_isUsed == 1)
            {
                (*pMaxEntriesSAScheduleTuple) = pCache->stData.stChargeParamDiscoveryReq.MaxEntriesSAScheduleTuple;
                
                return(true);
            }
        }
    }
    
    return(false);
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_DepartureTime
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月27日  15:47:06
 * @Description    :      ISO15118 协议 获取 DepartureTime 字段 api接口函数
 * @Input          :      i32Index                          外设号
 * @Input          :      pDepartureTime                    指向储存pDepartureTime的i32指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_DepartureTime(i32 i32Index, i32 *pDepartureTime)
{
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pDepartureTime != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        
        if(pCache->eId >= eV2gIso1MsgIdChargeParameterDiscovery)
        {
            if(pCache->stData.stChargeParamDiscoveryReq.AC_EVChargeParameter_isUsed == 1u)
            {
                (*pDepartureTime) = pCache->stData.stChargeParamDiscoveryReq.AC_EVChargeParameter.DepartureTime;
                
                return(true);
            }
            
            if(pCache->stData.stChargeParamDiscoveryReq.DC_EVChargeParameter_isUsed == 1u)
            {
                (*pDepartureTime) = pCache->stData.stChargeParamDiscoveryReq.DC_EVChargeParameter.DepartureTime;
                
                return(true);
            }
        }
    }
    
    return(false);
}













/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_EAmount
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月27日  15:34:38
 * @Description    :      ISO15118 协议 获取 EAmount 字段 api接口函数
 * @Input          :      i32Index              外设号
 * @Input          :      pEAmount              指向储存的PhysicalValue结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_EAmount(i32 i32Index, struct iso1PhysicalValueType *pEAmount)
{
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pEAmount != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        if(pCache->eId >= eV2gIso1MsgIdChargeParameterDiscovery)
        {
            if(pCache->stData.stChargeParamDiscoveryReq.AC_EVChargeParameter_isUsed == 1u)
            {
                memcpy(pEAmount, &pCache->stData.stChargeParamDiscoveryReq.AC_EVChargeParameter.EAmount, sizeof(struct iso1PhysicalValueType));
                
                return(true);
            }
        }
    }
    
    return(false);
}









/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_EVMaxVoltage
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月27日  15:35:00
 * @Description    :      ISO15118 协议 获取 EVMaxVoltage 字段 api接口函数
 * @Input          :      i32Index              外设号
 * @Input          :      pEVMaxVoltage         指向储存的PhysicalValue结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_EVMaxVoltage(i32 i32Index, struct iso1PhysicalValueType *pEVMaxVoltage)
{
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pEVMaxVoltage != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        if(pCache->eId >= eV2gIso1MsgIdChargeParameterDiscovery)
        {
            if(pCache->stData.stChargeParamDiscoveryReq.AC_EVChargeParameter_isUsed == 1u)
            {
                memcpy(pEVMaxVoltage, &pCache->stData.stChargeParamDiscoveryReq.AC_EVChargeParameter.EVMaxVoltage, sizeof(struct iso1PhysicalValueType));
                
                return(true);
            }
        }
    }
    
    return(false);
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_EVMaxCurrent
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月27日  15:35:31
 * @Description    :      ISO15118 协议 获取 EVMaxCurrent 字段 api接口函数
 * @Input          :      i32Index              外设号
 * @Input          :      pEVMaxCurrent         指向储存的PhysicalValue结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_EVMaxCurrent(i32 i32Index, struct iso1PhysicalValueType *pEVMaxCurrent)
{
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pEVMaxCurrent != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        if(pCache->eId >= eV2gIso1MsgIdChargeParameterDiscovery)
        {
            if(pCache->stData.stChargeParamDiscoveryReq.AC_EVChargeParameter_isUsed == 1u)
            {
                memcpy(pEVMaxCurrent, &pCache->stData.stChargeParamDiscoveryReq.AC_EVChargeParameter.EVMaxCurrent, sizeof(struct iso1PhysicalValueType));
                
                return(true);
            }
        }
    }
    
    return(false);
}












/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_EVMinCurrent
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月27日  15:36:15
 * @Description    :      ISO15118 协议 获取 EVMinCurrent 字段 api接口函数
 * @Input          :      i32Index              外设号
 * @Input          :      pEVMinCurrent         指向储存的PhysicalValue结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_EVMinCurrent(i32 i32Index, struct iso1PhysicalValueType *pEVMinCurrent)
{
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pEVMinCurrent != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        if(pCache->eId >= eV2gIso1MsgIdChargeParameterDiscovery)
        {
            if(pCache->stData.stChargeParamDiscoveryReq.AC_EVChargeParameter_isUsed == 1u)
            {
                memcpy(pEVMinCurrent, &pCache->stData.stChargeParamDiscoveryReq.AC_EVChargeParameter.EVMinCurrent, sizeof(struct iso1PhysicalValueType));
                
                return(true);
            }
        }
    }
    
    return(false);
}















/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_EVEnergyCapacity
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月20日  17:40:58
 * @Description    :      ISO15118 协议 获取 EVEnergyCapacity 字段 api接口函数
 * @Input          :      i32Index               外设号
 * @Input          :      pEVEnergyCapacity      指向储存PhysicalValueType结构体的指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_EVEnergyCapacity(i32 i32Index, struct iso1PhysicalValueType *pEVEnergyCapacity)
{
    bool bRst = false;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pEVEnergyCapacity != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        if(pCache->eId >= eV2gIso1MsgIdChargeParameterDiscovery)
        {
            if((pCache->stData.stChargeParamDiscoveryReq.DC_EVChargeParameter_isUsed == 1u)
            && (pCache->stData.stChargeParamDiscoveryReq.DC_EVChargeParameter.EVEnergyCapacity_isUsed == 1u))
            {
                memcpy(pEVEnergyCapacity, &pCache->stData.stChargeParamDiscoveryReq.DC_EVChargeParameter.EVEnergyCapacity, sizeof(struct iso1PhysicalValueType));
                bRst = true;
            }
        }
    }
    
    return(bRst);
}











/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_EVEnergyRequest
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月20日  17:55:22
 * @Description    :      ISO15118 协议 获取 EVEnergyRequest 字段 api接口函数
 * @Input          :      i32Index               外设号
 * @Input          :      pEVEnergyRequest      指向储存PhysicalValueType结构体的指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_EVEnergyRequest(i32 i32Index, struct iso1PhysicalValueType *pEVEnergyRequest)
{
    bool bRst = false;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pEVEnergyRequest != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        if(pCache->eId >= eV2gIso1MsgIdChargeParameterDiscovery)
        {
            if((pCache->stData.stChargeParamDiscoveryReq.DC_EVChargeParameter_isUsed == 1u)
            && (pCache->stData.stChargeParamDiscoveryReq.DC_EVChargeParameter.EVEnergyRequest_isUsed == 1u))
            {
                memcpy(pEVEnergyRequest, &pCache->stData.stChargeParamDiscoveryReq.DC_EVChargeParameter.EVEnergyRequest, sizeof(struct iso1PhysicalValueType));
                bRst = true;
            }
        }
    }
    
    return(bRst);
}











/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_FullSOC
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月20日  17:57:13
 * @Description    :      ISO15118 协议 获取 FullSOC 字段 api接口函数
 * @Input          :      i32Index          外设号
 * @Input          :      pFullSOC          指向储存FullSOC的i8指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_FullSOC(i32 i32Index, i8 *pFullSOC)
{
    bool bRst = false;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pFullSOC != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        if(pCache->eId >= eV2gIso1MsgIdChargeParameterDiscovery)
        {
            if((pCache->stData.stChargeParamDiscoveryReq.DC_EVChargeParameter_isUsed == 1u)
            && (pCache->stData.stChargeParamDiscoveryReq.DC_EVChargeParameter.FullSOC_isUsed == 1u))
            {
                (*pFullSOC) = pCache->stData.stChargeParamDiscoveryReq.DC_EVChargeParameter.FullSOC;
                bRst = true;
            }
        }
        
    }
    
    return(bRst);
}











/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_BulkSOC
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月20日  17:59:04
 * @Description    :      ISO15118 协议 获取 BulkSOC 字段 api接口函数
 * @Input          :      i32Index          外设号
 * @Input          :      pBulkSOC          指向储存BulkSOC的i8指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_BulkSOC(i32 i32Index, i8 *pBulkSOC)
{
    bool bRst = false;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pBulkSOC != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        if(pCache->eId >= eV2gIso1MsgIdChargeParameterDiscovery)
        {
            if((pCache->stData.stChargeParamDiscoveryReq.DC_EVChargeParameter_isUsed == 1u)
            && (pCache->stData.stChargeParamDiscoveryReq.DC_EVChargeParameter.BulkSOC_isUsed == 1u))
            {
                (*pBulkSOC) = pCache->stData.stChargeParamDiscoveryReq.DC_EVChargeParameter.BulkSOC;
                bRst = true;
            }
        }
    }
    
    return(bRst);
}












/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_SAScheduleList
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月20日  19:19:13
 * @Description    :      ISO15118 协议 设置 SAScheduleList 字段 api接口函数
 * @Input          :      i32Index              外设号
 * @Input          :      bClearFlag            清空SAScheduleList字段标志
 * @Input          :      pSAScheduleTuple      指向储存SAScheduleTuple结构体的指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Set_SAScheduleList(i32 i32Index, bool bClearFlag, const struct iso1SAScheduleTupleType *pSAScheduleTuple)
{
    bool bRst = false;
    i32  i32ArrayIndex;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        if(bClearFlag == true)
        {
            //本函数逻辑相当于对SAScheduleList字段的 SAScheduleTuple[n]元素 做了一个增加接口函数
            //但是删除函数很难实现，因为移动 SAScheduleTuple[n] 的逻辑很麻烦, 
            //为了程序的简洁，就增加了 bClearFlag 标志 可以清除所有的 SAScheduleTuple[] 元素，不必实现删除逻辑
            memset(&pCache->stData.stChargeParamDiscoveryRes.SAScheduleList, 0, sizeof(struct iso1SAScheduleListType));
        }
        
        if(pSAScheduleTuple != NULL)
        {
            i32ArrayIndex = pCache->stData.stChargeParamDiscoveryRes.SAScheduleList.SAScheduleTuple.arrayLen;
            if(i32ArrayIndex < (iso1SAScheduleListType_SAScheduleTuple_ARRAY_SIZE - 1))
            {
                memcpy(&pCache->stData.stChargeParamDiscoveryRes.SAScheduleList.SAScheduleTuple.array[i32ArrayIndex], pSAScheduleTuple, sizeof(struct iso1SAScheduleTupleType));
                pCache->stData.stChargeParamDiscoveryRes.SAScheduleList.SAScheduleTuple.arrayLen++;
            }
            pCache->stData.stChargeParamDiscoveryRes.SAScheduleList_isUsed = 1u;
            bRst = true;
        }
    }
    
    return(bRst);
}









/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_EVSENominalVoltage
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月23日  16:55:59
 * @Description    :      ISO15118 协议 设置 EVSENominalVoltage 字段 api接口函数
 * @Input          :      i32Index                      外设号
 * @Input          :      pEVSENominalVoltage           指向储存PhysicalValue结构体的指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Set_EVSENominalVoltage(i32 i32Index, const struct iso1PhysicalValueType *pEVSENominalVoltage)
{
    bool bRst = false;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pEVSENominalVoltage != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        memcpy(&pCache->stData.stChargeParamDiscoveryRes.AC_EVSEChargeParameter.EVSENominalVoltage, pEVSENominalVoltage, sizeof(struct iso1PhysicalValueType));
        pCache->stData.stChargeParamDiscoveryRes.AC_EVSEChargeParameter_isUsed = 1u;
        bRst = true;
    }
    
    return(bRst);
}














/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_EVSEMinimumCurrentLimit
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月20日  19:29:55
 * @Description    :      ISO15118 协议 设置 EVSEMinimumCurrentLimit 字段 api接口函数
 * @Input          :      i32Index                      外设号
 * @Input          :      pEVSEMinimumCurrentLimit      指向储存PhysicalValue结构体的指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Set_EVSEMinimumCurrentLimit(i32 i32Index, const struct iso1PhysicalValueType *pEVSEMinimumCurrentLimit)
{
    bool bRst = false;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pEVSEMinimumCurrentLimit != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        memcpy(&pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEMinimumCurrentLimit, pEVSEMinimumCurrentLimit, sizeof(struct iso1PhysicalValueType));
        pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter_isUsed = 1u;
        bRst = true;
    }
    
    return(bRst);
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_EVSEMinimumVoltageLimit
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月20日  19:32:00
 * @Description    :      ISO15118 协议 设置 EVSEMinimumVoltageLimit 字段 api接口函数
 * @Input          :      i32Index                      外设号
 * @Input          :      pEVSEMinimumVoltageLimit      指向储存PhysicalValue结构体的指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Set_EVSEMinimumVoltageLimit(i32 i32Index, const struct iso1PhysicalValueType *pEVSEMinimumVoltageLimit)
{
    bool bRst = false;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pEVSEMinimumVoltageLimit != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        memcpy(&pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEMinimumVoltageLimit, pEVSEMinimumVoltageLimit, sizeof(struct iso1PhysicalValueType));
        pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter_isUsed = 1u;
        bRst = true;
    }
    
    return(bRst);
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_EVSECurrentRegulationTolerance
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月20日  19:44:10
 * @Description    :      ISO15118 协议 设置 EVSECurrentRegulationTolerance 字段 api接口函数
 * @Input          :      i32Index                          外设号
 * @Input          :      pEVSECurrentRegulationTolerance   指向储存PhysicalValue结构体的指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Set_EVSECurrentRegulationTolerance(i32 i32Index, const struct iso1PhysicalValueType *pEVSECurrentRegulationTolerance)
{
    bool bRst = false;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter_isUsed = 1u;
        if(pEVSECurrentRegulationTolerance != NULL)
        {
            memcpy(&pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSECurrentRegulationTolerance, pEVSECurrentRegulationTolerance, sizeof(struct iso1PhysicalValueType));
            pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSECurrentRegulationTolerance_isUsed = 1u;
        }
        else
        {
            pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSECurrentRegulationTolerance_isUsed = 0u;
        }
        bRst = true;
    }
    
    return(bRst);
}











/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_EVSEPeakCurrentRipple
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月20日  19:50:53
 * @Description    :      ISO15118 协议 设置 EVSEPeakCurrentRipple 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      pEVSEPeakCurrentRipple    指向储存PhysicalValue结构体的指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Set_EVSEPeakCurrentRipple(i32 i32Index, const struct iso1PhysicalValueType *pEVSEPeakCurrentRipple)
{
    bool bRst = false;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pEVSEPeakCurrentRipple != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        memcpy(&pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEPeakCurrentRipple, pEVSEPeakCurrentRipple, sizeof(struct iso1PhysicalValueType));
        pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter_isUsed = 1u;
        bRst = true;
    }
    
    return(bRst);
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_EVSEEnergyToBeDelivered
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月20日  19:53:25
 * @Description    :      ISO15118 协议 设置 EVSEEnergyToBeDelivered 字段 api接口函数
 * @Input          :      i32Index                      外设号
 * @Input          :      pEVSEEnergyToBeDelivered      指向储存PhysicalValue结构体的指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Set_EVSEEnergyToBeDelivered(i32 i32Index, const struct iso1PhysicalValueType *pEVSEEnergyToBeDelivered)
{
    bool bRst = false;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter_isUsed = 1u;
        if(pEVSEEnergyToBeDelivered != NULL)
        {
            memcpy(&pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEEnergyToBeDelivered, pEVSEEnergyToBeDelivered, sizeof(struct iso1PhysicalValueType));
            pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEEnergyToBeDelivered_isUsed = 1u;
        }
        else
        {
            pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEEnergyToBeDelivered_isUsed = 0u;
        }
        bRst = true;
    }
    
    return(bRst);
}











/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_ChargeProgress
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月20日  19:55:59
 * @Description    :      ISO15118 协议 获取 ChargeProgress 字段 api接口函数
 * @Input          :      i32Index              外设号
 * @Input          :      pChargeProgress       指向储存pChargeProgress的i32指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_ChargeProgress(i32 i32Index, i32 *pChargeProgress)
{
    bool bRst = false;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pChargeProgress != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        if(pCache->eId >= eV2gIso1MsgIdPowerDeliveryStart)
        {
            (*pChargeProgress) = pCache->stData.stPowerDeliveryReq.ChargeProgress;
            bRst = true;
        }
        
    }
    
    return(bRst);
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_ChargingProfile
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月20日  20:00:22
 * @Description    :      ISO15118 协议 获取 ChargeProgress 字段 api接口函数
 * @Input          :      i32Index              外设号
 * @Input          :      pChargingProfile      指向储存ChargingProfile结构体的指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_ChargingProfile(i32 i32Index, struct iso1ChargingProfileType *pChargingProfile)
{
    bool bRst = false;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pChargingProfile != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        if(pCache->eId >= eV2gIso1MsgIdPowerDeliveryStart)
        {
            if(pCache->stData.stPowerDeliveryReq.ChargingProfile_isUsed == 1u)
            {
                memcpy(pChargingProfile, &pCache->stData.stPowerDeliveryReq.ChargingProfile, sizeof(struct iso1ChargingProfileType));
                bRst = true;
            }
        }
    }
    
    return(bRst);
}











/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_RemainingTimeToFullSoC
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月20日  20:02:33
 * @Description    :      ISO15118 协议 获取 RemainingTimeToFullSoC 字段 api接口函数
 * @Input          :      i32Index                      外设号
 * @Input          :      pRemainingTimeToFullSoC      指向储存PhysicalValue结构体的指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_RemainingTimeToFullSoC(i32 i32Index, struct iso1PhysicalValueType *pRemainingTimeToFullSoC)
{
    bool bRst = false;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pRemainingTimeToFullSoC != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        if(pCache->stData.stCurrentDemandReq.RemainingTimeToFullSoC_isUsed == 1u)
        {
            memcpy(pRemainingTimeToFullSoC, &pCache->stData.stCurrentDemandReq.RemainingTimeToFullSoC, sizeof(struct iso1PhysicalValueType));
            bRst = true;
        }
    }
    
    return(bRst);
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_RemainingTimeToBulkSoC
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月20日  20:04:23
 * @Description    :      ISO15118 协议 获取 RemainingTimeToBulkSoC 字段 api接口函数
 * @Input          :      i32Index                      外设号
 * @Input          :      pRemainingTimeToBulkSoC       指向储存PhysicalValue结构体的指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_RemainingTimeToBulkSoC(i32 i32Index, struct iso1PhysicalValueType *pRemainingTimeToBulkSoC)
{
    bool bRst = false;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pRemainingTimeToBulkSoC != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        if(pCache->stData.stCurrentDemandReq.RemainingTimeToBulkSoC_isUsed == 1u)
        {
            memcpy(pRemainingTimeToBulkSoC, &pCache->stData.stCurrentDemandReq.RemainingTimeToBulkSoC, sizeof(struct iso1PhysicalValueType));
            bRst = true;
        }
    }
    
    return(bRst);
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_EVSEPresentCurrent
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月20日  20:05:43
 * @Description    :      ISO15118 协议 设置 EVSEPresentCurrent 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      pEVSEPresentCurrent       指向储存PhysicalValue结构体的指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Set_EVSEPresentCurrent(i32 i32Index, const struct iso1PhysicalValueType *pEVSEPresentCurrent)
{
    bool bRst = false;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pEVSEPresentCurrent != NULL) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        memcpy(&pCache->stData.stCurrentDemandRes.EVSEPresentCurrent, pEVSEPresentCurrent, sizeof(struct iso1PhysicalValueType));
        bRst = true;
    }
    
    return(bRst);
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_EVSEVoltageLimitAchieved
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月25日  16:53:35
 * @Description    :      ISO15118 协议 设置 EVSEVoltageLimitAchieved 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      bAchievedFlag             限压标识
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Set_EVSEVoltageLimitAchieved(i32 i32Index, bool bAchievedFlag)
{
    bool bRst = false;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        pCache->stData.stCurrentDemandRes.EVSEVoltageLimitAchieved = bAchievedFlag;
        bRst = true;
    }
    
    return(bRst);
}











/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_EVSECurrentLimitAchieved
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月25日  16:55:17
 * @Description    :      ISO15118 协议 设置 EVSECurrentLimitAchieved 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      bAchievedFlag             限压标识
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Set_EVSECurrentLimitAchieved(i32 i32Index, bool bAchievedFlag)
{
    bool bRst = false;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        pCache->stData.stCurrentDemandRes.EVSECurrentLimitAchieved = bAchievedFlag;
        bRst = true;
    }
    
    return(bRst);
}












/*******************************************************************************
 * @FunctionName   :      sV2gIso1Set_EVSEPowerLimitAchieved
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月25日  16:55:30
 * @Description    :      ISO15118 协议 设置 EVSEPowerLimitAchieved 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      bAchievedFlag             限压标识
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Set_EVSEPowerLimitAchieved(i32 i32Index, bool bAchievedFlag)
{
    bool bRst = false;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        pCache->stData.stCurrentDemandRes.EVSEPowerLimitAchieved = bAchievedFlag;
        bRst = true;
    }
    
    return(bRst);
}









/*******************************************************************************
 * @FunctionName   :      sV2gIso1Get_ChargingSession
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月25日  17:14:30
 * @Description    :      ISO15118 协议 获取 ChargingSession 字段 api接口函数
 * @Input          :      i32Index                  外设号
 * @Input          :      pChargingSession          充电会话状态
 *                                                  0 : 会话终止    1 : 会话暂停
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Get_ChargingSession(i32 i32Index, i8 *pChargingSession)
{
    bool bRst = false;
    stV2gIso1Cache_t *pCache = NULL;
    
    if((i32Index < cMse102xDevNum) && (pV2gIso1Cache[i32Index] != NULL))
    {
        pCache = pV2gIso1Cache[i32Index];
        (*pChargingSession) = pCache->stData.stSessionStopReq.ChargingSession;
        bRst = true;
    }
    
    return(bRst);
}












