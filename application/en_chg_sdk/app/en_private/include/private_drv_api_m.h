/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_api_m.h
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-10-19
* notice                                :     完整的内网协议包含两个方向，网关和桩端，本驱动需要实现
*                                             完整的内网协议驱动，
*
*                                             定义网关为master，缩写m，桩端为slave，缩写s
*
*                                             本文件实现master侧的抽象数据接口api
****************************************************************************************************/
#ifndef _private_drv_api_m_H_
#define _private_drv_api_m_H_
#include "en_common.h"
#include "private_drv_frame_def.h"





















#if (cSdkPrivDevType == cSdkPrivDevTypeM)


extern bool sPrivDrvVarInitM(void);




//--------------------------------------可读可写数据接口----------------------
//head部分
extern bool sPrivDrvGetProtocolVersion(ePrivDrvVersion_t *pVer);
extern bool sPrivDrvGetChargerAddr(u16 *pAddr);
extern bool sPrivDrvGetAckSeqno(u8 *pSeqno);
extern bool sPrivDrvGetRptSeqno(u8 *pSeqno);

//公共部分
extern bool sPrivDrvGetStatus(ePrivDrvStatus_t *pSts);
extern bool sPrivDrvGetPrintfType(void);


//02帧部分
extern bool sPrivDrvGetChargerType(ePrivDrvChargerType_t *pType);
extern bool sPrivDrvGetPayAt(ePrivDrvPayAt_t *pPayAt);
extern bool sPrivDrvGetMoneyCalcBy(ePrivDrvMoneyCalcBy_t *pMoneyCalcBy);
extern bool sPrivDrvGetDisableFlag(ePrivDrvChargerDisableFlag_t *pFlag);
extern bool sPrivDrvGetStartPowerMode(ePrivDrvStartPowerMode_t *pMode);
extern bool sPrivDrvGetWorkMode(ePrivDrvChargerWorkMode_t *pMode);
extern bool sPrivDrvGetBalancePhase(ePrivDrvBalancePhase_t *pBalancePhase);
extern bool sPrivDrvGetPowerMax(i32 *pPower);
extern bool sPrivDrvGetCurrMax(i32 *pCurr);
extern bool sPrivDrvGetPowerRated(i32 *pPower);
extern bool sPrivDrvGetCurrRated(i32 *pCurr);
extern bool sPrivDrvGetBookingViaBle(ePrivDrvBookingViaBle_t *pBookingViaBle);
extern bool sPrivDrvGetLoginedFlag(ePrivDrvLoginedFlag_t *pFlag);
extern bool sPrivDrvGetRebootReason(ePrivDrvRebootReason_t *pReason);
extern bool sPrivDrvGetSn(i32 i32MaxLen, u8 *pBuf);
extern bool sPrivDrvGetDevCode(i32 i32MaxLen, u8 *pBuf);
extern bool sPrivDrvGetDevName(i32 i32MaxLen, u8 *pBuf);
extern bool sPrivDrvGetHwVer(i32 i32MaxLen, u8 *pBuf);
extern bool sPrivDrvGetSwVer(i32 i32MaxLen, u8 *pBuf);
extern bool sPrivDrvGetGunNum(u8 *pGunNum);
extern bool sPrivDrvGetGunWorkStatus(u8 u8GunId, ePrivDrvGunWorkStatus_t *pStatus);
extern bool sPrivDrvGetGunConnStatus(u8 u8GunId, ePrivDrvGunConnStatus_t *pStatus);
extern bool sPrivDrvGetGunLockStatus(u8 u8GunId, ePrivDrvGunLockStatus_t *pStatus);
extern bool sPrivDrvGetRsvdDataLogin(i32 i32MaxLen, u8 *pBuf);

//03帧部分
extern bool sPrivDrvGetGunEvent(u8 u8GunId, ePrivDrvGunEvent_t *pEvent);
extern bool sPrivDrvGetGunWorkStatusOld(u8 u8GunId, ePrivDrvGunWorkStatus_t *pStatus);
extern bool sPrivDrvGetGunStsReason(u8 u8GunId, ePrivDrvGunWorkStsRsn_t *pReason);
extern bool sPrivDrvGetFaultByte(unPrivDrvFaultByte_t *pFaultByte);
extern bool sPrivDrvGetWarningByte(unPrivDrvWarningByte_t *pWarningByte);
extern bool sPrivDrvGetNewTime(stTime_t *pTime);
extern bool sPrivDrvGetFaultCode(u8 u8GunId, stPrivDrvFaultCode_t *pFaultCode);
extern bool sPrivDrvGetStatusRep(u8 u8GunId, u32 u32WaitTime);

//04帧部分
extern bool sPrivDrvGetHeartbeat(void);

//05帧部分
extern bool sPrivDrvGetCardAuth(u8 u8GunId, stPrivDrvCmd05_t **pCardAuth);

//06帧部分
extern bool sPrivDrvGetRmtAck(u8 u8GunId, stPrivDrvCmd06_t **pRmtAck);

//07帧部分
extern bool sPrivDrvGetTrans(u8 u8GunId, stPrivDrvCmd07_t **pTrans);

//08、09帧部分
extern bool sPrivDrvGetRtRpt (u8 u8GunId, stPrivDrvCmd08_t **pRtData );
extern bool sPrivDrvGetRcdRpt(u8 u8GunId, stPrivDrvCmd09_t **pRcdData);

//0A帧部分
extern bool sPrivDrvGetCmdParam(bool bRptFlag, stPrivDrvCmd0A_t **pCmd0A);

