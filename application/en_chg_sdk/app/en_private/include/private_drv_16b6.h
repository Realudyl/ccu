/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_16b6.h
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2024-08-09
* notice                                :     本文件负责实现内网协议中 15/b5 报文
****************************************************************************************************/
#ifndef _private_drv_16b6_H_
#define _private_drv_16b6_H_
#include "en_common.h"
#include "private_drv_data_def.h"































#if (cSdkPrivDevType == cSdkPrivDevTypeM)

extern bool sPrivDrvPktSendB6(ePrivDrvCmdRst_t eRst);
extern bool sPrivDrvPktRecv16(const u8 *pBuf, i32 i32Len);

#elif (cSdkPrivDevType == cSdkPrivDevTypeS)

extern bool sPrivDrvPktSend16(u8 u8GunId, stPrivDrvCmd16_t *pCoolingData);
extern bool sPrivDrvPktRecvB6(const u8 *pBuf, i32 i32Len);

#endif





















#endif

















