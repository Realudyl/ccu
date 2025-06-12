/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   bms_gbt_15_api.h
* Description                           :   协议结构体变量的一些set和get方法
* Version                               :   
* Author                                :   haisheng.dang@en-plus.com.cn
* Creat Date                            :   2024-04-17
* notice                                :   
****************************************************************************************************/
#ifndef _bms_gbt_15_api_h_
#define _bms_gbt_15_api_h_

#include "en_common.h"
#include "bms_gbt_15_frame_def.h"
#include "bms_gbt_15.h"




//set
extern bool sBmsGbt15Set_ChgPointNumber(ePileGunIndex_t eGunIndex, u8 *pNum);
extern bool sBmsGbt15Set_ChgAreaNum(ePileGunIndex_t eGunIndex, u8 *pAreaNum);
extern bool sBmsGbt15Set_ChgMaxOutVol(ePileGunIndex_t eGunIndex, u16 u16MaxOutVol);
extern bool sBmsGbt15Set_ChgMinOutVol(ePileGunIndex_t eGunIndex, u16 u16MinOutVol);
extern bool sBmsGbt15Set_ChgMaxOutCur(ePileGunIndex_t eGunIndex, u16 u16MaxOutCur);
extern bool sBmsGbt15Set_ChgMinOutCur(ePileGunIndex_t eGunIndex, u16 u16MinOutCur);
extern bool sBmsGbt15Set_PreIsoCheckResult(ePileGunIndex_t eGunIndex, eBmsGbt15PreIsoResult_t eResult);
extern bool sBmsGbt15Set_IsoCheckResult(ePileGunIndex_t eGunIndex, eBmsGbt15IsoResult_t eResult);
extern bool sBmsGbt15Set_PreChgResult(ePileGunIndex_t eGunIndex, eBmsGbt15PreChgRst_t eResult);
extern bool sBmsGbt15Set_ReChgCondition(ePileGunIndex_t eGunIndex, eBmsGbt15ReChgCondition_t eCondition);
extern bool sBmsGbt15Set_OutputVoltage(ePileGunIndex_t eGunIndex, u16 u16OutputVol);
extern bool sBmsGbt15Set_OutputCurrent(ePileGunIndex_t eGunIndex, u16 u16OutputCur);
extern bool sBmsGbt15Set_AuthorizeState(ePileGunIndex_t eGunIndex, bool bState);
extern bool sBmsGbt15Set_ChargeTotalWh(ePileGunIndex_t eGunIndex, u16 u16TotalWh);
extern bool sBmsGbt15Set_ChargeTime(ePileGunIndex_t eGunIndex, u16 u16ChgTime);
extern bool sBmsGbt15Set_Return2Wait(ePileGunIndex_t eGunIndex);
extern bool sBmsGbt15Set_CstCode(ePileGunIndex_t eGunIndex, eBmsGbt15StopReason_t eReason, eBmsGbt15StopFault_t eFault, eBmsGbt15StopErr_t eErr);
extern bool sBmsGbt15Set_Cc1StateInChg(ePileGunIndex_t eGunIndex, eBmsGbt15ChgCc1State_t eResult);








//get
extern bool sBmsGbt15Get_EvPauseChargeState(ePileGunIndex_t eGunIndex);
extern bool sBmsGbt15Get_AllowStopAuxPower(ePileGunIndex_t eGunIndex);

extern bool sBmsGbt15Get_EvccMaxChgVol(ePileGunIndex_t eGunIndex, u16 *pMaxChgVol);
extern bool sBmsGbt15Get_BcpMaxChgVol(ePileGunIndex_t eGunIndex, u16 *pMaxChgVol);
extern bool sBmsGbt15Get_BcpMaxChgCur(ePileGunIndex_t eGunIndex, u16 *pMaxChgCur);
extern bool sBmsGbt15Get_BcpCurBatVol(ePileGunIndex_t eGunIndex, u16 *pMaxChgCur);
extern bool sBmsGbt15Get_BclReqChgVol(ePileGunIndex_t eGunIndex, u16 *pReqChgVol);
extern bool sBmsGbt15Get_BclReqChgCur(ePileGunIndex_t eGunIndex, u16 *pReqChgCur);
extern bool sBmsGbt15Get_ChgRemainTime(ePileGunIndex_t eGunIndex, u16 *pRemainTime);
extern u8   sBmsGbt15Get_EvccCurSoc(ePileGunIndex_t eGunIndex);
extern bool sBmsGbt15Get_VinCode(ePileGunIndex_t eGunIndex, u8 *pVin);
extern bool sBmsGbt15Get_IsoCheckResult(ePileGunIndex_t eGunIndex, eBmsGbt15IsoResult_t *eResult);
extern bool sBmsGbt15Get_PreChgResult(ePileGunIndex_t eGunIndex, eBmsGbt15PreChgRst_t *eResult);
extern bool sBmsGbt15Get_PreIsoCheckResult(ePileGunIndex_t eGunIndex, eBmsGbt15PreIsoResult_t *eResult);
extern bool sBmsGbt15Get_EvccCacheData(ePileGunIndex_t eGunIndex, stBmsGbt15Evcc_t *pCache);
extern bool sBmsGbt15Get_EvccFrameTimeout(ePileGunIndex_t eGunIndex, unBmsGbt15RecvTimeout *pTimeout);    
extern bool sBmsGbt15Get_BmsShakeHandTimes(ePileGunIndex_t eGunIndex, u16 *u16BmsShankhandTimes);
extern bool sBmsGbt15Get_Cc1StateInChg(ePileGunIndex_t eGunIndex, eBmsGbt15ChgCc1State_t * eResult);
#endif

