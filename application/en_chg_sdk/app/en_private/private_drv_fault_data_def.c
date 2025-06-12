/**
 * @file private_drv_fault_data_def.c
 * @author warn_infor.exe
 * @brief 故障组件数据定义
 * @version 0.0.19
 * @date 2025-02-19 10:51
 *
 * @copyright Shenzhen EN Plus Technologies Co., Ltd. 2015-2024. All rights reserved
 *
 */

#include "private_drv_fault_data_def.h"

const char* cVersion = "0.0.19";





stPrivDrvFaultData_t stPrivDrvFaultData[] = 
{
    {ePrivDrvFaultCode_devFault,                   ePrivDrvLeve1G3,  ePrivDrvSourceMod,  "devFault",                   "模块故障"},
    {ePrivDrvFaultCode_devProtect,                 ePrivDrvLeve1G3,  ePrivDrvSourceMod,  "devProtect",                 "模块告警"},
    {ePrivDrvFaultCode_sciComFault,                ePrivDrvLeve1G3,  ePrivDrvSourceMod,  "sciComFault",                "模块内部 SCI 通信故障"},
    {ePrivDrvFaultCode_dcdcOverVoltage,            ePrivDrvLeve1G3,  ePrivDrvSourceMod,  "dcdcOverVoltage",            "DCDC 过压"},
    {ePrivDrvFaultCode_pfcVoltageErr,              ePrivDrvLeve1G3,  ePrivDrvSourceMod,  "pfcVoltageErr",              "PFC 电压异常"},
    {ePrivDrvFaultCode_acInputOverVoltage,         ePrivDrvLeve1G3,  ePrivDrvSourceMod,  "acInputOverVoltage",         "交流过压"},
    {ePrivDrvFaultCode_acUndervolt,                ePrivDrvLeve1G3,  ePrivDrvSourceMod,  "acUndervolt",                "交流欠压"},
    {ePrivDrvFaultCode_canCommErr,                 ePrivDrvLeve1G3,  ePrivDrvSourceMod,  "canCommErr",                 "CAN 通信故障"},
    {ePrivDrvFaultCode_moduleCurrImbalance,        ePrivDrvLeve1G1,  ePrivDrvSourceMod,  "moduleCurrImbalance",        "模块不均流"},
    {ePrivDrvFaultCode_powerStatus,                ePrivDrvLeve1G3,  ePrivDrvSourceMod,  "powerStatus",                "DCDC 关机"},
    {ePrivDrvFaultCode_powerLimitation,            ePrivDrvLeve1G1,  ePrivDrvSourceMod,  "powerLimitation",            "模块限功率"},
    {ePrivDrvFaultCode_highTempPowerLimit,         ePrivDrvLeve1G1,  ePrivDrvSourceMod,  "highTempPowerLimit",         "温度限功率"},
    {ePrivDrvFaultCode_acPowerLimit,               ePrivDrvLeve1G1,  ePrivDrvSourceMod,  "acPowerLimit",               "交流限功率"},
    {ePrivDrvFaultCode_fanFault,                   ePrivDrvLeve1G1,  ePrivDrvSourceMod,  "fanFault",                   "风扇故障"},
    {ePrivDrvFaultCode_dcShort,                    ePrivDrvLeve1G4,  ePrivDrvSourceMod,  "dcShort",                    "DCDC 短路"},
    {ePrivDrvFaultCode_dcOverTemp,                 ePrivDrvLeve1G1,  ePrivDrvSourceMod,  "dcOverTemp",                 "DCDC 过温"},
    {ePrivDrvFaultCode_bmsCommErr,                 ePrivDrvLeve1G2,  ePrivDrvSourceBms,  "bmsCommErr",                 "BMS通讯异常"},
    {ePrivDrvFaultCode_bcpTimeout,                 ePrivDrvLeve1G1,  ePrivDrvSourceBms,  "bcpTimeout",                 "BCP充电参数配置报文超时"},
    {ePrivDrvFaultCode_broAaTimeout,               ePrivDrvLeve1G1,  ePrivDrvSourceBms,  "broAaTimeout",               "BRO充电准备就绪报文超时"},
    {ePrivDrvFaultCode_bcsTimeout,                 ePrivDrvLeve1G1,  ePrivDrvSourceBms,  "bcsTimeout",                 "BCS电池充电状态报文超时"},
    {ePrivDrvFaultCode_bclTimeout,                 ePrivDrvLeve1G1,  ePrivDrvSourceBms,  "bclTimeout",                 "BCL电池充电需求报文超时"},
    {ePrivDrvFaultCode_bstTimeout,                 ePrivDrvLeve1G1,  ePrivDrvSourceBms,  "bstTimeout",                 "BST中止充电报文超时"},
    {ePrivDrvFaultCode_bsdTimeout,                 ePrivDrvLeve1G1,  ePrivDrvSourceBms,  "bsdTimeout",                 "BSD充电统计数据报文超时"},
    {ePrivDrvFaultCode_bsmTimeout,                 ePrivDrvLeve1G1,  ePrivDrvSourceBms,  "bsmTimeout",                 "BSM动力蓄电池状态报文超时"},
    {ePrivDrvFaultCode_bhmMismErr,                 ePrivDrvLeve1G2,  ePrivDrvSourceBms,  "bhmMismErr",                 "BHM桩的输出能力不匹配"},
    {ePrivDrvFaultCode_brmTimeout,                 ePrivDrvLeve1G1,  ePrivDrvSourceBms,  "brmTimeout",                 "BRM车辆辨识报文超时"},
    {ePrivDrvFaultCode_bmsImdErr,                  ePrivDrvLeve1G2,  ePrivDrvSourceBms,  "bmsImdErr",                  "BMS绝缘故障"},
    {ePrivDrvFaultCode_bmsOverTemp,                ePrivDrvLeve1G2,  ePrivDrvSourceBms,  "bmsOverTemp",                "BMS元件过温"},
    {ePrivDrvFaultCode_bmsOverVolt,                ePrivDrvLeve1G2,  ePrivDrvSourceBms,  "bmsOverVolt",                "BMS电压过高"},
    {ePrivDrvFaultCode_bmsPChVoltErr,              ePrivDrvLeve1G2,  ePrivDrvSourceBms,  "bmsPChVoltErr",              "BMS预充电压不匹配"},
    {ePrivDrvFaultCode_batVoltHighErr,             ePrivDrvLeve1G2,  ePrivDrvSourceBms,  "batVoltHighErr",             "单体动力蓄电池电压过高"},
    {ePrivDrvFaultCode_batVoltLowErr,              ePrivDrvLeve1G2,  ePrivDrvSourceBms,  "batVoltLowErr",              "单体动力蓄电池电压过低"},
    {ePrivDrvFaultCode_batSocHighErr,              ePrivDrvLeve1G2,  ePrivDrvSourceBms,  "batSocHighErr",              "整车动力蓄电池SOC过高"},
    {ePrivDrvFaultCode_batSocLowErr,               ePrivDrvLeve1G2,  ePrivDrvSourceBms,  "batSocLowErr",               "整车动力蓄电池SOC过低"},
    {ePrivDrvFaultCode_batOverCurr,                ePrivDrvLeve1G2,  ePrivDrvSourceBms,  "batOverCurr",                "动力蓄电池充电过流"},
    {ePrivDrvFaultCode_batOverTemp,                ePrivDrvLeve1G2,  ePrivDrvSourceBms,  "batOverTemp",                "动力蓄电池温度过高"},
    {ePrivDrvFaultCode_batImdErr,                  ePrivDrvLeve1G2,  ePrivDrvSourceBms,  "batImdErr",                  "动力蓄电池绝缘故障"},
    {ePrivDrvFaultCode_batConnErr,                 ePrivDrvLeve1G2,  ePrivDrvSourceBms,  "batConnErr",                 "动力蓄电池连接器故障"},
    {ePrivDrvFaultCode_batUnderVoltErr,            ePrivDrvLeve1G2,  ePrivDrvSourceBms,  "batUnderVoltErr",            "电池欠压"},
    {ePrivDrvFaultCode_batOverVolt,                ePrivDrvLeve1G2,  ePrivDrvSourceBms,  "batOverVolt",                "电池过压"},
    {ePrivDrvFaultCode_croTimeout,                 ePrivDrvLeve1G1,  ePrivDrvSourceBms,  "croTimeout",                 "CRO充电机输出就绪超时"},
    {ePrivDrvFaultCode_ccsTimeout,                 ePrivDrvLeve1G1,  ePrivDrvSourceBms,  "ccsTimeout",                 "CCS充电机状态报文超时"},
    {ePrivDrvFaultCode_csTimeout,                  ePrivDrvLeve1G1,  ePrivDrvSourceBms,  "csTimeout",                  "CST充电机终止充电报文超时 "},
    {ePrivDrvFaultCode_csdTimeout,                 ePrivDrvLeve1G1,  ePrivDrvSourceBms,  "csdTimeout",                 "CSD充电统计数据报文超时"},
    {ePrivDrvFaultCode_bstTp2Volt,                 ePrivDrvLeve1G2,  ePrivDrvSourceBms,  "bstTp2Volt",                 "BST检测点2电压检测故障"},
    {ePrivDrvFaultCode_bclVoltHigh,                ePrivDrvLeve1G2,  ePrivDrvSourceBms,  "bclVoltHigh",                "BCL需求电压过高"},
    {ePrivDrvFaultCode_bclCurrHigh,                ePrivDrvLeve1G2,  ePrivDrvSourceBms,  "bclCurrHigh",                "BCL需求电流过高"},
    {ePrivDrvFaultCode_ccuSelfTestErr,             ePrivDrvLeve1G3,  ePrivDrvSourceCcu,  "ccuSelfTestErr",             "设备自检失败"},
    {ePrivDrvFaultCode_dispenserDoorErr,           ePrivDrvLeve1G3,  ePrivDrvSourceCcu,  "dispenserDoorErr",           "终端柜门被打开"},
    {ePrivDrvFaultCode_ccuEstopErr,                ePrivDrvLeve1G3,  ePrivDrvSourceCcu,  "ccuEstopErr",                "急停按键被按下"},
    {ePrivDrvFaultCode_cardRderErr,                ePrivDrvLeve1G1,  ePrivDrvSourceCcu,  "cardRderErr",                "读卡器异常"},
    {ePrivDrvFaultCode_mainCabErr,                 ePrivDrvLeve1G3,  ePrivDrvSourceCcu,  "mainCabErr",                 "主柜功率分配故障"},
    {ePrivDrvFaultCode_meterCommErr,               ePrivDrvLeve1G3,  ePrivDrvSourceCcu,  "meterCommErr",               "电表通讯故障"},
    {ePrivDrvFaultCode_dcContactor1Err,            ePrivDrvLeve1G3,  ePrivDrvSourceCcu,  "dcContactor1Err",            "直流接触器K1故障"},
    {ePrivDrvFaultCode_dcContactor2Err,            ePrivDrvLeve1G3,  ePrivDrvSourceCcu,  "dcContactor2Err",            "直流接触器K2故障"},
    {ePrivDrvFaultCode_ccuApsErr,                  ePrivDrvLeve1G3,  ePrivDrvSourceCcu,  "ccuApsErr",                  "辅助电源故障"},
    {ePrivDrvFaultCode_ccuImdErr,                  ePrivDrvLeve1G2,  ePrivDrvSourceCcu,  "ccuImdErr",                  "绝缘检测故障"},
    {ePrivDrvFaultCode_ccuHighTempWarn,            ePrivDrvLeve1G1,  ePrivDrvSourceCcu,  "ccuHighTempWarn",            "充电接口高温降载"},
    {ePrivDrvFaultCode_gunHighTempErr,             ePrivDrvLeve1G2,  ePrivDrvSourceCcu,  "gunHighTempErr",             "充电接口过温保护"},
    {ePrivDrvFaultCode_ccuELockErr,                ePrivDrvLeve1G1,  ePrivDrvSourceCcu,  "ccuELockErr",                "充电接口电子锁故障"},
    {ePrivDrvFaultCode_chgConnectLossErr,          ePrivDrvLeve1G2,  ePrivDrvSourceCcu,  "chgConnectLossErr",          "控制导引连接连接故障"},
    {ePrivDrvFaultCode_chgGunVoltErr,              ePrivDrvLeve1G3,  ePrivDrvSourceCcu,  "chgGunVoltErr",              "枪口电压异常"},
    {ePrivDrvFaultCode_ccuPoStopErr,               ePrivDrvLeve1G1,  ePrivDrvSourceCcu,  "ccuPoStopErr",               "指令要求终止的订单号不存在"},
    {ePrivDrvFaultCode_preChgOvertime,             ePrivDrvLeve1G2,  ePrivDrvSourceCcu,  "preChgOvertime",             "预充完成超时"},
    {ePrivDrvFaultCode_outputOverVoltProtect,      ePrivDrvLeve1G2,  ePrivDrvSourceCcu,  "outputOverVoltProtect",      "枪线输出电压过压-保护"},
    {ePrivDrvFaultCode_outputOverCurrentProtect,   ePrivDrvLeve1G2,  ePrivDrvSourceCcu,  "outputOverCurrentProtect",   "枪线输出电流过流-保护"},
    {ePrivDrvFaultCode_ccuMeterConnectErr,         ePrivDrvLeve1G3,  ePrivDrvSourceCcu,  "ccuMeterConnectErr",         "电表采样端反接"},
    {ePrivDrvFaultCode_spdErr,                     ePrivDrvLeve1G3,  ePrivDrvSourceCcu,  "spdErr",                     "SPD防雷器异常"},
    {ePrivDrvFaultCode_outputReverse,              ePrivDrvLeve1G3,  ePrivDrvSourceCcu,  "outputReverse",              "枪线输出反接"},
    {ePrivDrvFaultCode_ccuTempLoErr,               ePrivDrvLeve1G3,  ePrivDrvSourceCcu,  "ccuTempLoErr",               "主板温度过低"},
    {ePrivDrvFaultCode_outputCurrErr,              ePrivDrvLeve1G2,  ePrivDrvSourceCcu,  "outputCurrErr",              "无输出电流"},
    {ePrivDrvFaultCode_ccuImdWarn,                 ePrivDrvLeve1G1,  ePrivDrvSourceCcu,  "ccuImdWarn",                 "绝缘电阻告警"},
    {ePrivDrvFaultCode_ccuLiqUnitComErr,           ePrivDrvLeve1G3,  ePrivDrvSourceCcu,  "ccuLiqUnitComErr",           "液冷机通信故障"},
    {ePrivDrvFaultCode_ccuLiqUnitPressHighErr,     ePrivDrvLeve1G3,  ePrivDrvSourceCcu,  "ccuLiqUnitPressHighErr",     "液冷机高压报警"},
    {ePrivDrvFaultCode_liquidPumpOverFlow,         ePrivDrvLeve1G1,  ePrivDrvSourceCcu,  "liquidPumpOverFlow",         "液冷机循环泵电流过流"},
    {ePrivDrvFaultCode_liquidCoolerOverCurrent,    ePrivDrvLeve1G1,  ePrivDrvSourceCcu,  "liquidCoolerOverCurrent",    "液冷机风机过流"},
    {ePrivDrvFaultCode_liquidCoolerOverTemp,       ePrivDrvLeve1G1,  ePrivDrvSourceCcu,  "liquidCoolerOverTemp",       "液冷机温度过高"},
    {ePrivDrvFaultCode_liquidPumpBlocked,          ePrivDrvLeve1G1,  ePrivDrvSourceCcu,  "liquidPumpBlocked",          "液冷机温度过高"},
    {ePrivDrvFaultCode_liquidCoolerLowFlow,        ePrivDrvLeve1G1,  ePrivDrvSourceCcu,  "liquidCoolerLowFlow",        "液冷机低流量报警"},
    {ePrivDrvFaultCode_liquidCoolerOverFlow,       ePrivDrvLeve1G1,  ePrivDrvSourceCcu,  "liquidCoolerOverFlow",       "液冷机高流量报警"},
    {ePrivDrvFaultCode_liquidCoolerUnderPressure,  ePrivDrvLeve1G1,  ePrivDrvSourceCcu,  "liquidCoolerUnderPressure",  "液冷机低压报警"},
    {ePrivDrvFaultCode_lowLiquidLevel,             ePrivDrvLeve1G1,  ePrivDrvSourceCcu,  "lowLiquidLevel",             "液冷机低液位报警"},
    {ePrivDrvFaultCode_ccuPcuComErr,               ePrivDrvLeve1G3,  ePrivDrvSourceCcu,  "ccuPcuComErr",               "主柜通信故障"},
    {ePrivDrvFaultCode_ccuOutsideVoltHiErr,        ePrivDrvLeve1G2,  ePrivDrvSourceCcu,  "ccuOutsideVoltHiErr",        "外侧电压超范围"},
    {ePrivDrvFaultCode_ccuXKmErr,                  ePrivDrvLeve1G2,  ePrivDrvSourceCcu,  "ccuXKmErr",                  "X接触器故障"},
    {ePrivDrvFaultCode_ccuYKmErr,                  ePrivDrvLeve1G2,  ePrivDrvSourceCcu,  "ccuYKmErr",                  "Y接触器故障"},
    {ePrivDrvFaultCode_ccuZKmErr,                  ePrivDrvLeve1G2,  ePrivDrvSourceCcu,  "ccuZKmErr",                  "Z接触器故障"},
    {ePrivDrvFaultCode_ccuDischargeErr,            ePrivDrvLeve1G2,  ePrivDrvSourceCcu,  "ccuDischargeErr",            "泄放超时故障"},
    {ePrivDrvFaultCode_ccuChgVoltHigher,           ePrivDrvLeve1G2,  ePrivDrvSourceCcu,  "ccuChgVoltHigher",           "充电电压高于车端允许电压"},
    {ePrivDrvFaultCode_ccuLockStopCharging,        ePrivDrvLeve1G2,  ePrivDrvSourceCcu,  "ccuLockStopCharging",        "枪锁停充故障"},
    {ePrivDrvFaultCode_pcuAcContactorErr,          ePrivDrvLeve1G3,  ePrivDrvSourcePcu,  "pcuAcContactorErr",          "主柜交流接触器故障"},
    {ePrivDrvFaultCode_pcuDoorOpenErr,             ePrivDrvLeve1G3,  ePrivDrvSourcePcu,  "pcuDoorOpenErr",             "主柜门禁打开"},
    {ePrivDrvFaultCode_pcuFanErr,                  ePrivDrvLeve1G1,  ePrivDrvSourcePcu,  "pcuFanErr",                  "主柜风扇异常"},
    {ePrivDrvFaultCode_pcuEsbErr,                  ePrivDrvLeve1G3,  ePrivDrvSourcePcu,  "pcuEsbErr",                  "主柜触发急停"},
    {ePrivDrvFaultCode_pcuTempOverErr,             ePrivDrvLeve1G1,  ePrivDrvSourcePcu,  "pcuTempOverErr",             "主柜内温度过高"},
    {ePrivDrvFaultCode_pcuSpdErr,                  ePrivDrvLeve1G3,  ePrivDrvSourcePcu,  "pcuSpdErr",                  "发生雷击"},
    {ePrivDrvFaultCode_pcuScuComErr,               ePrivDrvLeve1G1,  ePrivDrvSourcePcu,  "pcuScuComErr",               "SCU板通信中断"},
    {ePrivDrvFaultCode_pcuPmComErr,                ePrivDrvLeve1G1,  ePrivDrvSourcePcu,  "pcuPmComErr",                "模块通讯中断"},
    {ePrivDrvFaultCode_pcuTcuComErr,               ePrivDrvLeve1G1,  ePrivDrvSourcePcu,  "pcuTcuComErr",               "TCU通信超时"},
    {ePrivDrvFaultCode_pcuScuOtaErr,               ePrivDrvLeve1G1,  ePrivDrvSourcePcu,  "pcuScuOtaErr",               "SCU OTA失败"},
    {ePrivDrvFaultCode_pcuCcuComErr,               ePrivDrvLeve1G1,  ePrivDrvSourcePcu,  "pcuCcuComErr",               "CCU通讯失败"},
    {ePrivDrvFaultCode_scuRelayErr,                ePrivDrvLeve1G1,  ePrivDrvSourceScu,  "scuRelayErr",                "SCU继电器故障"},
    {ePrivDrvFaultCode_scuOverTempErr,             ePrivDrvLeve1G3,  ePrivDrvSourceScu,  "scuOverTempErr",             "触发过温"},
    {ePrivDrvFaultCode_tcuSelfTestErrt,            ePrivDrvLeve1G3,  ePrivDrvSourceTcu,  "tcuSelfTestErrt",            "TCU自检故障"},
    {ePrivDrvFaultCode_tcuChgStartTimeout,         ePrivDrvLeve1G2,  ePrivDrvSourceTcu,  "tcuChgStartTimeout",         "启动充电超时"},
    {ePrivDrvFaultCode_tcuChgStartResErr,          ePrivDrvLeve1G3,  ePrivDrvSourceTcu,  "tcuChgStartResErr",          "启动完成应答失败"},
    {ePrivDrvFaultCode_tcuCcuComErr,               ePrivDrvLeve1G3,  ePrivDrvSourceTcu,  "tcuCcuComErr",               "CCU 通信中断"},
    {ePrivDrvFaultCode_tcuPcuComErr,               ePrivDrvLeve1G3,  ePrivDrvSourceTcu,  "tcuPcuComErr",               "PCU 通信中断"},
    {ePrivDrvFaultCode_tcuOtaErr,                  ePrivDrvLeve1G1,  ePrivDrvSourceTcu,  "tcuOtaErr",                  "TCU OTA 失败"},
    {ePrivDrvFaultCode_ccuOtaErr,                  ePrivDrvLeve1G1,  ePrivDrvSourceTcu,  "ccuOtaErr",                  "CCU OTA 升级失败"},
    {ePrivDrvFaultCode_pcuOtaErr,                  ePrivDrvLeve1G1,  ePrivDrvSourceTcu,  "pcuOtaErr",                  "PCU OTA 升级失败"},
    {ePrivDrvFaultCode_ccuSetParmErr,              ePrivDrvLeve1G1,  ePrivDrvSourceTcu,  "ccuSetParmErr",              "设置CCU参数失败"},
    {ePrivDrvFaultCode_pcuSetParmErr,              ePrivDrvLeve1G1,  ePrivDrvSourceTcu,  "pcuSetParmErr",              "设置PCU参数失败"},
    {ePrivDrvFaultCode_ccuGetParmErr,              ePrivDrvLeve1G1,  ePrivDrvSourceTcu,  "ccuGetParmErr",              "获取CCU 参数失败"},
    {ePrivDrvFaultCode_pcuGetParmErr,              ePrivDrvLeve1G1,  ePrivDrvSourceTcu,  "pcuGetParmErr",              "获取PCU参数失败"},
    {ePrivDrvFaultCode_ccuGetLogErr,               ePrivDrvLeve1G1,  ePrivDrvSourceTcu,  "ccuGetLogErr",               "获取CCU运行日志失败"},
    {ePrivDrvFaultCode_pcuGetLogErr,               ePrivDrvLeve1G1,  ePrivDrvSourceTcu,  "pcuGetLogErr",               "获取PCU运行日志失败"},
    {ePrivDrvFaultCode_tcuLoginServerErr,          ePrivDrvLeve1G3,  ePrivDrvSourceTcu,  "tcuLoginServerErr",          "登录平台失败"},
    {ePrivDrvFaultCode_tcuOffline,                 ePrivDrvLeve1G3,  ePrivDrvSourceTcu,  "tcuOffline",                 "服务器通信中断"},
    {ePrivDrvFaultCode_comModuleErr,               ePrivDrvLeve1G3,  ePrivDrvSourceTcu,  "comModuleErr",               "通信中断故障"},
    {ePrivDrvFaultCode_chargeAuthFail,             ePrivDrvLeve1G1,  ePrivDrvSourceTcu,  "chargeAuthFail",             "账号鉴权失败"},
    {ePrivDrvFaultCode_screenLinkFail,             ePrivDrvLeve1G1,  ePrivDrvSourceTcu,  "screenLinkFail",             "屏幕连接异常"},
};
#define cPrivDrvFaultNum (sizeof(stPrivDrvFaultData)/sizeof(stPrivDrvFaultData[0]))





