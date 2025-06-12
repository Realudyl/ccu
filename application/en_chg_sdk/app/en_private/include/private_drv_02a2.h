/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_02a2.h
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-10-19
* notice                                :     本文件负责实现内网协议中 02/A2 报文
****************************************************************************************************/
#ifndef _private_drv_02a2_H_
#define _private_drv_02a2_H_
#include "en_common.h"
#include "private_drv_data_def.h"











//02报文 发送周期
#ifndef cSdkPrivDrvCycle02
#define cPrivDrvCycle02                 (30)
#else
#define cPrivDrvCycle02                 (cSdkPrivDrvCycle02)
#endif



//重登陆间隔
#ifndef cSdkPrivDrvReloginGap
#define cPrivDrvReloginGap              (10)
#else
#define cPrivDrvReloginGap              (cSdkPrivDrvReloginGap)
#endif



//报文 接收打印
#ifndef cSdkPrivDrv02RecvPrintf
#define cPrivDrv02RecvPrintf            (0)
#else
#define cPrivDrv02RecvPrintf            (cSdkPrivDrv02RecvPrintf)
#endif





#if (cSdkPrivDevType == cSdkPrivDevTypeM)

extern bool sPrivDrvPktSendA2(void);
extern bool sPrivDrvPktRecv02(const u8 *pBuf, i32 i32Len);

#elif (cSdkPrivDevType == cSdkPrivDevTypeS)

extern bool sPrivDrvPktSend02(void);
extern bool sPrivDrvPktRecvA2(const u8 *pBuf, i32 i32Len);

#endif





















#endif

















