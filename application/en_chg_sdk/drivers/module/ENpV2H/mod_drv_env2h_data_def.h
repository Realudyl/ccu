/**
 * @file mod_drv_env2h_data_def.h
 * @author anlada (bo.hou@en-plus.com.cn)
 * @brief en+ 双向模块协议数据定义
 * @version 0.1
 * @date 2025-02-11
 * 
 * @copyright Shenzhen EN Plus Technologies Co., Ltd. 2015-2025. All rights reserved 
 * 
 */

#pragma once

#include "en_common.h"

/**
 * @defgroup ENpV2H 双向模块协议数据定义
 * @brief 此模块定义了en+双向模块协议的数据结构和相关功能。
 * @{
 */

/**
 * @brief 数据编号枚举
 */
typedef enum {
    eDCVoltage,
    eDCCurrent,
    eModuleStatus2,
    eModuleStatus1,
    eModuleStatus0,
    eInverterStatus2,
    eInverterStatus1,
    eInverterStatus0,
    ePhaseAVoltage,
    ePhaseBVoltage,
    ePhaseCVoltage,
    ePhaseACurrent,
    ePhaseBCurrent,
    ePhaseCCurrent,
    eFrequency,
    eActivePowerA,
    eActivePowerB,
    eActivePowerC,
    eReactivePowerA,
    eReactivePowerB,
    eReactivePowerC,
    eSetDCVoltage,
    eSetDCCurrent,
    ePowerOnOff,
    eOffGridVoltageSetting,
    eOffGridFrequencySetting,
    eACReactivePowerSetting,
    eOperationMode,
    eACActivePowerSetting,
} eDataIndex_t;

/**
 * @brief 双向模块协议数据结构
 */
typedef struct stModDrvV2H_t
{
    eDataIndex_t                        eIndex;                                  //数据编号
    u8                                  u8Cmd;                                  //命令号
    u8                                  u8GroupNum;                             //数据组编号
    u8                                  u8DataNum;                              //数据编号
    u8                                  u8DataStart;                            //数据起始字节偏移
    u8                                  u8DataL;                                //数据
    void*                               pSingleParemeter;                       //单个模块参数指针
    void*                               pGroupParemeter;                        //组模块参数指针(预留)
}stModDrvV2H_t;

extern stModDrvV2H_t stV2HMap[];

#define V2HMapMaxSize (sizeof(stV2HMap)/sizeof(stModDrvV2H_t))

#define V2H_DeviceSingle                0x0C                                    /*单播设备号*/
#define V2H_DeviceGroup                 0x0D                                    /*组播设备号*/

typedef enum
{
    eRectification                      = 0xA0,                                  /*整流模式*/
    eGridConnectedInverter              = 0xA1,                                  /*电网并网逆变模式*/
    eOffGridInverter                    = 0xA2,                                  /*离网逆变模式*/
}eModV2HOperationMode_t;

typedef struct stSingleData_t
{
    f32                                  f32DCVoltage;                           // 直流电压 mV
    f32                                  f32DCCurrent;                           // 直流电流 mA
    u8                                   u8ModuleStatus2;                        // 模块状态2
    u8                                   u8ModuleStatus1;                        // 模块状态1
    u8                                   u8ModuleStatus0;                        // 模块状态0
    u8                                   u8InverterStatus2;                      // 逆变状态2
    u8                                   u8InverterStatus1;                      // 逆变状态1
    u8                                   u8InverterStatus0;                      // 逆变状态0
    f32                                  f32PhaseAVoltage;                       // 交流A相相电压 mV
    f32                                  f32PhaseBVoltage;                       // 交流B相相电压 mV
    f32                                  f32PhaseCVoltage;                       // 交流C相相电压 mV
    f32                                  f32PhaseACurrent;                       // 交流A相相电流 mA
    f32                                  f32PhaseBCurrent;                       // 交流B相相电流 mA
    f32                                  f32PhaseCCurrent;                       // 交流C相相电流 mA
    f32                                  f32Frequency;                           // 交流频率 mHz
    f32                                  f32ActivePowerA;                        // A相有功功率 mW
    f32                                  f32ActivePowerB;                        // B相有功功率 mW
    f32                                  f32ActivePowerC;                        // C相有功功率 mW
    f32                                  f32ReactivePowerA;                      // A相无功功率 mVar
    f32                                  f32ReactivePowerB;                      // B相无功功率 mVar
    f32                                  f32ReactivePowerC;                      // C相无功功率 mVar
    f32                                  f32SetDCVoltage;                        // 设置直流电压 mV
    f32                                  f32SetDCCurrent;                        // 设置直流电流 mA
    u16                                  u16SetPowerOnOff;                       // 开关机状态
    f32                                  f32SetOffGridVoltageSetting;            // 离网电压设置 V
    f32                                  f32SetOffGridFrequencySetting;          // 离网频率设置 Hz
    f32                                  f32SetACReactivePowerSetting;           // AC侧无功功率设置 mVar
    u16                                  u16SetOperationMode;                    // 工作模式
    f32                                  f32SetActivePowerSetting;               // AC侧有功功率设置 mW
} stSingleData_t;

/**
 * @}
 */