/**
 * @brief 获取故障等级
 */
bool sPrivDrvGetFaultLevel(ePrivDrvFaultCode_t eFaultCode, ePrivDrvLeve_t *peFaultLevel)
{
    u32 u32Index;
    for(u32Index = 0; u32Index < cPrivDrvFaultNum; u32Index++)
    {
        if(stPrivDrvFaultData[u32Index].eCode == eFaultCode)
        {
            *peFaultLevel = stPrivDrvFaultData[u32Index].eLeve;
            return true;
        }
    }
    return false;
}




/**
 * @brief 获取OCPPstr
 */
char* sGetOcppStr(ePrivDrvFaultCode_t eTarget) 
{
    int left  = 0;
    int right = cPrivDrvFaultNum - 1;
    int mid   = 0;

    while (left <= right) 
    {
        mid = left + (right - left) / 2;

        // 检查目标值是否在中间位置
        if (stPrivDrvFaultData[mid].eCode == eTarget) {
            return stPrivDrvFaultData[mid].pOcppCode;
        }
        // 如果目标值大于中间值，忽略左半部分
        else if (stPrivDrvFaultData[mid].eCode < eTarget) {
            left = mid + 1;
        }
        // 如果目标值小于中间值，忽略右半部分
        else {
            right = mid - 1;
        }
    }

    // 如果数组中没有目标值,返回false
    return NULL;
}




/**
 * @brief 获取GBstr
 */
char* sGetGbStr(ePrivDrvFaultCode_t eTarget) 
{
    int left  = 0;
    int right = cPrivDrvFaultNum - 1;
    int mid   = 0;

    while (left <= right) 
    {
        mid = left + (right - left) / 2;

        // 检查目标值是否在中间位置
        if (stPrivDrvFaultData[mid].eCode == eTarget) {
            return stPrivDrvFaultData[mid].pName;
        }
        // 如果目标值大于中间值，忽略左半部分
        else if (stPrivDrvFaultData[mid].eCode < eTarget) {
            left = mid + 1;
        }
        // 如果目标值小于中间值，忽略右半部分
        else {
            right = mid - 1;
        }
    }

    // 如果数组中没有目标值,返回false
    return NULL;
}




/**
 * @brief 获取故障表版本号
 */
const char* sPrivDrvGetVersion(void)
{
    return cVersion;
}