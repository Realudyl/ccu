/**
 * @file mod_drv_env2h_api.c
 * @author anlada (bo.hou@en-plus.com.cn)
 * @brief ENpV2H模块驱动API接口
 * @version 0.1
 * @date 2025-02-12
 * 
 * @copyright Shenzhen EN Plus Technologies Co., Ltd. 2015-2025. All rights reserved 
 * 
 */
#include "mod_drv_env2h_api.h"
#include "mod_drv_env2h_data_def.h"

stModDrvV2HOpt_t stModDrvV2HOpt = {NULL, NULL};

/**
 * @brief 获取电源收发模块驱动
 */
stModDrvV2HOpt_t* ModDrvV2HGetOpt(void)
{
    return &stModDrvV2HOpt;
}

stSingleData_t stSingleData = {0};

/**
 * @brief 获取单播数据
 */
stSingleData_t* ModDrvV2HGingleData(void)
{
    return &stSingleData;
}

stModDrvV2H_t stV2HMap[] = 
{
    {eDCVoltage,                0x23, 0x11, 0x01, 0x04, 0x04, &stSingleData.f32DCVoltage,             NULL},  // 模块直流侧电压 mV
    {eDCCurrent,                0x23, 0x11, 0x02, 0x04, 0x04, &stSingleData.f32DCCurrent,             NULL},  // 模块直流侧电流 mA
    {eModuleStatus2,            0x23, 0x11, 0x10, 0x01, 0x01, &stSingleData.u8ModuleStatus2,          NULL},  // 模块状态2
    {eModuleStatus1,            0x23, 0x11, 0x10, 0x01, 0x01, &stSingleData.u8ModuleStatus1,          NULL},  // 模块状态1
    {eModuleStatus0,            0x23, 0x11, 0x10, 0x01, 0x01, &stSingleData.u8ModuleStatus0,          NULL},  // 模块状态0
    {eInverterStatus2,          0x23, 0x11, 0x11, 0x01, 0x01, &stSingleData.u8InverterStatus2,        NULL},  // 逆变状态2
    {eInverterStatus1,          0x23, 0x11, 0x11, 0x01, 0x01, &stSingleData.u8InverterStatus1,        NULL},  // 逆变状态1
    {eInverterStatus0,          0x23, 0x11, 0x11, 0x01, 0x01, &stSingleData.u8InverterStatus0,        NULL},  // 逆变状态0
    {ePhaseAVoltage,            0x23, 0x21, 0x01, 0x04, 0x04, &stSingleData.f32PhaseAVoltage,         NULL},  // 交流A相相电压 mV
    {ePhaseBVoltage,            0x23, 0x21, 0x02, 0x04, 0x04, &stSingleData.f32PhaseBVoltage,         NULL},  // 交流B相相电压 mV
    {ePhaseCVoltage,            0x23, 0x21, 0x03, 0x04, 0x04, &stSingleData.f32PhaseCVoltage,         NULL},  // 交流C相相电压 mV
    {ePhaseACurrent,            0x23, 0x21, 0x04, 0x04, 0x04, &stSingleData.f32PhaseACurrent,         NULL},  // 交流A相相电流 mA
    {ePhaseBCurrent,            0x23, 0x21, 0x05, 0x04, 0x04, &stSingleData.f32PhaseBCurrent,         NULL},  // 交流B相相电流 mA
    {ePhaseCCurrent,            0x23, 0x21, 0x06, 0x04, 0x04, &stSingleData.f32PhaseCCurrent,         NULL},  // 交流C相相电流 mA
    {eFrequency,                0x23, 0x21, 0x07, 0x04, 0x04, &stSingleData.f32Frequency,             NULL},  // 交流频率 mHz
    {eActivePowerA,             0x23, 0x21, 0x09, 0x04, 0x04, &stSingleData.f32ActivePowerA,          NULL},  // 交流A相有功功率 mW
    {eActivePowerB,             0x23, 0x21, 0x0A, 0x04, 0x04, &stSingleData.f32ActivePowerB,          NULL},  // 交流B相有功功率 mW
    {eActivePowerC,             0x23, 0x21, 0x0B, 0x04, 0x04, &stSingleData.f32ActivePowerC,          NULL},  // 交流C相有功功率 mW
    {eReactivePowerA,           0x23, 0x21, 0x0D, 0x04, 0x04, &stSingleData.f32ReactivePowerA,        NULL},  // A相无功功率 mVar
    {eReactivePowerB,           0x23, 0x21, 0x0E, 0x04, 0x04, &stSingleData.f32ReactivePowerB,        NULL},  // B相无功功率 mVar
    {eReactivePowerC,           0x23, 0x21, 0x0F, 0x04, 0x04, &stSingleData.f32ReactivePowerC,        NULL},  // C相无功功率 mVar
    {eSetDCVoltage,             0x24, 0x11, 0x01, 0x04, 0x04, &stSingleData.f32SetDCVoltage,          NULL},  // 设置直流电压 mV
    {eSetDCCurrent,             0x24, 0x11, 0x02, 0x04, 0x04, &stSingleData.f32SetDCCurrent,          NULL},  // 设置直流电流 mA
    {ePowerOnOff,               0x24, 0x11, 0x10, 0x06, 0x02, &stSingleData.u16SetPowerOnOff,            NULL},  // 开关机状态
    {eOffGridVoltageSetting,    0x24, 0x21, 0x06, 0x04, 0x04, &stSingleData.f32SetOffGridVoltageSetting, NULL},  // 离网电压设置 V
    {eOffGridFrequencySetting,  0x24, 0x21, 0x07, 0x04, 0x04, &stSingleData.f32SetOffGridFrequencySetting, NULL},  // 离网频率设置 Hz
    {eACReactivePowerSetting,   0x24, 0x21, 0x08, 0x04, 0x04, &stSingleData.f32SetACReactivePowerSetting, NULL},  // AC侧无功功率设置 mVar
    {eOperationMode,            0x24, 0x21, 0x10, 0x04, 0x04, &stSingleData.u16SetOperationMode,         NULL},  // 工作模式
    {eACActivePowerSetting,     0x24, 0x31, 0x23, 0x04, 0x04, &stSingleData.f32SetActivePowerSetting,  NULL},  // AC侧有功功率设置 mW
};

