/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   bms_ccs_v2g_din_api.h
* Description                           :   DIN70121 协议实现 之 对外的api接口实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-05-24
* notice                                :   
****************************************************************************************************/
#ifndef _bms_ccs_v2g_din_api_h_
#define _bms_ccs_v2g_din_api_h_
#include "bms_ccs_v2g_din.h"











//v2g消息 msgId 字段读取
extern bool sV2gDinGet_MsgId(i32 i32Index, eV2gDinMsgId_t *pId);

//多消息公用的api函数
extern bool sV2gDinSet_ResponseCode(i32 i32Index, eV2gDinMsgId_t eId, i32 i32ResponseCode);
extern bool sV2gDinSet_EVSEProcessing(i32 i32Index, eV2gDinMsgId_t eId, i32 i32EvseProcessing);
extern bool sV2gDinGet_DC_EVStatus(i32 i32Index, struct dinDC_EVStatusType *pDC_EVStatus);
extern bool sV2gDinGet_EVMaximumCurrentLimit(i32 i32Index, struct dinPhysicalValueType *pEVMaximumCurrentLimit);
extern bool sV2gDinGet_EVMaximumPowerLimit(i32 i32Index, struct dinPhysicalValueType *pEVMaximumPowerLimit);
extern bool sV2gDinGet_EVMaximumVoltageLimit(i32 i32Index, struct dinPhysicalValueType *pEVMaximumVoltageLimit);
extern bool sV2gDinSet_DC_EVSEStatus(i32 i32Index, eV2gDinMsgId_t eId, struct dinDC_EVSEStatusType *pDC_EVSEStatus);
extern bool sV2gDinSet_EVSEMaximumCurrentLimit(i32 i32Index, eV2gDinMsgId_t eId, struct dinPhysicalValueType *pEVSEMaximumCurrentLimit);
extern bool sV2gDinSet_EVSEMaximumPowerLimit(i32 i32Index, eV2gDinMsgId_t eId, struct dinPhysicalValueType *pEVSEMaximumPowerLimit);
extern bool sV2gDinSet_EVSEMaximumVoltageLimit(i32 i32Index, eV2gDinMsgId_t eId, struct dinPhysicalValueType *pEVSEMaximumVoltageLimit);
extern bool sV2gDinGet_EVTargetVoltage(i32 i32Index, struct dinPhysicalValueType *pEVTargetVoltage);
extern bool sV2gDinGet_EVTargetCurrent(i32 i32Index, struct dinPhysicalValueType *pEVTargetCurrent);
extern bool sV2gDinSet_EVSEPresentVoltage(i32 i32Index, eV2gDinMsgId_t eId, struct dinPhysicalValueType *pEVSEPresentVoltage);
extern bool sV2gDinGet_BulkChargingComplete(i32 i32Index, i32 *pBulkChargingComplete);
extern bool sV2gDinGet_ChargingComplete(i32 i32Index, i32 *pChargingComplete);

//SupportedAppProtocol
extern bool sV2gDinGet_SupportedAppProtocol(i32 i32Index);

//SessionSetup
extern bool sV2gDinGet_EVCCID(i32 i32Index, i32 i32MaxLen, u8 *pEvccId);
extern bool sV2gDinSet_EVSEID(i32 i32Index, i32 i32Len, const u8 *pEvseId);

//ServiceDiscovery
extern bool sV2gDinGet_ServiceScope(i32 i32Index, i32 i32MaxLen, i32 *pServiceScope);
extern bool sV2gDinGet_ServiceCategory(i32 i32Index, i32 *pServiceCategory);
extern bool sV2gDinSet_PaymentOptions(i32 i32Index, i32 i32PaymentOptionNum, const dinpaymentOptionType *pPaymentOption);
extern bool sV2gDinSet_ChargeService(i32 i32Index, u16 u16Id, i32 *pName, i32 i32NameLen, i32 i32Category, i32 *pScope, i32 i32ScopeLen, bool bFree, i32 i32EnergyTsfType);

//ServicePaymentSelection
extern bool sV2gDinGet_SelectedPaymentOption(i32 i32Index, i32 *pSelectedPaymentOption);

//ContractAuthentication---无

//ChargeParameterDiscovery
extern bool sV2gDinGet_EVRequestedEnergyTransferType(i32 i32Index, i32 *pEvRequestedEnergyTransferType);
extern bool sV2gDinGet_EVEnergyCapacity(i32 i32Index, struct dinPhysicalValueType *pEVEnergyCapacity);
extern bool sV2gDinGet_EVEnergyRequest(i32 i32Index, struct dinPhysicalValueType *pEVEnergyRequest);
extern bool sV2gDinGet_FullSOC(i32 i32Index, i8 *pFullSOC);
extern bool sV2gDinGet_BulkSOC(i32 i32Index, i8 *pBulkSOC);
extern bool sV2gDinSet_SAScheduleList(i32 i32Index, bool bClearFlag, struct dinSAScheduleTupleType *pSAScheduleTuple);
extern bool sV2gDinSet_EVSEMinimumCurrentLimit(i32 i32Index, struct dinPhysicalValueType *pEVSEMinimumCurrentLimit);
extern bool sV2gDinSet_EVSEMinimumVoltageLimit(i32 i32Index, struct dinPhysicalValueType *pEVSEMinimumVoltageLimit);
extern bool sV2gDinSet_EVSECurrentRegulationTolerance(i32 i32Index, struct dinPhysicalValueType *pEVSECurrentRegulationTolerance);
extern bool sV2gDinSet_EVSEPeakCurrentRipple(i32 i32Index, struct dinPhysicalValueType *pEVSEPeakCurrentRipple);
extern bool sV2gDinSet_EVSEEnergyToBeDelivered(i32 i32Index, struct dinPhysicalValueType *pEVSEEnergyToBeDelivered);

//CableCheck----无,都在公用api里面
//PreCharge-----无,都在公用api里面

//PowerDelivery
extern bool sV2gDinGet_ReadyToChargeState(i32 i32Index, i32 *pReadyToChargeState);
extern bool sV2gDinGet_ChargingProfile(i32 i32Index, struct dinChargingProfileType *pChargingProfile);


//CurrentDemand
extern bool sV2gDinGet_RemainingTimeToFullSoC(i32 i32Index, struct dinPhysicalValueType *pRemainingTimeToFullSoC);
extern bool sV2gDinGet_RemainingTimeToBulkSoC(i32 i32Index, struct dinPhysicalValueType *pRemainingTimeToBulkSoC);
extern bool sV2gDinSet_EVSEPresentCurrent(i32 i32Index, struct dinPhysicalValueType *pEVSEPresentCurrent);

//WeldingDetection-----无,都在公用api里面
//SessionStop---无












#endif















