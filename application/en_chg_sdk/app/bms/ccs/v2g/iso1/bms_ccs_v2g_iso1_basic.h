/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     bms_ccs_v2g_iso1_basic.h
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2024-3-13
 * @Attention             :     
 * @Brief                 :     ISO15118 协议实现 之 V2G消息内各个字段的处理函数实现
 * 
 * @History:
 * 
 * 1.@Date: 2024-3-13
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#ifndef _bms_ccs_v2g_iso1_basic_H
#define _bms_ccs_v2g_iso1_basic_H

#include "bms_ccs_v2g_iso1.h"























//V2G ISO15118 PhysicalValueType 复杂数据类型处理函数----公用
extern f32  sV2gIso1GetPhyValue(struct iso1PhysicalValueType *pValue, char *pUnit);
extern void sV2gIso1SetPhyValue(struct iso1PhysicalValueType *pValue, f32 f32Value, iso1unitSymbolType eUnit);
extern bool sV2gIso1CmpPhyValue(struct iso1PhysicalValueType *pBig, struct iso1PhysicalValueType *pSmall, bool bEqualFlag);

//V2G ISO15118 SessionId 处理----公用
extern void sV2gIso1MakeSessionId(u8 *pSessionId, u16 u16Len);
extern bool sV2gIso1FillSessionId(struct iso1EXIDocument *pDoc, const u8 *pId, i32 i32IdLen);
extern void sV2gIso1CompSessionId(i32 i32DevIndex, struct iso1EXIDocument *pDoc, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode);

//V2G ISO15118 FAILED_SequenceError 错误检查
extern bool sV2gIso1MsgSequenceErrorCheck(i32 i32DevIndex, eV2gIso1MsgId_t eMsgId, iso1responseCodeType *pCode);

//V2G ISO15118 cp状态错误 检查
extern bool sV2gIso1CpStateErrorCheck(i32 i32DevIndex, eChgCpState_t eCpState);




//for SessionSetup
extern void sV2gIso1CompSessionIdForSessionSetup(i32 i32DevIndex, u8 *pSessionIdEvse, u8 *pSessionIdEv, i32 i32IdLen);

//for ServiceDetail
extern bool sV2gIso1ServiceIdCheck(i32 i32DevIndex, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode, u16 u16ServiceId);

//for PaymentServiceSelection
extern bool sV2gIso1ProcSelectedPaymentOption(i32 i32DevIndex, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode, iso1paymentOptionType eType);
extern bool sV2gIso1ProcSelectedServiceList(i32 i32DevIndex, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode, struct iso1SelectedServiceListType *pList);

//for Authorization
extern bool sV2gIso1ProcGenChallenge(i32 i32DevIndex, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode, i32 i32Len, const u8 *pGenChallenge);

//for ChargeParameterDiscovery
extern bool sV2gIso1ProcPublicDataChargeParameterDiscovery(stV2gIso1Cache_t *pCache, struct iso1ChargeParameterDiscoveryReqType *pReq);
extern bool sV2gIso1ProcRequestedEnergyTransferMode(i32 i32DevIndex, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode, iso1EnergyTransferModeType eType);
extern bool sV2gIso1ProcDcEvChargeParameter(i32 i32DevIndex, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode, struct iso1DC_EVChargeParameterType *pDcEvParam);
extern bool sV2gIso1ProcAcEvChargeParameter(i32 i32DevIndex, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode, struct iso1AC_EVChargeParameterType *pDcEvParam);


//for PowerDelivery
extern bool sV2gIso1ProcPublicDataPowerDelivery(stV2gIso1Cache_t *pCache, struct iso1PowerDeliveryReqType *pReq);
extern bool sV2gIso1SAScheduleTupleIDCheck(i32 i32DevIndex, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode, i16 SAScheduleTupleID);
extern bool sV2gIso1ChargingProfileCheck(i32 i32DevIndex, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode, struct iso1ChargingProfileType *pChargingProfile);

//for CurrentDemand
extern bool sV2gIso1ProcPublicDataCurrentDemand(stV2gIso1Cache_t *pCache, struct iso1CurrentDemandReqType *pReq);

//for MeterReceipt
bool sV2gIso1ContractIdCheck(i32 i32DevIndex, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode, struct iso1MeteringReceiptReqType *pReq);
bool sV2gIso1MeterInfoCheck(i32 i32DevIndex, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode, struct iso1MeterInfoType *pMeterInfo);





















#endif

