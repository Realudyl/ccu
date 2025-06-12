/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   mod_drv_en_api.h
* Description                           :   EN+模块电源CAN通讯驱动实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-08-17
* notice                                :   
****************************************************************************************************/
#ifndef _mod_drv_en_api_h_
#define _mod_drv_en_api_h_
#include "mod_drv_en.h"



































//对外的api接口
extern bool  sModDrvEnGetOnlineAddrList(u8 u8MaxNum, i8 *pList);

extern bool  sModDrvEnOpen(u8 u8Addr, bool bGroupFlag);
extern bool  sModDrvEnClose(u8 u8Addr, bool bGroupFlag);
extern bool  sModDrvEnSetOutput(u8 u8Addr, bool bGroupFlag, f32 f32Udc, f32 f32IdcLimit);
extern bool  sModDrvEnSetGroupNum(u8 u8Addr, u32 u32GroupNum);

extern bool  sModDrvEnGetCurve(f32 *pUdcMax, f32 *pUdcMin, f32 *pIdcMax, f32 *pPdc);
extern bool  sModDrvEnGetOutput(u8 u8Addr, f32 *pUdc, f32 *pIdc);
extern bool  sModDrvEnGetFanSpeed(u8 u8Addr, u8 *pSpeed);
extern bool  sModDrvEnGetErrCode(u8 u8Addr, u8 u8MaxLen, u8 *pCode);
extern bool  sModDrvEnGetTenv(u8 u8Addr, f32 *pTenv);
extern bool  sModDrvEnGetTempDetail(u8 u8Addr, u8 u8MaxLen, f32 *pTemp);
extern bool  sModDrvEnGetUacInput(u8 u8Addr, f32 *pUab, f32 *pUbc, f32 *pUca);
extern bool  sModDrvEnGetWorkStatus(u8 u8Addr, u8 *pStatus);
extern bool  sModDrvEnGetRunTime(u8 u8Addr, u32 *pHtotal, u32 *pHthistime);
extern bool  sModDrvEnGetIacInput(u8 u8Addr, f32 *pIa, f32 *pIb, f32 *pIc);
extern bool  sModDrvEnGetPacFac(u8 u8Addr, f32 *pPac, f32 *pFac);
extern bool  sModDrvEnGetEnergy(u8 u8Addr, u32 *pEtotal, u32 *pEthistime);
extern bool  sModDrvEnGetMaterialNum(u8 u8Addr, u32 *pNum);
extern bool  sModDrvEnGetMatchCode(u8 u8Addr, u16 *pCode);
extern bool  sModDrvEnGetModelType(u8 u8Addr, u32 *pType);
extern bool  sModDrvEnGetFeatureWord(u8 u8Addr, u32 *pWord);
extern bool  sModDrvEnGetSerialNum(u8 u8Addr, u8 u8MaxLen, char *pSerialNum);
extern bool  sModDrvEnGetModelName(u8 u8Addr, u8 u8MaxLen, char *pModelName);
extern bool  sModDrvEnGetManufacturerId(u8 u8Addr, u8 u8MaxLen, char *pId);
extern bool  sModDrvEnGetSwVer(u8 u8Addr, u8 u8MaxLen, char *pSwVer);




#endif














