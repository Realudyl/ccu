/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   cooling_app.h
* Description                           :   液冷机通讯 用户程序实现
* Version                               :   V1.0.0
* Author                                :   haisheng.dang@en-plus.com.cn
* Creat Date                            :   2024-06-18
* notice                                :   具体型号为:英特尼迪
*                                           使用modbus-rtu协议
****************************************************************************************************/
#ifndef _cooling_app_h_
#define _cooling_app_h_

#include "en_common.h"
#include "en_log.h"
#include "en_mem.h"
#include "cooling_app_shell.h"

#include "usart_drv_trans.h"





//液冷机串口
#ifndef cSdkCoolingComUartNum
#define cCoolingComUartNum              (eUsartNum6)
#else
#define cCoolingComUartNum              (cSdkCoolingComUartNum)
#endif


//液冷机串口波特率
#ifndef cSdkCoolingComBaudrate
#define cCoolingComBaudrate             (38400)
#else
#define cCoolingComBaudrate             (cSdkCoolingComBaudrate)
#endif



//液冷机设备地址
#define cCoolingDevAddr                 0x30


//收发缓存区大小
#define cCoolingBuffSize                256



//通讯故障判断最大计数
#define cCoolingMaxErrCnt               10






























//读写功能码定义
typedef enum
{
    eFunCodeReadBits                    = 0x01,                                 //读多个bit
    eFunCodeReadRegs                    = 0x03,                                 //读多个寄存器
    eFunCodeWriteReg                    = 0x06,                                 //写单个寄存器（鉴于寄存器地址不连续，因此只支持一个个写）
    eFunCodeWriteBits                   = 0x0F,                                 //写多个bit
}eCoolingAppFunCode_t;









//遥控生效设置值
typedef enum
{
    eRemoteCtrlStop                     = 0x0000,                               //停止
    eRemoteCtrlStart                    = 0xFF00,                               //启动
}eCoolingAppCtrlParam_t;












//英特尼迪协议 读遥信寄存器地址 定义，功能码：0x01
typedef enum
{
    eSigRegHighPress                    = 0x0000,                               //高压报警状态
    eSigRegLowPress                     = 0x0001,                               //低压报警状态
    eSigRegGunOvTemp                    = 0x0002,                               //枪头超温报警状态
    eSigRegFan01Err                     = 0x0003,                               //风机1出错状态
    eSigRegFan02Err                     = 0x0004,                               //风机2出错状态
    eSigRegHighLiquid                   = 0x0005,                               //高液位报警状态
    eSigRegLowLiquid                    = 0x0006,                               //低液位报警状态
    eSigRegPumpsOvCurr                  = 0x0007,                               //循环泵过流报警状态
    eSigRegPumpsUnVolt                  = 0x0008,                               //循环泵欠压报警状态
    eSigRegPumpsOvVolt                  = 0x0009,                               //循环泵过压报警状态
    eSigRegCoolingOvTemp                = 0x000A,                               //液冷机过温报警状态
    eSigRegPumpsBlocked                 = 0x000B,                               //循环泵堵转报警状态
    eSigRegStandby                      = 0x000C,                               //预留状态
    eSigRegFlowSlow                     = 0x000D,                               //低流量报警状态
    eSigRegFlowFast                     = 0x000E,                               //高流量报警状态
    eSigRegFanOvCurr                    = 0x000F,                               //风机过流状态
    
    eSigRegMax
}__attribute__((packed)) eEtyReadRemoteSignalReg_t;








//英特尼迪协议 读遥测寄存器地址 定义，功能码：0x03
typedef enum
{
    eTelRegSysFlowRate                  = 0x0000,                               //系统流量，单位：0.01L/min
    eTelRegSysPressure                  = 0x0002,                               //系统压力，单位：0.01bar
    eTelRegInLiquidTemp                 = 0x0005,                               //回液温度，单位：0.01°C，偏移：-50°C
    eTelRegOutLiquidTemp                = 0x0006,                               //供液温度，单位：0.01°C，偏移：-50°C
    eTelRegPumpsSpeed                   = 0x000B,                               //循环泵转速，单位：1
    eTelRegFanSpeed                     = 0x000C,                               //风机转速，单位：1
    eTelRegVersion                      = 0x000F,                               //软件版本号，高8bit大版本，低8bit小版本
    eTelRegRunning                      = 0x1501,                               //运行状态
    eTelRegRunMode                      = 0x1703,                               //运行模式，0：自动运行 1：手动调试 2：枪线加液 3：枪线排液 4：储液罐加液 5：储液罐排液
    
    eTelRegMax
}__attribute__((packed)) eEtyReadTelemetryReg_t;








//英特尼迪协议 写遥控寄存器地址 定义，功能码：0x0F
typedef enum
{
    eCtrlRegResumeFactory               = 0x1500,                               //恢复出厂设置：0xFF00，其余值无效
    eCtrlRegPowerOnOff                  = 0x1501,                               //启停控制：0xFF00-启动 0x0000-停止
    
    eCtrlRegMax
}__attribute__((packed)) eEtyWriteCtrlReg_t;







