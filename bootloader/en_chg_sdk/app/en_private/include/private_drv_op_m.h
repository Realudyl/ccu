/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :     private_drv_op_m.h
* Description                           :     
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2024-05-27
* notice                                :     
****************************************************************************************************/
#ifndef _private_drv_op_m_H_
#define _private_drv_op_m_H_
#include "en_common.h"
#include "private_drv_frame_def.h"

















#if   (cSdkPrivDevType == cSdkPrivDevTypeM)                                     //Master 侧才有这些操作
extern void sPrivDrvInitLogLevel(void);                                         //esp32  日志等级设定
extern void sPrivDrvPktLogin(void);
#endif







#endif






















