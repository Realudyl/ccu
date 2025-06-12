/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_04a4.h
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-10-22
* notice                                :     本文件负责实现内网协议中 04/A4 报文
****************************************************************************************************/
#ifndef _private_drv_04a4_H_
#define _private_drv_04a4_H_
#include "en_common.h"
#include "private_drv_data_def.h"










//04报文 发送周期
#ifndef cSdkPrivDrvCycle04
#define cPrivDrvCycle04                 (120)
#else
#define cPrivDrvCycle04                 (cSdkPrivDrvCycle04)
#endif








#if (cSdkPrivDevType == cSdkPrivDevTypeM)

extern bool sPrivDrvPktSendA4(void);
extern bool sPrivDrvPktRecv04(const u8 *pBuf, i32 i32Len);

#elif (cSdkPrivDevType == cSdkPrivDevTypeS)

extern bool sPrivDrvPktSend04(void);
extern bool sPrivDrvPktRecvA4(const u8 *pBuf, i32 i32Len);

#endif





















#endif

















