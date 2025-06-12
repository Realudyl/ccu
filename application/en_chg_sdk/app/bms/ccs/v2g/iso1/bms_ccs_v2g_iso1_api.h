/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     bms_ccs_v2g_iso1_api.h
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
#ifndef _bms_ccs_v2g_iso1_api_H
#define _bms_ccs_v2g_iso1_api_H
#include "bms_ccs_v2g_iso1.h"

















//v2g消息 msgId 字段读取
extern bool sV2gIso1Get_MsgId(i32 i32Index, eV2gIso1MsgId_t *pId);

//多消息公用的api函数
extern bool sV2gIso1Set_ResponseCode(i32 i32Index, eV2gIso1MsgId_t eId, i32 i32ResponseCode);
extern bool sV2gIso1Set_EVSEID(i32 i32Index, eV2gIso1MsgId_t eId, i32 i32Len, const i32 *pEvseId);
extern bool sV2gIso1Set_EVSEProcessing(i32 i32Index, eV2gIso1MsgId_t eId, i32 i32EvseProcessing);
extern bool sV2gIso1Get_ServiceId(i32 i32Index, i32 *pServiceId);
extern bool sV2gIso1Get_DC_EVStatus(i32 i32Index, struct iso1DC_EVStatusType *pDC_EVStatus);
extern bool sV2gIso1Get_EVMaximumCurrentLimit(i32 i32Index, struct iso1PhysicalValueType *pEVMaximumCurrentLimit);
extern bool sV2gIso1Get_EVMaximumPowerLimit(i32 i32Index, struct iso1PhysicalValueType *pEVMaximumPowerLimit);
extern bool sV2gIso1Get_EVMaximumVoltageLimit(i32 i32Index, struct iso1PhysicalValueType *pEVMaximumVoltageLimit);
extern bool sV2gIso1Set_AC_EVSEStatus(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1AC_EVSEStatusType *pAC_EVSEStatus);
extern bool sV2gIso1Set_EVSEMaxCurrent(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1PhysicalValueType *pEVSEMaxCurrent);
extern bool sV2gIso1Set_DC_EVSEStatus(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1DC_EVSEStatusType *pDC_EVSEStatus);
extern bool sV2gIso1Set_EVSEMaximumCurrentLimit(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1PhysicalValueType *pEVSEMaximumCurrentLimit);
extern bool sV2gIso1Set_EVSEMaximumPowerLimit(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1PhysicalValueType *pEVSEMaximumPowerLimit);
extern bool sV2gIso1Set_EVSEMaximumVoltageLimit(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1PhysicalValueType *pEVSEMaximumVoltageLimit);
extern bool sV2gIso1Get_EVTargetVoltage(i32 i32Index, struct iso1PhysicalValueType *pEVTargetVoltage);
extern bool sV2gIso1Get_EVTargetCurrent(i32 i32Index, struct iso1PhysicalValueType *pEVTargetCurrent);
extern bool sV2gIso1Set_EVSEPresentVoltage(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1PhysicalValueType *pEVSEPresentVoltage);
extern bool sV2gIso1Get_BulkChargingComplete(i32 i32Index, i32 *pBulkChargingComplete);
extern bool sV2gIso1Get_ChargingComplete(i32 i32Index, i32 *pChargingComplete);
extern bool sV2gIso1Get_Id(i32 i32Index, eV2gIso1MsgId_t eId, i32 *pLen, i32 *pId);
extern bool sV2gIso1Get_ListOfRootCertificateIDs(i32 i32Index, eV2gIso1MsgId_t eId, struct iso1ListOfRootCertificateIDsType *pListOfRootCertificateIDs);
extern bool sV2gIso1Get_ContractSignatureCertChain(i32 i32Index, eV2gIso1MsgId_t eId, struct iso1CertificateChainType *pSubCertificateChain);
extern bool sV2gIso1Get_eMAID(i32 i32Index, eV2gIso1MsgId_t eId, i32 *pEMAIDLen, i32 *pEMAID);
extern bool sV2gIso1Set_SAProvisioningCertificateChain(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1CertificateChainType *pCertificateChain);
extern bool sV2gIso1Set_ContractSignatureCertChain(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1CertificateChainType *pCertificateChain);
extern bool sV2gIso1Set_ContractSignatureEncryptedPrivateKey(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1ContractSignatureEncryptedPrivateKeyType* pContractSignatureEncryptedPrivateKey);
extern bool sV2gIso1Set_DHpublickey(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1DiffieHellmanPublickeyType* pDiffieHellmanPublickey);
extern bool sV2gIso1Set_eMAID(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1EMAIDType* pEMAID);
extern bool sV2gIso1Set_MeterInfo(i32 i32Index, eV2gIso1MsgId_t eId, const struct iso1MeterInfoType* pMeterInfo);
extern bool sV2gIso1Set_ReceiptRequired(i32 i32Index, eV2gIso1MsgId_t eId, bool bReceiptRequired);



//SupportedAppProtocol
extern bool sV2gIso1Get_SupportedAppProtocol(i32 i32Index);

//SessionSetup
extern bool sV2gIso1Get_EVCCID(i32 i32Index, i32 *pLen, u8 *pEvccId);

