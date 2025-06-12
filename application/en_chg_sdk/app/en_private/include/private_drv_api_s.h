/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_api_s.h
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-10-19
* notice                                :     完整的内网协议包含两个方向，网关和桩端，本驱动需要实现
*                                             完整的内网协议驱动，
*
*                                             定义网关为master，缩写m，桩端为slave，缩写s

*                                             本文件实现slave侧的抽象数据接口api
****************************************************************************************************/
#ifndef _private_drv_api_s_H_
#define _private_drv_api_s_H_
#include "en_common.h"
#include "private_drv_frame_def.h"





















#if (cSdkPrivDevType == cSdkPrivDevTypeS)


extern bool sPrivDrvVarInitS(void);




//--------------------------------------可读可写数据接口----------------------
//head部分
extern bool sPrivDrvSetChargerAddr(u16 u16Addr);

//02帧部分
extern bool sPrivDrvSetChargerType(ePrivDrvChargerType_t eType);
extern bool sPrivDrvSetPayAt(ePrivDrvPayAt_t ePayAt);
extern bool sPrivDrvSetMoneyCalcBy(ePrivDrvMoneyCalcBy_t eMoneyCalcBy);
extern bool sPrivDrvSetDisableFlag(ePrivDrvChargerDisableFlag_t eFlag);
extern bool sPrivDrvSetStartPowerMode(ePrivDrvStartPowerMode_t eMode);
extern bool sPrivDrvSetWorkMode(ePrivDrvChargerWorkMode_t eMode);
extern bool sPrivDrvSetBalancePhase(ePrivDrvBalancePhase_t eBalancePhase);
extern bool sPrivDrvSetPowerMax(i32 i32Power);
extern bool sPrivDrvSetCurrMax(i32 i32Curr);
extern bool sPrivDrvSetPowerRated(i32 i32Power);
extern bool sPrivDrvSetCurrRated(i32 i32Curr);
extern bool sPrivDrvSetBookingViaBle(ePrivDrvBookingViaBle_t eBookingViaBle);
extern bool sPrivDrvSetLoginedFlag(ePrivDrvLoginedFlag_t eFlag);
extern bool sPrivDrvSetRebootReason(ePrivDrvRebootReason_t eReason);
extern bool sPrivDrvSetSn(i32 i32Len, u8 *pBuf);
extern bool sPrivDrvSetDevCode(i32 i32Len, u8 *pBuf);
extern bool sPrivDrvSetDevName(i32 i32Len, u8 *pBuf);
extern bool sPrivDrvSetHwVer(i32 i32Len, u8 *pBuf);
extern bool sPrivDrvSetSwVer(i32 i32Len, u8 *pBuf);
extern bool sPrivDrvSetGunNum(u8 u8GunNum);
extern bool sPrivDrvSetGunWorkStatus(u8 u8GunId, ePrivDrvGunWorkStatus_t eStatus);
extern bool sPrivDrvSetGunConnStatus(u8 u8GunId, ePrivDrvGunConnStatus_t eStatus);
extern bool sPrivDrvSetGunLockStatus(u8 u8GunId, ePrivDrvGunLockStatus_t eStatus);
extern bool sPrivDrvSetRsvdDataLogin(i32 i32Len, u8 *pBuf);

//03帧部分
extern bool sPrivDrvSetGunEvent(u8 u8GunId, ePrivDrvGunEvent_t eEvent);
extern bool sPrivDrvSetGunStsReason(u8 u8GunId, ePrivDrvGunWorkStsRsn_t eReason);
extern bool sPrivDrvSetFaultByte(unPrivDrvFaultByte_t *pFaultByte);
extern bool sPrivDrvSetWarningByte(unPrivDrvWarningByte_t *pWarningByte);
extern bool sPrivDrvSetFaultCode(u8 u8GunId, stPrivDrvFaultCode_t *pFaultCode);

//04帧部分--无

//05帧部分
extern bool sPrivDrvSetCardAuth(u8 u8GunId, stPrivDrvCmd05_t *pCardAuth);

//06帧部分
extern bool sPrivDrvSetRmtAck(u8 u8GunId, stPrivDrvCmd06_t *pRmtAck);

//07帧部分
extern bool sPrivDrvSetTrans(u8 u8GunId, stPrivDrvCmd07_t *pTrans);

//08、09帧部分
extern bool sPrivDrvSetRtRptType(u8 u8GunId, ePrivDrvRtDataRptType_t eRptType);
extern bool sPrivDrvSetChargingPower(u8 u8GunId, u32 u32Power);
extern bool sPrivDrvSetRtRpt (u8 u8GunId, stPrivDrvCmd09_t *pRcdData);
extern bool sPrivDrvSetRcdRpt(stPrivDrvCmd09_t *pRcdData);

//0A帧部分
extern bool sPrivDrvSetCmdParam(bool bRptFlag, u8 u8GunId, ePrivDrvCfgCmdType_t eType, ePrivDrvCmdRst_t eRst, ePrivDrvParamAddr_t eAddr, u16 u16ParamLen, const u8 *pParam, u8 u8RepeatCnt);