//英特尼迪协议 写遥调寄存器地址 定义，功能码：0x06/0x10
typedef enum
{
    eAdjustRegSpeed                     = 0x1700,                               //速度（待定，暂时不建议设置）
    eAdjustRegDevAddr                   = 0x1701,                               //PLC从站地址，默认0x30
    eAdjustRegBaudrate                  = 0x1702,                               //波特率设定，0-4800 1-9600 2-19200 3-38400 4-57600 5-115200
    eAdjustRegRunMode                   = 0x1703,                               //运行模式，0：自动运行 1：手动调试 2：枪线加液 3：枪线排液 4：储液罐加液 5：储液罐排液
    
    eAdjustRegGunTemp                   = 0x1705,                               //枪头温度，单位：0.1°C，偏移量：-50°C
    eAdjustRegLv1Temp                   = 0x1706,                               //1档温度，低于此温度执行1档转速
    eAdjustRegLv1PumpsSpeed             = 0x1707,                               //1档循环泵转速，精度%1，范围0%~100%
    eAdjustRegLv1FanSpeed               = 0x1708,                               //1档风机转速，精度%1，范围：0%~100%
    
    eAdjustRegLv2Temp                   = 0x1709,                               //2档温度，低于此温度执行1档转速
    eAdjustRegLv2PumpsSpeed             = 0x170A,                               //2档循环泵转速，精度%1，范围0%~100%
    eAdjustRegLv2FanSpeed               = 0x170B,                               //2档风机转速，精度%1，范围：0%~100%
    
    eAdjustRegLv3Temp                   = 0x170C,                               //3档温度，低于此温度执行1档转速
    eAdjustRegLv3PumpsSpeed             = 0x170D,                               //3档循环泵转速，精度%1，范围0%~100%
    eAdjustRegLv3FanSpeed               = 0x170E,                               //3档风机转速，精度%1，范围：0%~100%
    
    eAdjustRegLv4Temp                   = 0x170F,                               //4档温度，低于此温度执行1档转速
    eAdjustRegLv4PumpsSpeed             = 0x1710,                               //4档循环泵转速，精度%1，范围0%~100%
    eAdjustRegLv4FanSpeed               = 0x1711,                               //4档风机转速，精度%1，范围：0%~100%
    
    eAdjustRegLv5Temp                   = 0x1712,                               //5档温度，低于此温度执行1档转速
    eAdjustRegLv5PumpsSpeed             = 0x1713,                               //5档循环泵转速，精度%1，范围0%~100%
    eAdjustRegLv5FanSpeed               = 0x1714,                               //5档风机转速，精度%1，范围：0%~100%
    eAdjustRegHighTempAlarm             = 0x1715,                               //高温报警温度值，单位：0.1°C，偏移量：-50°C
    
    eAdjustRegSlowPressure              = 0x1718,                               //低压报警值，单位：0.01MPa
    eAdjustRegHighPressure              = 0x1719,                               //高压报警值，单位：0.01MPa
    
}__attribute__((packed)) eEtyWriteAdjustReg_t;























//液冷机通讯状态
typedef enum
{
    eCommStateFailed                    = 0,                                    //通讯故障
    eCommStateNormal                    = 1,                                    //通讯正常
}__attribute__((packed)) eCoolingAppCommState_t;









//液冷机运行模式
typedef enum
{
    eRunModeAuto                        = 0,                                    //自动运行
    eRunModeManual                      = 1,                                    //手动调试
    eRunModeAddLiquid                   = 2,                                    //枪线加液
    eRunModeLessLiquid                  = 3,                                    //枪线排液
    eRunModeStorageAdd                  = 4,                                    //储存罐加液（不建议设置）
    eRunModeStorageLess                 = 5,                                    //储存罐排液（不建议设置）
}eCoolingAppRunMode_t;











//液冷机故障定义
typedef struct
{
    u16                                 bPressHigh          : 1;                //bit0 高压报警
    u16                                 bPressLow           : 1;                //bit1 低压报警
    u16                                 bGunOvTemp          : 1;                //bit2 枪头超温报警
    u16                                 bFan01Err           : 1;                //bit3 风机1出错
    u16                                 bFan02Err           : 1;                //bit4 风机2出错
    u16                                 bLiquidHigh         : 1;                //bit5 高液位报警
    u16                                 bLiquidLow          : 1;                //bit6 低液位报警
    u16                                 bIpumpHigh          : 1;                //bit7 循环泵过流报警
    
    u16                                 bPpumpLow           : 1;                //bit8 循环泵欠压报警
    u16                                 bPpumpHigh          : 1;                //bit9 循环泵过压报警
    u16                                 bTempHigh           : 1;                //bit10液冷机过温报警
    u16                                 bPpumpStall         : 1;                //bit11循环泵堵转报警
    u16                                 bStandby            : 1;                //bit12预留
    u16                                 bFlowSlow           : 1;                //bit13低流量报警
    u16                                 bFlowFast           : 1;                //bit14高流量报警
    u16                                 bIfanHigh           : 1;                //bit15风机过流
}stCoolingAppErrCode_t;



