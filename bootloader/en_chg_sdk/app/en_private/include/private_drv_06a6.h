/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_06a6.h
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-10-30
* notice                                :     本文件负责实现内网协议中 06/A6 报文
****************************************************************************************************/
#ifndef _private_drv_06a6_H_
#define _private_drv_06a6_H_
#include "en_common.h"
#include "private_drv_data_def.h"































#if (cSdkPrivDevType == cSdkPrivDevTypeM)

extern bool sPrivDrvPktSendA6(u8 u8GunId, stPrivDrvCmdA6_t *pRmt);
extern bool sPrivDrvPktRecv06(const u8 *pBuf, i32 i32Len);

#elif (cSdkPrivDevType == cSdkPrivDevTypeS)

extern bool sPrivDrvPktSend06(u8 u8GunId, stPrivDrvCmd06_t *pRmtAck);
extern bool sPrivDrvPktRecvA6(const u8 *pBuf, i32 i32Len);

#endif





















#endif

















