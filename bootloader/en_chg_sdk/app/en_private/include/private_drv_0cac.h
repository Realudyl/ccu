/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_0cac.h
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-11-03
* notice                                :     本文件负责实现内网协议中 0C/AC 报文
****************************************************************************************************/
#ifndef _private_drv_0cac_H_
#define _private_drv_0cac_H_
#include "en_common.h"
#include "private_drv_data_def.h"




























#if (cSdkPrivDevType == cSdkPrivDevTypeM)

extern bool sPrivDrvPktSendAC(u16 u16AddrCpu, u16 u16AddrBase, u16 u16Offset, ePrivDrvUpdateCmd_t eCmd, u16 u16SegCrc, const u8 *pBuf, u8 *pDataLen);
extern bool sPrivDrvPktRecv0C(const u8 *pBuf, i32 i32Len);

#elif (cSdkPrivDevType == cSdkPrivDevTypeS)

extern bool sPrivDrvPktSend0C(u16 u16AddrCpu, ePrivDrvUpdateCmd_t eCmd, u16 u16Rst);
extern bool sPrivDrvPktRecvAC(const u8 *pBuf, i32 i32Len);

#endif





















#endif

