typedef union
{
    stCoolingAppErrCode_t               stErrCode;
    u16                                 u16ErrCode;
}unCoolingAppErrCode_t;























//当前液冷机的一些实时信息
typedef struct
{
    f32                                 f32SysFlowRate;                         //系统流量，单位：1L/min
    f32                                 f32SysPressure;                         //系统压力，单位：1bar
    f32                                 f32InTemper;                            //回液温度，单位：1°C，
    f32                                 f32OutTemper;                           //供液温度，单位：1°C，
    u16                                 u16PumpsSpeed;                          //循环泵转速
    u16                                 u16FanSpeed;                            //风机转速，%
    u16                                 u16Version;                             //软件版本号，高8bit大版本，低8bit小版本
    u16                                 u16RunState;                            //运行状态
    eCoolingAppRunMode_t                eRunMode;                               //运行模式
}stCoolingAppRealInfo_t;










//读取液冷机的配置参数
typedef struct
{
    u16                                 u16Speed;                               //速度，通讯协议没有明确表达这个字段的含义，建议不设置
    u16                                 u16DevAddr;                             //设备地址，默认0x30
    u16                                 u16BaudType;                            //设备波特率，0-4800 1-9600 2-19200 3-38400 4-57600 5-115200
    f32                                 f32GunTemp;                             //枪头报警温度，单位：0.1，偏移量：-50
    
    f32                                 f32Lv1Temp;                             //1档温度
    f32                                 f32Lv2Temp;                             //2档温度
    f32                                 f32Lv3Temp;                             //3档温度
    f32                                 f32Lv4Temp;                             //4档温度
    f32                                 f32Lv5Temp;                             //5档温度
    u16                                 u16Lv1PumpSpeed;                        //1档循环泵转速
    u16                                 u16Lv2PumpSpeed;                        //2档循环泵转速
    u16                                 u16Lv3PumpSpeed;                        //3档循环泵转速
    u16                                 u16Lv4PumpSpeed;                        //4档循环泵转速
    u16                                 u16Lv5PumpSpeed;                        //5档循环泵转速
    u16                                 u16Lv1FanSpeed;                         //1档风机转速
    u16                                 u16Lv2FanSpeed;                         //2档风机转速
    u16                                 u16Lv3FanSpeed;                         //3档风机转速
    u16                                 u16Lv4FanSpeed;                         //4档风机转速
    u16                                 u16Lv5FanSpeed;                         //5档风机转速
    
    f32                                 f32HighTempAlarm;                       //高温报警值（进液位置温度）
    f32                                 f32SlowPressure;                        //低压报警值
    f32                                 f32HighPressure;                        //高压报警值
}stCoolingAppCfgParam_t;













//因寄存器地址不连续，不能一次写多个寄存器，故采用以下方式提供接口给外部设置
typedef struct
{
    bool                                bNeedWrite;                             //是否需要立即写入
    eEtyWriteAdjustReg_t                eReg;                                   //待设置的寄存器
    u16                                 u16RegValue;                            //待设置的数值
}stCoolingAppSetParam;














//液冷机信息
typedef struct
{
    eCoolingAppCtrlParam_t              eStartup;                               //启动液冷机
    eCoolingAppCtrlParam_t              eResumeFact;                            //恢复出厂设置
    u16                                 u16Addr;                                //设备通讯地址
    u16                                 u16CommFailCount;                       //通讯失败累计次数
    eCoolingAppCommState_t              eCommSt;                                //通讯状态
    unCoolingAppErrCode_t               unErrCode;                              //当前故障
    
    stCoolingAppRealInfo_t              stRealInfo;                             //从设备获取的实时信息
    stCoolingAppCfgParam_t              stCfgParam;                             //从设备获取的配置信息（暂时不读了，液冷机协议有毒，回复一遍之后再也不回了！！！）
    stCoolingAppSetParam                stSetParam;                             //设置指定系统寄存器的值
    
    u16                                 u16Index;                               //解析索引
}stCoolingAppCache_t;














extern bool sCoolingAppInit(void);

extern eCoolingAppCommState_t sCoolingAppGetCommSt(void);
extern stCoolingAppErrCode_t sCoolingAppGetErrCode(void);
extern bool sCoolingAppGetInfo(stCoolingAppRealInfo_t **pInfo);

extern bool sCoolingAppSetPowerOnOff(eCoolingAppCtrlParam_t eCtrl);
extern bool sCoolingAppSetResumeFactory(eCoolingAppCtrlParam_t eCtrl);

extern bool sCoolingAppSetSysParam(eEtyWriteAdjustReg_t eReg, u16 u16RegValue);

#endif

































