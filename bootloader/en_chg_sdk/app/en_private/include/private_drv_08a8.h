/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_08a8.h
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-10-22
* notice                                :     本文件负责实现内网协议中 08/A8 报文
****************************************************************************************************/
#ifndef _private_drv_08a8_H_
#define _private_drv_08a8_H_
#include "en_common.h"
#include "private_drv_data_def.h"












//08报文 发送周期------充电开始一段时间以后
#ifndef cSdkPrivDrvCycle08
#define cPrivDrvCycle08                 (1)
#else
#define cPrivDrvCycle08                 (cSdkPrivDrvCycle08)
#endif



//08报文 快速发送周期------刚开始充电
#ifndef cSdkPrivDrvCycle08Fast
#define cPrivDrvCycle08Fast             (1)
#else
#define cPrivDrvCycle08Fast             (cSdkPrivDrvCycle08Fast)
#endif









#if (cSdkPrivDevType == cSdkPrivDevTypeM)

extern bool sPrivDrvPktSendA8(u8 u8GunId, ePrivDrvCmdRst_t eRst, u32 u32AccountBalance, i32 i32RsvdLen, const u8 *pRsvd);
extern bool sPrivDrvPktRecv08(const u8 *pBuf, i32 i32Len);

#elif (cSdkPrivDevType == cSdkPrivDevTypeS)

extern bool sPrivDrvPktSend08(u8 u8GunId, stPrivDrvCmd09_t *pRcdData);
extern bool sPrivDrvPktRecvA8(const u8 *pBuf, i32 i32Len);

#endif





















#endif

















