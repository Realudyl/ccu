/**
 * @file private_drv_fault_data_def.h
 * @author warn_infor.exe
 * @brief 故障组件数据定义
 * @version 0.0.19
 * @date 2025-02-19 10:51
 *
 * @copyright Shenzhen EN Plus Technologies Co., Ltd. 2015-2024. All rights reserved
 *
 */

#pragma once
#include "en_common.h"




typedef enum
{
    ePrivDrvFaultCode_devFault                   = 5100u,                       //模块故障
    ePrivDrvFaultCode_devProtect                 = 5101u,                       //模块告警
    ePrivDrvFaultCode_sciComFault                = 5103u,                       //模块内部 SCI 通信故障
    ePrivDrvFaultCode_dcdcOverVoltage            = 5107u,                       //DCDC 过压
    ePrivDrvFaultCode_pfcVoltageErr              = 5108u,                       //PFC 电压异常
    ePrivDrvFaultCode_acInputOverVoltage         = 5109u,                       //交流过压
    ePrivDrvFaultCode_acUndervolt                = 5114u,                       //交流欠压
    ePrivDrvFaultCode_canCommErr                 = 5116u,                       //CAN 通信故障
    ePrivDrvFaultCode_moduleCurrImbalance        = 5117u,                       //模块不均流
    ePrivDrvFaultCode_powerStatus                = 5122u,                       //DCDC 关机
    ePrivDrvFaultCode_powerLimitation            = 5123u,                       //模块限功率
    ePrivDrvFaultCode_highTempPowerLimit         = 5124u,                       //温度限功率
    ePrivDrvFaultCode_acPowerLimit               = 5125u,                       //交流限功率
    ePrivDrvFaultCode_fanFault                   = 5127u,                       //风扇故障
    ePrivDrvFaultCode_dcShort                    = 5128u,                       //DCDC 短路
    ePrivDrvFaultCode_dcOverTemp                 = 5130u,                       //DCDC 过温
    ePrivDrvFaultCode_bmsCommErr                 = 6000u,                       //BMS通讯异常
    ePrivDrvFaultCode_bcpTimeout                 = 6001u,                       //BCP充电参数配置报文超时
    ePrivDrvFaultCode_broAaTimeout               = 6002u,                       //BRO充电准备就绪报文超时
    ePrivDrvFaultCode_bcsTimeout                 = 6003u,                       //BCS电池充电状态报文超时
    ePrivDrvFaultCode_bclTimeout                 = 6004u,                       //BCL电池充电需求报文超时
    ePrivDrvFaultCode_bstTimeout                 = 6005u,                       //BST中止充电报文超时
    ePrivDrvFaultCode_bsdTimeout                 = 6006u,                       //BSD充电统计数据报文超时
    ePrivDrvFaultCode_bsmTimeout                 = 6007u,                       //BSM动力蓄电池状态报文超时
    ePrivDrvFaultCode_bhmMismErr                 = 6009u,                       //BHM桩的输出能力不匹配
    ePrivDrvFaultCode_brmTimeout                 = 6010u,                       //BRM车辆辨识报文超时
    ePrivDrvFaultCode_bmsImdErr                  = 6013u,                       //BMS绝缘故障
    ePrivDrvFaultCode_bmsOverTemp                = 6014u,                       //BMS元件过温
    ePrivDrvFaultCode_bmsOverVolt                = 6015u,                       //BMS电压过高
    ePrivDrvFaultCode_bmsPChVoltErr              = 6016u,                       //BMS预充电压不匹配
    ePrivDrvFaultCode_batVoltHighErr             = 6018u,                       //单体动力蓄电池电压过高
    ePrivDrvFaultCode_batVoltLowErr              = 6019u,                       //单体动力蓄电池电压过低
    ePrivDrvFaultCode_batSocHighErr              = 6020u,                       //整车动力蓄电池SOC过高
    ePrivDrvFaultCode_batSocLowErr               = 6021u,                       //整车动力蓄电池SOC过低
    ePrivDrvFaultCode_batOverCurr                = 6022u,                       //动力蓄电池充电过流
    ePrivDrvFaultCode_batOverTemp                = 6023u,                       //动力蓄电池温度过高
    ePrivDrvFaultCode_batImdErr                  = 6024u,                       //动力蓄电池绝缘故障
    ePrivDrvFaultCode_batConnErr                 = 6025u,                       //动力蓄电池连接器故障
    ePrivDrvFaultCode_batUnderVoltErr            = 6027u,                       //电池欠压
    ePrivDrvFaultCode_batOverVolt                = 6028u,                       //电池过压
    ePrivDrvFaultCode_croTimeout                 = 6029u,                       //CRO充电机输出就绪超时
    ePrivDrvFaultCode_ccsTimeout                 = 6030u,                       //CCS充电机状态报文超时
    ePrivDrvFaultCode_csTimeout                  = 6031u,                       //CST充电机终止充电报文超时 
    ePrivDrvFaultCode_csdTimeout                 = 6032u,                       //CSD充电统计数据报文超时
    ePrivDrvFaultCode_bstTp2Volt                 = 6037u,                       //BST检测点2电压检测故障
    ePrivDrvFaultCode_bclVoltHigh                = 6038u,                       //BCL需求电压过高
    ePrivDrvFaultCode_bclCurrHigh                = 6039u,                       //BCL需求电流过高
    ePrivDrvFaultCode_ccuSelfTestErr             = 7000u,                       //设备自检失败
    ePrivDrvFaultCode_dispenserDoorErr           = 7002u,                       //终端柜门被打开
    ePrivDrvFaultCode_ccuEstopErr                = 7003u,                       //急停按键被按下
    ePrivDrvFaultCode_cardRderErr                = 7007u,                       //读卡器异常
    ePrivDrvFaultCode_mainCabErr                 = 7009u,                       //主柜功率分配故障
    ePrivDrvFaultCode_meterCommErr               = 7013u,                       //电表通讯故障
    ePrivDrvFaultCode_dcContactor1Err            = 7015u,                       //直流接触器K1故障
    ePrivDrvFaultCode_dcContactor2Err            = 7016u,                       //直流接触器K2故障
    ePrivDrvFaultCode_ccuApsErr                  = 7019u,                       //辅助电源故障
    ePrivDrvFaultCode_ccuImdErr                  = 7020u,                       //绝缘检测故障
    ePrivDrvFaultCode_ccuHighTempWarn            = 7022u,                       //充电接口高温降载
    ePrivDrvFaultCode_gunHighTempErr             = 7023u,                       //充电接口过温保护
    ePrivDrvFaultCode_ccuELockErr                = 7024u,                       //充电接口电子锁故障
    ePrivDrvFaultCode_chgConnectLossErr          = 7027u,                       //控制导引连接连接故障
    ePrivDrvFaultCode_chgGunVoltErr              = 7028u,                       //枪口电压异常
    ePrivDrvFaultCode_ccuPoStopErr               = 7033u,                       //指令要求终止的订单号不存在
    ePrivDrvFaultCode_preChgOvertime             = 7041u,                       //预充完成超时
    ePrivDrvFaultCode_outputOverVoltProtect      = 7048u,                       //枪线输出电压过压-保护
    ePrivDrvFaultCode_outputOverCurrentProtect   = 7049u,                       //枪线输出电流过流-保护
    ePrivDrvFaultCode_ccuMeterConnectErr         = 7050u,                       //电表采样端反接
    ePrivDrvFaultCode_spdErr                     = 7054u,                       //SPD防雷器异常
    ePrivDrvFaultCode_outputReverse              = 7057u,                       //枪线输出反接
    ePrivDrvFaultCode_ccuTempLoErr               = 7058u,                       //主板温度过低
    ePrivDrvFaultCode_outputCurrErr              = 7059u,                       //无输出电流
    ePrivDrvFaultCode_ccuImdWarn                 = 7061u,                       //绝缘电阻告警
    ePrivDrvFaultCode_ccuLiqUnitComErr           = 7062u,                       //液冷机通信故障
    ePrivDrvFaultCode_ccuLiqUnitPressHighErr     = 7063u,                       //液冷机高压报警
    ePrivDrvFaultCode_liquidPumpOverFlow         = 7064u,                       //液冷机循环泵电流过流
    ePrivDrvFaultCode_liquidCoolerOverCurrent    = 7065u,                       //液冷机风机过流
    ePrivDrvFaultCode_liquidCoolerOverTemp       = 7066u,                       //液冷机温度过高
    ePrivDrvFaultCode_liquidPumpBlocked          = 7067u,                       //液冷机温度过高
    ePrivDrvFaultCode_liquidCoolerLowFlow        = 7068u,                       //液冷机低流量报警
    ePrivDrvFaultCode_liquidCoolerOverFlow       = 7069u,                       //液冷机高流量报警
    ePrivDrvFaultCode_liquidCoolerUnderPressure  = 7070u,                       //液冷机低压报警
    ePrivDrvFaultCode_lowLiquidLevel             = 7071u,                       //液冷机低液位报警
    ePrivDrvFaultCode_ccuPcuComErr               = 7072u,                       //主柜通信故障
    ePrivDrvFaultCode_ccuOutsideVoltHiErr        = 7073u,                       //外侧电压超范围
    ePrivDrvFaultCode_ccuXKmErr                  = 7075u,                       //X接触器故障
    ePrivDrvFaultCode_ccuYKmErr                  = 7076u,                       //Y接触器故障
    ePrivDrvFaultCode_ccuZKmErr                  = 7077u,                       //Z接触器故障
    ePrivDrvFaultCode_ccuDischargeErr            = 7078u,                       //泄放超时故障
    ePrivDrvFaultCode_ccuChgVoltHigher           = 7079u,                       //充电电压高于车端允许电压
    ePrivDrvFaultCode_ccuLockStopCharging        = 7080u,                       //枪锁停充故障
    ePrivDrvFaultCode_pcuAcContactorErr          = 7600u,                       //主柜交流接触器故障
    ePrivDrvFaultCode_pcuDoorOpenErr             = 7601u,                       //主柜门禁打开
    ePrivDrvFaultCode_pcuFanErr                  = 7602u,                       //主柜风扇异常
    ePrivDrvFaultCode_pcuEsbErr                  = 7603u,                       //主柜触发急停
    ePrivDrvFaultCode_pcuTempOverErr             = 7606u,                       //主柜内温度过高
    ePrivDrvFaultCode_pcuSpdErr                  = 7608u,                       //发生雷击
    ePrivDrvFaultCode_pcuScuComErr               = 7609u,                       //SCU板通信中断
    ePrivDrvFaultCode_pcuPmComErr                = 7610u,                       //模块通讯中断
    ePrivDrvFaultCode_pcuTcuComErr               = 7613u,                       //TCU通信超时
    ePrivDrvFaultCode_pcuScuOtaErr               = 7614u,                       //SCU OTA失败
    ePrivDrvFaultCode_pcuCcuComErr               = 7615u,                       //CCU通讯失败
    ePrivDrvFaultCode_scuRelayErr                = 7800u,                       //SCU继电器故障
    ePrivDrvFaultCode_scuOverTempErr             = 7801u,                       //触发过温
    ePrivDrvFaultCode_tcuSelfTestErrt            = 8000u,                       //TCU自检故障
    ePrivDrvFaultCode_tcuChgStartTimeout         = 8001u,                       //启动充电超时
    ePrivDrvFaultCode_tcuChgStartResErr          = 8002u,                       //启动完成应答失败
    ePrivDrvFaultCode_tcuCcuComErr               = 8003u,                       //CCU 通信中断
    ePrivDrvFaultCode_tcuPcuComErr               = 8004u,                       //PCU 通信中断
    ePrivDrvFaultCode_tcuOtaErr                  = 8005u,                       //TCU OTA 失败
    ePrivDrvFaultCode_ccuOtaErr                  = 8006u,                       //CCU OTA 升级失败
    ePrivDrvFaultCode_pcuOtaErr                  = 8007u,                       //PCU OTA 升级失败
    ePrivDrvFaultCode_ccuSetParmErr              = 8008u,                       //设置CCU参数失败
    ePrivDrvFaultCode_pcuSetParmErr              = 8009u,                       //设置PCU参数失败
    ePrivDrvFaultCode_ccuGetParmErr              = 8010u,                       //获取CCU 参数失败
    ePrivDrvFaultCode_pcuGetParmErr              = 8011u,                       //获取PCU参数失败
    ePrivDrvFaultCode_ccuGetLogErr               = 8012u,                       //获取CCU运行日志失败
    ePrivDrvFaultCode_pcuGetLogErr               = 8013u,                       //获取PCU运行日志失败
    ePrivDrvFaultCode_tcuLoginServerErr          = 8014u,                       //登录平台失败
    ePrivDrvFaultCode_tcuOffline                 = 8015u,                       //服务器通信中断
    ePrivDrvFaultCode_comModuleErr               = 8016u,                       //通信中断故障
    ePrivDrvFaultCode_chargeAuthFail             = 8017u,                       //账号鉴权失败
    ePrivDrvFaultCode_screenLinkFail             = 8018u,                       //屏幕连接异常
    
}__attribute__((packed)) ePrivDrvFaultCode_t;




