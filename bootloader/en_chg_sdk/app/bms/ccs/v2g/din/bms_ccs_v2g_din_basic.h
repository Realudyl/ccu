/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   bms_ccs_v2g_din_basic.h
* Description                           :   DIN70121 协议实现 之 V2G消息内各个字段的处理函数实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-05-24
* notice                                :   
****************************************************************************************************/
#ifndef _bms_ccs_v2g_din_basic_h_
#define _bms_ccs_v2g_din_basic_h_
#include "bms_ccs_v2g_din.h"

























//V2G DIN70121 PhysicalValueType 复杂数据类型处理函数----公用
extern f32  sV2gDinGetPhyValue(struct dinPhysicalValueType *pValue, char *pUnit);
extern void sV2gDinSetPhyValue(struct dinPhysicalValueType *pValue, f32 f32Value, dinunitSymbolType eUnit);
extern bool sV2gDinCmpPhyValue(struct dinPhysicalValueType *pBig, struct dinPhysicalValueType *pSmall, bool bEqualFlag);


//V2G DIN70121 SessionId 处理----公用
extern void sV2gDinMakeSessionId(u8 *pSessionId, u16 u16Len);
extern bool sV2gDinFillSessionId(struct dinEXIDocument *pDoc, const u8 *pId, i32 i32IdLen);
extern void sV2gDinCompSessionId(i32 i32DevIndex, struct dinEXIDocument *pDoc, stV2gDinCache_t *pCache, dinresponseCodeType *pCode);

//V2G DIN70121 FAILED_SequenceError 错误检查
extern bool sV2gDinMsgSequenceErrorCheck(i32 i32DevIndex, eV2gDinMsgId_t eMsgId);

//V2G DIN70121 cp状态错误 检查
extern bool sV2gDinCpStateErrorCheck(i32 i32DevIndex, eChgCpState_t eCpState);




//for SessionSetup
extern void sV2gDinCompSessionIdForSessionSetup(i32 i32DevIndex, u8 *pSessionIdEvse, u8 *pSessionIdEv, i32 i32IdLen);

//for ServicePaymentSelection
extern bool sV2gDinProcSelectedPaymentOption(i32 i32DevIndex, stV2gDinCache_t *pCache, dinresponseCodeType *pCode, dinpaymentOptionType eType);
extern bool sV2gDinProcSelectedServiceList(i32 i32DevIndex, stV2gDinCache_t *pCache, dinresponseCodeType *pCode, struct dinSelectedServiceListType *pList);

//for DinChargeParameterDiscovery
extern bool sV2gDinProcPublicDataChargeParameterDiscovery(stV2gDinCache_t *pCache, struct dinChargeParameterDiscoveryReqType *pReq);
extern bool sV2gDinProcEvRequestedEnergyTransferType(i32 i32DevIndex, stV2gDinCache_t *pCache, dinresponseCodeType *pCode, dinEVRequestedEnergyTransferType eType);
extern bool sV2gDinProcDcEvChargeParameter(i32 i32DevIndex, stV2gDinCache_t *pCache, dinresponseCodeType *pCode, struct dinDC_EVChargeParameterType *pDcEvParam);

//for PowerDelivery
extern bool sV2gDinProcPublicDataPowerDelivery(stV2gDinCache_t *pCache, struct dinPowerDeliveryReqType *pReq);
extern bool sV2gDinSAScheduleTupleIDCheck(i32 i32DevIndex, stV2gDinCache_t *pCache, dinresponseCodeType *pCode, i16 SAScheduleTupleID);
extern bool sV2gDinChargingProfileCheck(i32 i32DevIndex, stV2gDinCache_t *pCache, dinresponseCodeType *pCode, struct dinChargingProfileType *pChargingProfile);

//for CurrentDemand
extern bool sV2gDinProcPublicDataCurrentDemand(stV2gDinCache_t *pCache, struct dinCurrentDemandReqType *pReq);






#endif














