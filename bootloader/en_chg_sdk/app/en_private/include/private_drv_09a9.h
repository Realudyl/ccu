/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_09a9.h
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-10-22
* notice                                :     本文件负责实现内网协议中 09/A9 报文
****************************************************************************************************/
#ifndef _private_drv_09a9_H_
#define _private_drv_09a9_H_
#include "en_common.h"
#include "private_drv_data_def.h"
















#if (cSdkPrivDevType == cSdkPrivDevTypeM)

extern bool sPrivDrvPktSendA9(u8 u8GunId, ePrivDrvCmdRst_t eRst, i32 i32OrderIdLen, const u8 *pOrderId, i32 i32SrvTradeIdLen, const u8 *pSrvTradeId);
extern bool sPrivDrvPktRecv09(const u8 *pBuf, i32 i32Len);

#elif (cSdkPrivDevType == cSdkPrivDevTypeS)

extern bool sPrivDrvPktSend09(stPrivDrvCmd09_t *pRcdData);
extern bool sPrivDrvPktRecvA9(const u8 *pBuf, i32 i32Len);

#endif





















#endif

















