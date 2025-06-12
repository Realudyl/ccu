/**
 * @file mod_drv_env2h.c
 * @author anlada (bo.hou@en-plus.com.cn)
 * @brief 电源模块驱动资源初始化等
 * @version 0.1
 * @date 2025-02-12
 * 
 * @copyright Shenzhen EN Plus Technologies Co., Ltd. 2015-2025. All rights reserved 
 * 
 */
#include "mod_drv_env2h.h"
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "mod_drv_env2h.c";

bool modDrvSend(u32 u32FrameId, u8 *pu8Data);
bool modDrvRecv(u32* pFrameId, u8 *pu8Data);
void ModDrvV2HSendTask(void *pParam);
void ModDrvV2HRecvTask(void *pParam);

/**
 * @brief 发送回调函数
 */
bool modDrvSend(u32 u32FrameId, u8 *pu8Data)
{
    stCanMsg_t stFrame;
    u32 u32Id = 0x80000000 | u32FrameId;
    stFrame.u32Id = u32Id;
    stFrame.u8Len = 8;
    memmove(stFrame.u8Data, pu8Data, 8);

    return sCanDrvSend(cSdkModCanPort, &stFrame);
}

/**
 * @brief 接收回调函数
 */
bool modDrvRecv(u32* pFrameId, u8 *pu8Data)
{
    stCanMsg_t stFrame;
    bool bFlag = false;

    bFlag = sCanDrvRecv(cSdkModCanPort, &stFrame);
    if(bFlag)
    {
        *pFrameId = stFrame.u32Id & 0x1FFFFFFF;
        memmove(pu8Data, stFrame.u8Data, 8);
    }

    return bFlag;
}


/**
 * @brief 电源模块驱动资源初始化
 * 
 * @param i32Port CAN端口号
 * @param i32Baudrate 波特率
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2025-02-12
 */
bool ModDrvV2HInit(i32 i32Port, i32 i32Baudrate)
{
    stModDrvV2HOpt_t* pV2hOpt = ModDrvV2HGetOpt();

    if(!sCanDrvOpen(i32Port, i32Baudrate))
    {
        EN_SLOGE(TAG, "can open failed");
    }

    pV2hOpt->ModDrvV2HSendOpt = modDrvSend;
    pV2hOpt->ModDrvV2HRecvOpt  = modDrvRecv;

    xTaskCreate(ModDrvV2HSendTask, "ModDrvV2HSendTask", 1024, (void*)20, cModDrvV2HSendTaskPriority, NULL);
    xTaskCreate(ModDrvV2HRecvTask, "ModDrvV2HRecvTask", 1024, (void*)3 , cModDrvV2HRecvTaskPriority, NULL);

		return true;
}

/**
 * @brief 发送任务
 * 
 * @param pParam 周期ms
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2025-02-12
 */
void ModDrvV2HSendTask(void *pParam)
{
    EN_SLOGI(TAG, "任务建立:V2H模块协议发送任务");
    
    while(1)
    {
        u32 u32data = 0;

        ModDrvV2HSend(eDCVoltage, ModAddr, &u32data, V2H_DeviceSingle);
        ModDrvV2HSend(eDCCurrent, ModAddr, &u32data, V2H_DeviceSingle);
        ModDrvV2HSend(eModuleStatus2, ModAddr, &u32data, V2H_DeviceSingle);
        ModDrvV2HSend(eModuleStatus1, ModAddr, &u32data, V2H_DeviceSingle);
        ModDrvV2HSend(eModuleStatus0, ModAddr, &u32data, V2H_DeviceSingle);
        ModDrvV2HSend(eInverterStatus2, ModAddr, &u32data, V2H_DeviceSingle);
        ModDrvV2HSend(eInverterStatus1, ModAddr, &u32data, V2H_DeviceSingle);
        ModDrvV2HSend(eInverterStatus0, ModAddr, &u32data, V2H_DeviceSingle);

        vTaskDelay(pdMS_TO_TICKS(*(u32*)(pParam)));
    }
}

/**
 * @brief 接收任务
 * 
 * @param pParam 周期ms
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2025-02-12
 */
void ModDrvV2HRecvTask(void *pParam)
{
    EN_SLOGI(TAG, "任务建立:V2H模块协议发送任务");
    
    while(1)
    {
        ModDrvV2HRecv();
        vTaskDelay(pdMS_TO_TICKS(*(u32*)(pParam)));
    }
}

/**
 * @brief 打开电源模块
 * 
 * @param u8Addr 地址
 * @param bGroupFlag 是否组地址
 * @return true 
 * @return false 
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2025-02-12
 */
bool sModDrvV2HOpen(u8 u8Addr, bool bGroupFlag)
{
    u8 u8Device = 0;
    u16 u16Value = 0xA0;
    if(bGroupFlag)
    {
        u8Device = V2H_DeviceGroup;
    }
    else
    {
        u8Device = V2H_DeviceSingle;
    }

    ModDrvV2HSend(ePowerOnOff, u8Addr, &u16Value, V2H_DeviceSingle);

    return true;
}

/**
 * @brief 关闭电源模块
 * 
 * @param u8Addr 地址
 * @param bGroupFlag 是否组地址
 * @return true 
 * @return false 
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2025-02-12
 */
bool sModDrvV2HClose(u8 u8Addr, bool bGroupFlag)
{
    u8 u8Device = 0;
    u16 u16Value = 0xA1;
    if(bGroupFlag)
    {
        u8Device = V2H_DeviceGroup;
    }
    else
    {
        u8Device = V2H_DeviceSingle;
    }

    ModDrvV2HSend(ePowerOnOff, u8Addr, &u16Value, V2H_DeviceSingle);

    return true;
}

/**
 * @brief 设置输出
 * 
 * @param u8Addr 地址
 * @param bGroupFlag 是否组地址
 * @param f32Udc 输出电压
 * @param f32Idc 输出电流
 * @return true 
 * @return false 
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2025-02-12
 */
bool sModDrvV2HSetOutput(u8 u8Addr, bool bGroupFlag, f32 f32Udc, f32 f32Idc)
{
    u8 u8Device = 0;
    if(bGroupFlag)
    {
        u8Device = V2H_DeviceGroup;
    }
    else
    {
        u8Device = V2H_DeviceSingle;
    }

    ModDrvV2HSend(eSetDCVoltage, u8Addr, &f32Udc, V2H_DeviceSingle);
    ModDrvV2HSend(eSetDCCurrent, u8Addr, &f32Idc, V2H_DeviceSingle);

    return true;
}

/**
 * @brief 获取输出
 * 
 * @param u8Addr 地址
 * @param pUdc 输出电压
 * @param pIdc 输出电流
 * @return true 
 * @return false 
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2025-02-12
 */
bool sModDrvV2HGetOutput(u8 u8Addr, f32 *pUdc, f32 *pIdc)
{
    *pUdc = ModDrvV2HDCVoltage(0, 0);
    *pIdc = ModDrvV2HDCCurrent(0, 0);

    return true;
}

/**
 * @brief 设置工作模式
 * 
 * @param u8Addr 地址
 * @param u16Mode 工作模式
 * @return true 
 * @return false 
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2025-02-12
 */
bool sModDrvSerWorkMode(u8 u8Addr, u16 u16Mode)
{
    ModDrvV2HSend(eOperationMode, u8Addr, &u16Mode, V2H_DeviceSingle);

    return true;
}