//0B、0C、0D帧部分---无
//0E帧部分
extern bool sPrivDrvGetUacIn(u8 u8GunId, stPrivDrvPhaseData_t *pUacIn);
extern bool sPrivDrvGetIacIn(u8 u8GunId, stPrivDrvPhaseDataI_t *pIacIn);
extern bool sPrivDrvGetFac(u8 u8GunId, i16 *pFac);
extern bool sPrivDrvGetUacOut(u8 u8GunId, stPrivDrvPhaseData_t *pUacOut);
extern bool sPrivDrvGetIacOut(u8 u8GunId, stPrivDrvPhaseDataI_t *pIacOut);
extern bool sPrivDrvGetPfTotal(u8 u8GunId, i16 *pPfTotal);
extern bool sPrivDrvGetPf(u8 u8GunId, stPrivDrvPhaseData_t *pPf);
extern bool sPrivDrvGetDuty(u8 u8GunId, i16 *pDuty);
extern bool sPrivDrvGetUcp(u8 u8GunId, i16 *pUcp);
extern bool sPrivDrvGetUcc(u8 u8GunId, i16 *pUcc);
extern bool sPrivDrvGetOffset(u8 u8GunId, i16 *pOffset0, i16 *pOffset1);
extern bool sPrivDrvGetIct(u8 u8GunId, i32 *pIct);
extern bool sPrivDrvGetUpeIn(u8 u8GunId, stPrivDrvPhaseDataN_t *pUpeIn);
extern bool sPrivDrvGetUpeOut(u8 u8GunId, stPrivDrvPhaseDataN_t *pUpeOut);
extern bool sPrivDrvGetIleak(u8 u8GunId, i16 *pIleakAc, i16 *pIleakDc);
extern bool sPrivDrvGetChgTemp(stPrivDrvChgTempData_t *pTemp);
extern bool sPrivDrvGetGunTemp(u8 u8GunId, stPrivDrvGunTempData_t *pTemp);
extern bool sPrivDrvGetChargPower(u8 u8GunId, u32 *pPower);
extern bool sPrivDrvGetMeterNow(u8 u8GunId, u32 *pMeter);
extern bool sPrivDrvGetRsvdDataYc(u8 u8GunId, i32 i32MaxLen, u8 *pBuf);
extern bool sPrivDrvGetYc(u8 u8GunId);

//11帧部分
extern bool sPrivDrvGetEvtNotice(stPrivDrvCmd11_t **pEvtNotice);

//13帧部分
extern bool sPrivDrvGetBmsRpt(u8 u8GunId, bool bMutex, stPrivDrvCmd13_t **pBmsData);

//14帧部分
extern bool sPrivDrvGetBookingDataAck(u8 u8GunId, stPrivDrvCmd14_t **pBookingDataAck);

//15帧部分
extern bool sPrivDrvGetMpcData(stPrivDrvCmd15_t **pMpcData);

//16帧部分
extern bool sPrivDrvGetCoolingData(u8 u8GunId, stPrivDrvCmd16_t **pCoolingData);











//公共帧部分
extern bool sPrivDrvSetPrintfType(bool bTpye);

//A2帧部分
extern bool sPrivDrvSetLoginAck(ePrivDrvCmdRst_t eLogin);
extern bool sPrivDrvSetNetType(ePrivDrvNetHwType_t eHwType);
extern bool sPrivDrvSetGwVersion(i32 i32DatLen, u8 *pBuf);

//A3帧部分
extern bool sPrivDrvSetNetStatus(ePrivDrvNetStatus_t eStatus);
extern bool sPrivDrvSetRssi(i8 i8Rssi);
extern bool sPrivDrvSetRsvdDataStsAck(u8 u8GunId, i32 i32DatLen, u8 *pBuf);

//A4帧部分
extern bool sPrivDrvSetHbAck(void);

//A5帧部分
extern bool sPrivDrvSetCardAuthAck(u8 u8GunId, stPrivDrvCmdA5_t *pCardAuthAckData);

//A6帧部分
extern bool sPrivDrvSetRmt(u8 u8GunId, stPrivDrvCmdA6_t *pRmt);

//A7帧部分
extern bool sPrivDrvSetTransAck(u8 u8GunId, stPrivDrvCmdA7_t *pTransAck);

//A8、A9帧部分
extern bool sPrivDrvSetRtRptAck (u8 u8GunId, ePrivDrvCmdRst_t eRst, u32 u32AccountBalance, i32 i32RsvdLen, const u8 *pRsvd);
extern bool sPrivDrvSetRcdRptAck(u8 u8GunId, ePrivDrvCmdRst_t eRst, i32 i32OrderIdLen, const u8 *pOrderId, i32 i32SrvTradeIdLen, const u8 *pSrvTradeId);
extern bool sPrivDrvSetResetChargingData(u8 u8GunId);

//AA帧部分
extern bool sPrivDrvSetCmdParam(bool bRptFlag, u8 u8GunId, ePrivDrvCfgCmdType_t eType, ePrivDrvCmdRst_t eRst, ePrivDrvParamAddr_t eAddr, u16 u16ParamLen, const u8 *pParam, u8 u8RepeatCnt);



//AB、AC帧部分
extern bool sPrivDrvSetChgUpdateFileInstall(const char *pPath);
extern bool sPrivDrvSetModUpdateFileInstall(const char *pPath);

//AD帧部分---无
//AE帧部分
extern bool sPrivDrvSetRsvdDataYcAck(u8 u8GunId, i32 i32DatLen, u8 *pBuf);

//B1帧部分
extern bool sPrivDrvSetEvtNoticeAck(stPrivDrvCmdB1_t *pEvtNoticeAck);

//B3帧部分---无

//B4帧部分
extern bool sPrivDrvSetBookingData(u8 u8GunId, stPrivDrvCmdB4_t *pBookingData);

//B5帧部分---无
//B6帧部分---无




















#endif














































#endif