/**
 * @brief 获取数据映射表
 * 
 * @return stModDrvV2H_t* 
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2025-02-11
 */
stModDrvV2H_t* ModDrvV2HGetDataMap(void)
{
    return stV2HMap;
}

/**
 * @brief 更新单个参数
 * 
 * @param u8Device 设备类型
 * @param u8Cmd 命令
 * @param pData 数据
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2025-02-11
 */
void ModDrvV2HUpdateSingleParameter(u8 u8Device, u8 u8Cmd, u8* pData)
{
    u8 u8GroupNum = pData[0];
    u8 u8DataNum = pData[1];

    stModDrvV2H_t* pDataMap = ModDrvV2HGetDataMap();
    // 遍历stV2HMap数组，查找符合条件的项
    for (size_t i = 0; i < V2HMapMaxSize; ++i) {
        if (pDataMap[i].u8Cmd == u8Cmd &&
            pDataMap[i].u8GroupNum == u8GroupNum &&
            pDataMap[i].u8DataNum == u8DataNum)
        {
            // 使用memmove将数据复制到pSingleParemeter
            if(u8Device == V2H_DeviceSingle)
            {
                memmove(pDataMap[i].pSingleParemeter, (void const*)(&pData[pDataMap[i].u8DataStart]), pDataMap[i].u8DataL);
            }
            /*暂时不支持
            else if(u8Device == V2H_DeviceGroup)
            {
                memmove(pDataMap[i].pGroupParemeter, (void const*)(&pData[pDataMap[i].u8DataStart]), pDataMap[i].u8DataL);
            }*/
            return;
        }
    }
}

/**
 * @brief 接收数据处理
 * 
 * @return true 返回true表示接收的数据太多了，处理了100次还没处理完。
 * @return false 
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2025-02-12
 */
bool ModDrvV2HRecv(void)
{
    bool bFlag = true;
    stModDrvV2HOpt_t* pModDrv = ModDrvV2HGetOpt();
    u32 u32FrameId = 0;
    u8 u8Data[8] = {0};
    u8 u8Device = 0;
    u8 u8Cmd = 0;
    u8 u8Count = 100;
    u8 u8MaxCount = 100;

    if(pModDrv->ModDrvV2HRecvOpt == NULL)
    {
        return false;
    }

    while (bFlag)
    {
        bFlag = pModDrv->ModDrvV2HRecvOpt(&u32FrameId, &u8Data[0]);
        if(bFlag == false)
        {
            continue;
        }
        u8Device = ID2Device(u32FrameId);
        u8Cmd = ID2Cmd(u32FrameId);
        
        ModDrvV2HUpdateSingleParameter(u8Device, u8Cmd, &u8Data[0]);

        u8Count++;
        if(u8Count >= u8MaxCount)
        {
            break;
        }
    }
    return bFlag;
}

