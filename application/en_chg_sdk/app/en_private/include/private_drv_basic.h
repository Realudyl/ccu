/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_basic.h
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-10-18
* notice                                :     负责实现协议中一些基础的功能，例如打印，校验，字段解析...
****************************************************************************************************/
#ifndef _private_drv_basic_H_
#define _private_drv_basic_H_
#include "en_common.h"
#include "private_drv_frame_def.h"

#if (cSdkPrivDevType == cSdkPrivDevTypeM)                                       //Master 侧是IoT SDK
#include "en_iot_sdk_cfg.h"
#else                                                                           //Slave  侧是chg SDK
#include "en_chg_sdk_cfg.h"
#endif







//打印最大值
#ifndef cSdkPrivDrvDataPrintMax
#define cPrivDrvDataPrintMax            (400)
#else
#define cPrivDrvDataPrintMax            (cSdkPrivDrvDataPrintMax)
#endif

//打印最大之后需要打印的值
#ifndef cSdkPrivDrvDataPrintMaxVaule
#define cPrivDrvDataPrintMaxVaule       (0)
#else
#define cPrivDrvDataPrintMaxVaule       (cSdkPrivDrvDataPrintMaxVaule)
#endif
























extern void sPrivDrvDataPrint(bool bSendFlag, const u8 *pBuf, u16 u16Len);
extern bool sPrivDrvCheckCrc(const u8  *pBuf, i32 i32Len);
extern char *sPrivDrvTimeToStr(void *pDat);






#endif


















