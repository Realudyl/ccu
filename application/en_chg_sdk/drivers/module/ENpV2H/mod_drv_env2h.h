/**
 * @file mod_drv_env2.h
 * @author anlada (bo.hou@en-plus.com.cn)
 * @brief 
 * @version 0.1
 * @date 2025-02-12
 * 
 * @copyright Shenzhen EN Plus Technologies Co., Ltd. 2015-2025. All rights reserved 
 * 
 */
#pragma once
#include "can_drv_trans.h"
#include "mod_drv_env2h_api.h"


#ifndef cSdkModDrvRecvTaskPriority
#define cModDrvV2HRecvTaskPriority     (21)
#else
#define cModDrvV2HRecvTaskPriority     (cSdkModDrvRecvTaskPriority)
#endif

#ifndef cSdkModDrvSendTaskPriority
#define cModDrvV2HSendTaskPriority     (22)
#else
#define cModDrvV2HSendTaskPriority     (cSdkModDrvSendTaskPriority)
#endif

#define ModAddr 0x01



bool ModDrvV2HInit(i32 i32Port, i32 i32Baudrate);
bool sModDrvV2HOpen(u8 u8Addr, bool bGroupFlag);
bool sModDrvV2HClose(u8 u8Addr, bool bGroupFlag);
bool sModDrvV2HSetOutput(u8 u8Addr, bool bGroupFlag, f32 f32Udc, f32 f32Idc);
bool sModDrvV2HGetOutput(u8 u8Addr, f32 *pUdc, f32 *pIdc);
bool sModDrvSerWorkMode(u8 u8Addr, u16 u16Mode);