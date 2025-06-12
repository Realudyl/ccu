/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_14b4.h
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     XRG
* Creat Date                            :     2024-01-18
* notice                                :     本文件负责实现内网协议中 14/B4 报文 
****************************************************************************************************/
#ifndef _private_drv_14b4_H_
#define _private_drv_14b4_H_
#include "en_common.h"
#include "private_drv_data_def.h"































#if (cSdkPrivDevType == cSdkPrivDevTypeM)

extern bool sPrivDrvPktSendB4(u8 u8GunId, stPrivDrvCmdB4_t *pBookingData);
extern bool sPrivDrvPktRecv14(const u8 *pBuf, i32 i32Len);

#elif (cSdkPrivDevType == cSdkPrivDevTypeS)

extern bool sPrivDrvPktSend14(u8 u8GunId, stPrivDrvCmd14_t *pCmd14Data);
extern bool sPrivDrvPktRecvB4(const u8 *pBuf, i32 i32Len);

#endif





















#endif

