//ServiceDiscovery
extern bool sV2gIso1Get_ServiceScope(i32 i32Index, i32 *pScopeLen, i32 *pServiceScope);
extern bool sV2gIso1Get_ServiceCategory(i32 i32Index, i32 *pServiceCategory);
extern bool sV2gIso1Set_PaymentOptionList(i32 i32Index, i32 i32PaymentOptionNum, const iso1paymentOptionType *pPaymentOption);
extern bool sV2gIso1Set_ChargeService(i32 i32Index, u16 u16ServiceId, const i32 *pName, i32 i32NameLen, i32 i32Category, const i32 *pScope, i32 i32ScopeLen, bool bFree, const u8 *pEnergyTsfMode, u8 u8ModeNum);
extern bool sV2gIso1Set_ServiceList(i32 i32Index, i32 i32ServiceNum, const struct iso1ServiceType *pService);

//ServiceDetail
extern bool sV2gIso1Set_ServiceParameterList(i32 i32Index, i32 i32ParameterSetNum, const struct iso1ParameterSetType *pParameterSet);

//PaymentServiceSelection
extern bool sV2gIso1Get_SelectedPaymentOption(i32 i32Index, i32 *pSelectedPaymentOption);

//CertificateInstallation
extern bool sV2gIso1Get_OEMProvisioningCert(i32 i32Index, i32 *pLen, u8 *pProvisioningCert);

//CertificateUpdate
extern bool sV2gIso1Set_RetryCounter(i32 i32Index, i16 i16RetryCounter);

//PaymentDetails
extern bool sV2gIso1Set_GenChallenge(i32 i32Index, i32 i32Len, const u8* pGenChallenge);

//ContractAuthentication---无

//ChargeParameterDiscovery
extern bool sV2gIso1Get_RequestedEnergyTransferMode(i32 i32Index, i32 *pRequestedEnergyTransferMode);
extern bool sV2gIso1Get_MaxEntriesSAScheduleTuple(i32 i32Index, i32 *pMaxEntriesSAScheduleTuple);
extern bool sV2gIso1Get_DepartureTime(i32 i32Index, i32 *pDepartureTime);
extern bool sV2gIso1Get_EAmount(i32 i32Index, struct iso1PhysicalValueType *pEAmount);
extern bool sV2gIso1Get_EVMaxVoltage(i32 i32Index, struct iso1PhysicalValueType *pEVMaxVoltage);
extern bool sV2gIso1Get_EVMaxCurrent(i32 i32Index, struct iso1PhysicalValueType *pEVMaxCurrent);
extern bool sV2gIso1Get_EVMinCurrent(i32 i32Index, struct iso1PhysicalValueType *pEVMinCurrent);
extern bool sV2gIso1Get_EVEnergyCapacity(i32 i32Index, struct iso1PhysicalValueType *pEVEnergyCapacity);
extern bool sV2gIso1Get_EVEnergyRequest(i32 i32Index, struct iso1PhysicalValueType *pEVEnergyRequest);
extern bool sV2gIso1Get_FullSOC(i32 i32Index, i8 *pFullSOC);
extern bool sV2gIso1Get_BulkSOC(i32 i32Index, i8 *pBulkSOC);
extern bool sV2gIso1Set_SAScheduleList(i32 i32Index, bool bClearFlag, const struct iso1SAScheduleTupleType *pSAScheduleTuple);
extern bool sV2gIso1Set_EVSENominalVoltage(i32 i32Index, const struct iso1PhysicalValueType *pEVSENominalVoltage);
extern bool sV2gIso1Set_EVSEMinimumCurrentLimit(i32 i32Index, const struct iso1PhysicalValueType *pEVSEMinimumCurrentLimit);
extern bool sV2gIso1Set_EVSEMinimumVoltageLimit(i32 i32Index, const struct iso1PhysicalValueType *pEVSEMinimumVoltageLimit);
extern bool sV2gIso1Set_EVSECurrentRegulationTolerance(i32 i32Index, const struct iso1PhysicalValueType *pEVSECurrentRegulationTolerance);
extern bool sV2gIso1Set_EVSEPeakCurrentRipple(i32 i32Index, const struct iso1PhysicalValueType *pEVSEPeakCurrentRipple);
extern bool sV2gIso1Set_EVSEEnergyToBeDelivered(i32 i32Index, const struct iso1PhysicalValueType *pEVSEEnergyToBeDelivered);

//CableCheck----无,都在公用api里面

//PreCharge-----无,都在公用api里面

//PowerDelivery
extern bool sV2gIso1Get_ChargeProgress(i32 i32Index, i32 *pReadyToChargeState);
extern bool sV2gIso1Get_ChargingProfile(i32 i32Index, struct iso1ChargingProfileType *pChargingProfile);

//ChargingStatus----无,都在公用api里面

//CurrentDemand
extern bool sV2gIso1Get_RemainingTimeToFullSoC(i32 i32Index, struct iso1PhysicalValueType *pRemainingTimeToFullSoC);
extern bool sV2gIso1Get_RemainingTimeToBulkSoC(i32 i32Index, struct iso1PhysicalValueType *pRemainingTimeToBulkSoC);
extern bool sV2gIso1Set_EVSEPresentCurrent(i32 i32Index, const struct iso1PhysicalValueType *pEVSEPresentCurrent);
extern bool sV2gIso1Set_EVSEVoltageLimitAchieved(i32 i32Index, bool bAchievedFlag);
extern bool sV2gIso1Set_EVSECurrentLimitAchieved(i32 i32Index, bool bAchievedFlag);
extern bool sV2gIso1Set_EVSEPowerLimitAchieved(i32 i32Index, bool bAchievedFlag);

//MeteringReceipt-----无,都在公用api里面

//WeldingDetection-----无,都在公用api里面

//SessionStop
extern bool sV2gIso1Get_ChargingSession(i32 i32Index, i8 *pChargingSession);













#endif