typedef enum
{

    ePrivDrvLeve1G1                      = 0,                                   //告警
    ePrivDrvLeve1G2                      = 1,                                   //故障，拔枪恢复
    ePrivDrvLeve1G3                      = 2,                                   //故障，自动恢复
    ePrivDrvLeve1G4                      = 3,                                   //故障，重启恢复
    ePrivDrvLevelMax,
}ePrivDrvLeve_t;




typedef enum
{
    ePrivDrvSourceCcu                    = 0,
    ePrivDrvSourceBms                    = 1,
    ePrivDrvSourceTcu                    = 2,
    ePrivDrvSourcePcu                    = 3,
    ePrivDrvSourceScu                    = 4,
    ePrivDrvSourceMod                    = 5,

    ePrivDrvSourceMax,
}ePrivDrvSource_t;




typedef struct
{
    ePrivDrvFaultCode_t                  eCode;
    ePrivDrvLeve_t                       eLeve;
    ePrivDrvSource_t                     eWarnSource;
    char*                                pOcppCode;
    char*                                pName;
}stPrivDrvFaultData_t;



extern bool sPrivDrvGetFaultLevel(ePrivDrvFaultCode_t eFaultCode, ePrivDrvLeve_t *peFaultLevel);
extern const char* sPrivDrvGetVersion(void);
extern char* sGetOcppStr(ePrivDrvFaultCode_t eTarget);
extern char* sGetGbStr(ePrivDrvFaultCode_t eTarget);