//0B、0C、0D帧部分---无
//0E帧部分
extern bool sPrivDrvSetUacIn(u8 u8GunId, stPrivDrvPhaseData_t *pUacIn);
extern bool sPrivDrvSetIacIn(u8 u8GunId, stPrivDrvPhaseDataI_t *pIacIn);
extern bool sPrivDrvSetFac(u8 u8GunId, i16 i16Fac);
extern bool sPrivDrvSetUacOut(u8 u8GunId, stPrivDrvPhaseData_t *pUacOut);
extern bool sPrivDrvSetIacOut(u8 u8GunId, stPrivDrvPhaseDataI_t *pIacOut);
extern bool sPrivDrvSetPfTotal(u8 u8GunId, i16 i16PfTotal);
extern bool sPrivDrvSetPf(u8 u8GunId, stPrivDrvPhaseData_t *pPf);
extern bool sPrivDrvSetDuty(u8 u8GunId, i16 i16Duty);
extern bool sPrivDrvSetUcp(u8 u8GunId, i16 i16Ucp);
extern bool sPrivDrvSetUcc(u8 u8GunId, i16 i16Ucc);
extern bool sPrivDrvSetOffset(u8 u8GunId, i16 i16Offset0, i16 i16Offset1);
extern bool sPrivDrvSetIct(u8 u8GunId, i32 i32Ict);
extern bool sPrivDrvSetUpeIn(u8 u8GunId, stPrivDrvPhaseDataN_t *pUpeIn);
extern bool sPrivDrvSetUpeOut(u8 u8GunId, stPrivDrvPhaseDataN_t *pUpeOut);
extern bool sPrivDrvSetIleak(u8 u8GunId, i16 i16IleakAc, i16 i16IleakDc);
extern bool sPrivDrvSetChgTemp(stPrivDrvChgTempData_t *pTemp);
extern bool sPrivDrvSetGunTemp(u8 u8GunId, stPrivDrvGunTempData_t *pTemp);
extern bool sPrivDrvSetRsvdDataYc(u8 u8GunId, i32 i32Len, u8 *pBuf);
extern bool sPrivDrvSetYcRpt(u8 u8GunId);

//11帧部分
extern bool sPrivDrvSetEvtNotice(stPrivDrvCmd11_t *pEvtNotice);

//13帧部分
extern bool sPrivDrvSetBmsRpt(u8 u8GunId, stPrivDrvCmd13_t *pBmsData);

//14帧部分
extern bool sPrivDrvSetBookingDataAck(u8 u8GunId, stPrivDrvCmd14_t *pBookingDataAck);

//15帧部分
extern bool sPrivDrvSetMpcData(stPrivDrvCmd15_t *pMpcData, u16 u16ActualDataLen);

//16帧部分
extern bool sPrivDrvSetCoolingData(u8 u8GunId, stPrivDrvCmd16_t *pCoolingData);













//A2帧部分
extern bool sPrivDrvGetLoginAck(ePrivDrvCmdRst_t *pRst);
extern bool sPrivDrvGetNetType(ePrivDrvNetHwType_t *pType);
extern bool sPrivDrvGetGwVersion(i32 i32MaxLen, u8 *pBuf);

//A3帧部分
extern bool sPrivDrvGetNetStatus(ePrivDrvNetStatus_t *pStatus);
extern bool sPrivDrvGetSignalStr(i8 *pRssi);
extern bool sPrivDrvGetMatserTime(stTime_t *pTime);
extern bool sPrivDrvGetRsvdDataStsAck(u8 u8GunId, i32 i32MaxLen, u8 *pBuf);

//A4帧部分--无

//A5帧部分
extern bool sPrivDrvGetCardAuthAck(u8 u8GunId, stPrivDrvCmdA5_t **pCardAuth);

//A6帧部分
extern bool sPrivDrvGetRmt(u8 u8GunId, stPrivDrvCmdA6_t **pRmt);

//A7帧部分
extern bool sPrivDrvGetTransAck(u8 u8GunId, stPrivDrvCmdA7_t **pTransAck);

//A8、A9帧部分
extern bool sPrivDrvGetRtRptAck (u8 u8GunId, stPrivDrvCmdA8_t **pRtDataAck );
extern bool sPrivDrvGetRcdRptAck(u8 u8GunId, stPrivDrvCmdA9_t **pRcdDataAck);


//AA帧部分
extern bool sPrivDrvGetCmdParam(bool bRptFlag, stPrivDrvCmdAA_t **pCmdAA);

//AB帧部分
extern bool sPrivDrvGetChgOta(stPrivDrvCmdAB_t **pOta);

//AC帧部分
extern bool sPrivDrvGetModOta(stPrivDrvCmdAC_t **pOta);

//AD帧部分---无
//AE帧部分
extern bool sPrivDrvGetRsvdDataYcAck(u8 u8GunId, i32 i32MaxLen, u8 *pBuf);

//B1帧部分
extern bool sPrivDrvGetEvtNoticeAck(stPrivDrvCmdB1_t **pEvtNoticeAck);

//B3帧部分---无

//B4帧部分
extern bool sPrivDrvGetBookingData(u8 u8GunId, stPrivDrvCmdB4_t **pBookingData);

//B5帧部分---无
//B6帧部分---无




















#endif














































#endif














