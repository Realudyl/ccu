/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_13b3.h
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     XRG
* Creat Date                            :     2024-05-20
* notice                                :     本文件负责实现内网协议中 13/B3 报文 
****************************************************************************************************/
#ifndef _private_drv_13b3_H_
#define _private_drv_13b3_H_
#include "en_common.h"
#include "private_drv_data_def.h"










//13报文 发送周期------充电开始一段时间以后
#ifndef cSdkPrivDrvCycle13
#define cPrivDrvCycle13                 (1)
#else
#define cPrivDrvCycle13                 (cSdkPrivDrvCycle13)
#endif



//13报文 快速发送周期------刚开始充电
#ifndef cSdkPrivDrvCycle13Fast
#define cPrivDrvCycle13Fast             (1)
#else
#define cPrivDrvCycle13Fast             (cSdkPrivDrvCycle13Fast)
#endif









#if (cSdkPrivDevType == cSdkPrivDevTypeM)

extern bool sPrivDrvPktSendB3(u8 u8GunId, u16 u16Seq);
extern bool sPrivDrvPktRecv13(const u8 *pBuf, i32 i32Len);

#elif (cSdkPrivDevType == cSdkPrivDevTypeS)

extern bool sPrivDrvPktSend13(u8 u8GunId, stPrivDrvCmd13_t *pBmsData);
extern bool sPrivDrvPktRecvB3(const u8 *pBuf, i32 i32Len);

#endif





















#endif

















