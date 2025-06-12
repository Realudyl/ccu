/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_0dad.h
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-11-06
* notice                                :     本文件负责实现内网协议中 0D/AD 报文
****************************************************************************************************/
#ifndef _private_drv_0dad_H_
#define _private_drv_0dad_H_
#include "en_common.h"
#include "private_drv_data_def.h"



















#if (cSdkPrivDevType == cSdkPrivDevTypeM)

extern bool sPrivDrvPktSendAD(void);
extern bool sPrivDrvPktRecv0D(const u8 *pBuf, i32 i32Len);

#elif (cSdkPrivDevType == cSdkPrivDevTypeS)

extern bool sPrivDrvPktSend0D(void);
extern bool sPrivDrvPktRecvAD(const u8 *pBuf, i32 i32Len);

#endif





















#endif

















