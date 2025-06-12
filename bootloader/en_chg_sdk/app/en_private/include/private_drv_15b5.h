/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_15b5.h
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2024-08-09
* notice                                :     本文件负责实现内网协议中 15/b5 报文
****************************************************************************************************/
#ifndef _private_drv_15b5_H_
#define _private_drv_15b5_H_
#include "en_common.h"
#include "private_drv_data_def.h"































#if (cSdkPrivDevType == cSdkPrivDevTypeM)

extern bool sPrivDrvPktSendB5(ePrivDrvCmdRst_t eRst);
extern bool sPrivDrvPktRecv15(const u8 *pBuf, i32 i32Len);

#elif (cSdkPrivDevType == cSdkPrivDevTypeS)

extern bool sPrivDrvPktSend15(stPrivDrvCmd15_t *pMpcData, u16 u16ActualDataLen);
extern bool sPrivDrvPktRecvB5(const u8 *pBuf, i32 i32Len);

#endif





















#endif

















