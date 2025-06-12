/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_05a5.h
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-10-22
* notice                                :     本文件负责实现内网协议中 05/A5 报文
****************************************************************************************************/
#ifndef _private_drv_05a5_H_
#define _private_drv_05a5_H_
#include "en_common.h"
#include "private_drv_data_def.h"











//桩端上报卡认证请求之后 等待响应信息的超时时间
#ifndef cSdkPrivDrvCardAuthAckTimeout
#define cPrivDrvCardAuthAckTimeout      (15)
#else
#define cPrivDrvCardAuthAckTimeout      (cSdkPrivDrvCardAuthAckTimeout)
#endif






















#if (cSdkPrivDevType == cSdkPrivDevTypeM)

extern bool sPrivDrvPktSendA5(u8 u8GunId, stPrivDrvCmdA5_t *pCardAuthAckData);
extern bool sPrivDrvPktRecv05(const u8 *pBuf, i32 i32Len);

#elif (cSdkPrivDevType == cSdkPrivDevTypeS)

extern bool sPrivDrvPktSend05(u8 u8GunId, stPrivDrvCmd05_t *pCardAuth);
extern bool sPrivDrvPktRecvA5(const u8 *pBuf, i32 i32Len);

#endif





















#endif

















