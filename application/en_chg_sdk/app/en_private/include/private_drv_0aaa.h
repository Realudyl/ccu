/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_0aaa.h
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-11-01
* notice                                :     本文件负责实现内网协议中 0A/AA 报文
****************************************************************************************************/
#ifndef _private_drv_0aaa_H_
#define _private_drv_0aaa_H_
#include "en_common.h"
#include "private_drv_data_def.h"




























#if (cSdkPrivDevType == cSdkPrivDevTypeM)

extern bool sPrivDrvPktSendAA(u8 u8GunId, bool bRptFlag, ePrivDrvCfgCmdType_t eType, ePrivDrvCmdRst_t eRst, ePrivDrvParamAddr_t eAddr, u16 u16ParamLen, const u8 *pParam);
extern bool sPrivDrvPktRecv0A(const u8 *pBuf, i32 i32Len);

#elif (cSdkPrivDevType == cSdkPrivDevTypeS)

extern bool sPrivDrvPktSend0A(bool bRptFlag, u8 u8GunId, ePrivDrvCfgCmdType_t eType, ePrivDrvCmdRst_t eRst, ePrivDrvParamAddr_t eAddr, u16 u16ParamLen, const u8 *pParam);
extern bool sPrivDrvPktRecvAA(const u8 *pBuf, i32 i32Len);

#endif





















#endif

















