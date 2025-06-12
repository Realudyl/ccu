/**
 * @file mod_drv_env2h_api.h
 * @author anlada (bo.hou@en-plus.com.cn)
 * @brief ENpV2H模块驱动API接口
 * @version 0.1
 * @date 2025-02-12
 * 
 * @copyright Shenzhen EN Plus Technologies Co., Ltd. 2015-2025. All rights reserved 
 * 
 */
#pragma once

#include "mod_drv_env2h_data_def.h"

typedef struct stModDrvV2HOpt_t{
    bool (*ModDrvV2HSendOpt)(u32 u32FrameId, u8 *pu8Data);
    bool (*ModDrvV2HRecvOpt)(u32 *pFrameId, u8 *pu8Data);
} stModDrvV2HOpt_t;

extern stModDrvV2HOpt_t stModDrvV2HOpt;

#define ID2Device(id) (((id) & 0x3c00000) >> 22)
#define ID2Cmd(id)    (((id) & 0x3f0000) >> 16)

#define Device2ID(dev, id) (((dev) << 22) | ((id) & (~0x3c00000)))
#define Cmd2ID(cmd, id)    (((cmd) << 16) | ((id) & (~0x3f0000)))
#define DestAddr2ID(addr, id) (((addr) << 8) | ((id) & (~0xff00)))
#define SrcAddr2ID(addr, id) ((addr) | ((id) & (~0xff)))

#define MonitorAddr 0xF0

stModDrvV2HOpt_t* ModDrvV2HGetOpt(void);
stSingleData_t* ModDrvV2HGingleData(void);
stModDrvV2H_t* ModDrvV2HGetDataMap(void);

void ModDrvV2HUpdateSingleParameter(u8 u8Device, u8 u8Cmd, u8* pData);
bool ModDrvV2HRecv(void);
u8 ModDrvV2HMetaDataGet(eDataIndex_t index, u8* pData);
bool ModDrvV2HSend(eDataIndex_t index, u8 u8DestAddr, void* pValue, u8 u8Device);
f32 ModDrvV2HDCVoltage(u8 u8Addr, u8 u8Device);
f32 ModDrvV2HDCCurrent(u8 u8Addr, u8 u8Device);
u8 ModDrvV2HModuleStatus2(u8 u8Addr, u8 u8Device);
u8 ModDrvV2HModuleStatus1(u8 u8Addr, u8 u8Device);
u8 ModDrvV2HModuleStatus0(u8 u8Addr, u8 u8Device);
u8 ModDrvV2HInverterStatus2(u8 u8Addr, u8 u8Device);
u8 ModDrvV2HInverterStatus1(u8 u8Addr, u8 u8Device);
u8 ModDrvV2HInverterStatus0(u8 u8Addr, u8 u8Device);
f32 ModDrvV2HPhaseAVoltage(u8 u8Addr, u8 u8Device);
f32 ModDrvV2HPhaseBVoltage(u8 u8Addr, u8 u8Device);
f32 ModDrvV2HPhaseCVoltage(u8 u8Addr, u8 u8Device);
f32 ModDrvV2HPhaseACurrent(u8 u8Addr, u8 u8Device);
f32 ModDrvV2HPhaseBCurrent(u8 u8Addr, u8 u8Device);
f32 ModDrvV2HPhaseCCurrent(u8 u8Addr, u8 u8Device);
f32 ModDrvV2HFrequency(u8 u8Addr, u8 u8Device);
f32 ModDrvV2HActivePowerA(u8 u8Addr, u8 u8Device);
f32 ModDrvV2HActivePowerB(u8 u8Addr, u8 u8Device);
f32 ModDrvV2HActivePowerC(u8 u8Addr, u8 u8Device);
f32 ModDrvV2HReactivePowerA(u8 u8Addr, u8 u8Device);
f32 ModDrvV2HReactivePowerB(u8 u8Addr, u8 u8Device);
f32 ModDrvV2HReactivePowerC(u8 u8Addr, u8 u8Device);