/**
 * @brief 获取参数元数据
 * 
 * @param index 参数索引
 * @param pData 参数元数据 部分2 [组号, 数据号, 数据起始地址, 数据长度]
 * @return u8 参数元数据 部分1 [命令]
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2025-02-11
 */
u8 ModDrvV2HMetaDataGet(eDataIndex_t index, u8* pData)
{
    stModDrvV2H_t* pDataMap = ModDrvV2HGetDataMap();
    for(size_t i = 0; i < V2HMapMaxSize; ++i)
    {
        if(pDataMap[i].eIndex == index)
        {
            pData[0] = pDataMap[i].u8GroupNum;
            pData[1] = pDataMap[i].u8DataNum;
            pData[2] = pDataMap[i].u8DataStart;
            pData[3] = pDataMap[i].u8DataL;
            return pDataMap[i].u8Cmd;
        }
    }

    return 0;
}


/**
 * @brief 发送命令
 * 
 * @param index 参数索引
 * @param pValue 参数值
 * @param u8Device 设备类型
 * @return true 
 * @return false 
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2025-02-12
 */
bool ModDrvV2HSend(eDataIndex_t index, u8 u8DestAddr, void* pValue, u8 u8Device)
{
    u8 u8Cmd = 0;
    u8 u8MetaData[4] = {0};
    u8 u8Data[8] = {0};
    u8 u8GroupNum = 0;
    u8 u8DataNum = 0;
    u8 u8DataStart = 0;
    u8 u8DataL = 0;
    u8 u8SourAddr = 0;
    u8 u32FrameId = 0;

    stModDrvV2HOpt_t* pModDrv = ModDrvV2HGetOpt();
    //查找元数据
    u8Cmd = ModDrvV2HMetaDataGet(index, &u8MetaData[0]);
    u8GroupNum = u8MetaData[0];
    u8DataNum = u8MetaData[1];
    u8DataStart = u8MetaData[2];
    u8DataL = u8MetaData[3];

    //生成需要发送的数据
    u8Data[0] = u8GroupNum;
    u8Data[1] = u8DataNum;
    memmove(&u8Data[u8DataStart], pValue, u8DataL);

    //发送命令
    if (pModDrv->ModDrvV2HSendOpt == NULL)
    {
        return false;
    }
    else
    {
        u32FrameId = SrcAddr2ID(MonitorAddr, u32FrameId);
        u32FrameId = DestAddr2ID(u8DestAddr, u32FrameId);
        u32FrameId = Cmd2ID(u8Cmd, u32FrameId);
        u32FrameId = Device2ID(u8Device, u32FrameId);

        pModDrv->ModDrvV2HSendOpt(u32FrameId, u8Data);

        return true;
    }
}


/**
 * @breif 读取：直流电压 mV
 */
f32 ModDrvV2HDCVoltage(u8 u8Addr, u8 u8Device)
{
    stSingleData_t* data = ModDrvV2HGingleData();
    return data->f32DCVoltage;
}

/**
 * @breif 读取：直流电流 mA
 */
f32 ModDrvV2HDCCurrent(u8 u8Addr, u8 u8Device)
{
    stSingleData_t* data = ModDrvV2HGingleData();
    return data->f32DCCurrent;
}

/**
 * @breif 读取：模块状态2
 */
u8 ModDrvV2HModuleStatus2(u8 u8Addr, u8 u8Device)
{
    stSingleData_t* data = ModDrvV2HGingleData();
    return data->u8ModuleStatus2;
}

/**
 * @breif 读取：模块状态1
 */
u8 ModDrvV2HModuleStatus1(u8 u8Addr, u8 u8Device)
{
    stSingleData_t* data = ModDrvV2HGingleData();
    return data->u8ModuleStatus1;
}

/**
 * @breif 读取：模块状态0
 */
u8 ModDrvV2HModuleStatus0(u8 u8Addr, u8 u8Device)
{
    stSingleData_t* data = ModDrvV2HGingleData();
    return data->u8ModuleStatus0;
}

