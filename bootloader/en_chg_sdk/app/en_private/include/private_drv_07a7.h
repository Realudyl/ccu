/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_07a7.h
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-10-22
* notice                                :     本文件负责实现内网协议中 07/A7 报文
****************************************************************************************************/
#ifndef _private_drv_07a7_H_
#define _private_drv_07a7_H_
#include "en_common.h"
#include "private_drv_data_def.h"






























#if (cSdkPrivDevType == cSdkPrivDevTypeM)

extern bool sPrivDrvPktSendA7(u8 u8GunId, stPrivDrvCmdA7_t *pTransAck);
extern bool sPrivDrvPktRecv07(const u8 *pBuf, i32 i32Len);

#elif (cSdkPrivDevType == cSdkPrivDevTypeS)

extern bool sPrivDrvPktSend07(u8 u8GunId, stPrivDrvCmd07_t *pTrans);
extern bool sPrivDrvPktRecvA7(const u8 *pBuf, i32 i32Len);

#endif





















#endif

















