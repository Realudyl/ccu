/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   mod_drv_winline_api.h
* Description                           :   永联科技 NXR 系列充电模块CAN通讯驱动实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-04-02
* notice                                :   基于  <NXR 系列充电模块通讯协议 V1.48>  设计
****************************************************************************************************/
#ifndef _mod_drv_winline_api_h_
#define _mod_drv_winline_api_h_
#include "mod_drv_winline.h"























//对外的api接口
extern bool  sModDrvWinlineGetOnlineAddrList(u8 u8MaxNum, i8 *pList);

extern bool  sModDrvWinlineOpen(u8 u8Addr, bool bGroupFlag);
extern bool  sModDrvWinlineClose(u8 u8Addr, bool bGroupFlag);
extern bool  sModDrvWinlineSetOutput(u8 u8Addr, bool bGroupFlag, f32 f32Udc, f32 f32IdcLimit);
extern bool  sModDrvWinlineSetGroup(u8 u8Addr, u32 u32Group);
extern bool  sModDrvWinlineSetAddrMode(u8 u8Addr, eModDrvWinlineAddrMode_t eMode);
extern bool  sModDrvWinlineSetAltitude(u8 u8Addr, u32 u32Altitude);

extern bool  sModDrvWinlineGetCurve(f32 *pUdcMax, f32 *pUdcMin, f32 *pIdcMax, f32 *pPdc);
extern bool  sModDrvWinlineGetOutput(u8 u8Addr, f32 *pUdc, f32 *pIdc);
extern bool  sModDrvWinlineGetGroup(u8 u8Addr, u32 *pGroup);
extern bool  sModDrvWinlineGetAddrBySwitch(u8 u8Addr, u32 *pAddrBySwitch);
extern bool  sModDrvWinlineGetAltitude(u8 u8Addr, u32 *pAltitude);
extern bool  sModDrvWinlineGetFanSpeed(u8 u8Addr, u8 *pFanS);
extern bool  sModDrvWinlineGetErrCode(u8 u8Addr, u8 u8MaxLen, u8 *pCode);
extern bool  sModDrvWinlineGetTenv(u8 u8Addr, f32 *pTenv);
extern bool  sModDrvWinlineGetTempDetail(u8 u8Addr, u8 u8MaxLen, f32 *pTemp);
extern bool  sModDrvWinlineGetUacInput(u8 u8Addr, f32 *pUab, f32 *pUbc, f32 *pUca);
extern bool  sModDrvWinlineGetPacFac(u8 u8Addr, f32 *pPac, f32 *pFac);
extern bool  sModDrvWinlineGetSwVer(u8 u8Addr, u8 u8MaxLen, char *pSwVer);



#endif













