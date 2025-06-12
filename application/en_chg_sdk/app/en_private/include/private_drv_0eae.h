/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_0eae.h
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-11-06
* notice                                :     本文件负责实现内网协议中 0E/AE 报文
****************************************************************************************************/
#ifndef _private_drv_0eae_H_
#define _private_drv_0eae_H_
#include "en_common.h"
#include "private_drv_data_def.h"













//08报文 发送周期------充电开始一段时间以后
#ifndef cSdkPrivDrvCycle0E
#define cPrivDrvCycle0E                 (1)
#else
#define cPrivDrvCycle0E                 (cSdkPrivDrvCycle0E)
#endif



//08报文 快速发送周期------刚开始充电
#ifndef cSdkPrivDrvCycle0EFast
#define cPrivDrvCycle0EFast             (1)
#else
#define cPrivDrvCycle0EFast             (cSdkPrivDrvCycle0EFast)
#endif



//08报文 慢速发送周期------未充电时
#ifndef cSdkPrivDrvCycle0ESlow
#define cPrivDrvCycle0ESlow             (30)
#else
#define cPrivDrvCycle0ESlow             (cSdkPrivDrvCycle0ESlow)
#endif





//报文 接收打印
#ifndef cSdkPrivDrv0ERecvPrintf
#define cPrivDrv0ERecvPrintf            (0)
#else
#define cPrivDrv0ERecvPrintf            (cSdkPrivDrv0ERecvPrintf)
#endif













#if (cSdkPrivDevType == cSdkPrivDevTypeM)

extern bool sPrivDrvPktSendAE(u8 u8GunId);
extern bool sPrivDrvPktRecv0E(const u8 *pBuf, i32 i32Len);

#elif (cSdkPrivDevType == cSdkPrivDevTypeS)

extern bool sPrivDrvPktSend0E(u8 u8GunId);
extern bool sPrivDrvPktRecvAE(const u8 *pBuf, i32 i32Len);

#endif





















#endif

