/**
 * @breif 读取：逆变状态2
 */
u8 ModDrvV2HInverterStatus2(u8 u8Addr, u8 u8Device)
{
    stSingleData_t* data = ModDrvV2HGingleData();
    return data->u8InverterStatus2;
}

/**
 * @breif 读取：逆变状态1
 */
u8 ModDrvV2HInverterStatus1(u8 u8Addr, u8 u8Device)
{
    stSingleData_t* data = ModDrvV2HGingleData();
    return data->u8InverterStatus1;
}

/**
 * @breif 读取：逆变状态0
 */
u8 ModDrvV2HInverterStatus0(u8 u8Addr, u8 u8Device)
{
    stSingleData_t* data = ModDrvV2HGingleData();
    return data->u8InverterStatus0;
}

/**
 * @breif 读取：交流A相相电压 mV
 */
f32 ModDrvV2HPhaseAVoltage(u8 u8Addr, u8 u8Device)
{
    stSingleData_t* data = ModDrvV2HGingleData();
    return data->f32PhaseAVoltage;
}

/**
 * @breif 读取：交流B相相电压 mV
 */
f32 ModDrvV2HPhaseBVoltage(u8 u8Addr, u8 u8Device)
{
    stSingleData_t* data = ModDrvV2HGingleData();
    return data->f32PhaseBVoltage;
}

/**
 * @breif 读取：交流C相相电压 mV
 */
f32 ModDrvV2HPhaseCVoltage(u8 u8Addr, u8 u8Device)
{
    stSingleData_t* data = ModDrvV2HGingleData();
    return data->f32PhaseCVoltage;
}

/**
 * @breif 读取：交流A相相电流 mA
 */
f32 ModDrvV2HPhaseACurrent(u8 u8Addr, u8 u8Device)
{
    stSingleData_t* data = ModDrvV2HGingleData();
    return data->f32PhaseACurrent;
}

/**
 * @breif 读取：交流B相相电流 mA
 */
f32 ModDrvV2HPhaseBCurrent(u8 u8Addr, u8 u8Device)
{
    stSingleData_t* data = ModDrvV2HGingleData();
    return data->f32PhaseBCurrent;
}

/**
 * @breif 读取：交流C相相电流 mA
 */
f32 ModDrvV2HPhaseCCurrent(u8 u8Addr, u8 u8Device)
{
    stSingleData_t* data = ModDrvV2HGingleData();
    return data->f32PhaseCCurrent;
}

/**
 * @breif 读取：交流频率 mHz
 */
f32 ModDrvV2HFrequency(u8 u8Addr, u8 u8Device)
{
    stSingleData_t* data = ModDrvV2HGingleData();
    return data->f32Frequency;
}

/**
 * @breif 读取：A相有功功率 mW
 */
f32 ModDrvV2HActivePowerA(u8 u8Addr, u8 u8Device)
{
    stSingleData_t* data = ModDrvV2HGingleData();
    return data->f32ActivePowerA;
}

/**
 * @breif 读取：B相有功功率 mW
 */
f32 ModDrvV2HActivePowerB(u8 u8Addr, u8 u8Device)
{
    stSingleData_t* data = ModDrvV2HGingleData();
    return data->f32ActivePowerB;
}

/**
 * @breif 读取：C相有功功率 mW
 */
f32 ModDrvV2HActivePowerC(u8 u8Addr, u8 u8Device)
{
    stSingleData_t* data = ModDrvV2HGingleData();
    return data->f32ActivePowerC;
}

/**
 * @breif 读取：A相无功功率 mVar
 */
f32 ModDrvV2HReactivePowerA(u8 u8Addr, u8 u8Device)
{
    stSingleData_t* data = ModDrvV2HGingleData();
    return data->f32ReactivePowerA;
}

/**
 * @breif 读取：B相无功功率 mVar
 */
f32 ModDrvV2HReactivePowerB(u8 u8Addr, u8 u8Device)
{
    stSingleData_t* data = ModDrvV2HGingleData();
    return data->f32ReactivePowerB;
}

/**
 * @breif 读取：C相无功功率 mVar
 */
f32 ModDrvV2HReactivePowerC(u8 u8Addr, u8 u8Device)
{
    stSingleData_t* data = ModDrvV2HGingleData();
    return data->f32ReactivePowerC;
}



