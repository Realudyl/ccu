/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_03a3.h
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-10-22
* notice                                :     本文件负责实现内网协议中 03/A3 报文
****************************************************************************************************/
#ifndef _private_drv_03a3_H_
#define _private_drv_03a3_H_
#include "en_common.h"
#include "private_drv_data_def.h"










//03报文 发送周期
#ifndef cSdkPrivDrvCycle03
#define cPrivDrvCycle03                 (3600)
#else
#define cPrivDrvCycle03                 (cSdkPrivDrvCycle03)
#endif








#if (cSdkPrivDevType == cSdkPrivDevTypeM)

extern bool sPrivDrvPktSendA3(u8 u8GunId);
extern bool sPrivDrvPktRecv03(const u8 *pBuf, i32 i32Len);

#elif (cSdkPrivDevType == cSdkPrivDevTypeS)

extern bool sPrivDrvPktSend03(u8 u8GunId);
extern bool sPrivDrvPktRecvA3(const u8 *pBuf, i32 i32Len);

#endif





















#endif

















