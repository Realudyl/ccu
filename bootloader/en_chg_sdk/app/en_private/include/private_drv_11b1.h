/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_11b1.h
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     XRG
* Creat Date                            :     2024-06-19
* notice                                :     本文件负责实现内网协议中 11/B1 报文 
****************************************************************************************************/
#ifndef _private_drv_11b1_H_
#define _private_drv_11b1_H_
#include "en_common.h"
#include "private_drv_data_def.h"



















#if (cSdkPrivDevType == cSdkPrivDevTypeM)

extern bool sPrivDrvPktSendB1(stPrivDrvCmdB1_t *pEvtNoticeAck);
extern bool sPrivDrvPktRecv11(const u8 *pBuf, i32 i32Len);

#elif (cSdkPrivDevType == cSdkPrivDevTypeS)

extern bool sPrivDrvPktSend11(stPrivDrvCmd11_t *pEvtNotice);
extern bool sPrivDrvPktRecvB1(const u8 *pBuf, i32 i32Len);

#endif






















#endif

















