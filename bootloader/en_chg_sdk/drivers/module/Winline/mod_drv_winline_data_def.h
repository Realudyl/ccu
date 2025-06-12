/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   mod_drv_winline_data_def.h
* Description                           :   永联科技 NXR 系列充电模块CAN通讯驱动实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-04-02
* notice                                :   基于  <NXR 系列充电模块通讯协议 V1.48>  设计
****************************************************************************************************/
#ifndef _mod_drv_winline_data_def_h_
#define _mod_drv_winline_data_def_h_
#include "en_common.h"
#include "en_log.h"









//永联模块modbus协议 基本参数定义
#define cModDrvWinlinePayloadLen        (4)                                     //payload长度






//永联模块modbus协议 功能码 定义
typedef enum
{
    eModDrvWinlineFunSet                = 0x03,                                 //设置模块参数
    eModDrvWinlineFunGet                = 0x10,                                 //读取模块数据
    
    eModDrvWinlineFunFloat              = 0x41,                                 //返回的是浮点数---模块应答读取命令时：返回数据的类型
    eModDrvWinlineFunInt                = 0x42,                                 //返回的是整型
    
    eModDrvWinlineFunMax
}__attribute__((packed)) eModDrvWinlineFun_t;






//永联模块modbus协议 错误代码 定义
typedef enum
{
    eModDrvWinlineErrRsvd               = 0x00,                                 //保留
    eModDrvWinlineErrNone               = 0xF0,                                 //正常---其他值就是故障帧 应丢弃
    
    eModDrvWinlineErrMax
}__attribute__((packed)) eModDrvWinlineErr_t;






//永联模块modbus协议 寄存器地址 定义
typedef enum
{
    eModDrvWinlineRegGetUdcOut          = 0x0001,                               //只读, 浮点数, 模块电压
    eModDrvWinlineRegGetIdcOut          = 0x0002,                               //只读, 浮点数, 模块电流
    eModDrvWinlineRegGetIdcOutLimit     = 0x0003,                               //只读, 浮点数, 模块电流限流点
    eModDrvWinlineRegGetTdc             = 0x0004,                               //只读, 浮点数, 模块DC 板温度
    eModDrvWinlineRegGetUacUdcIn        = 0x0005,                               //只读, 浮点数, 模块输入相电压（直流输入电压）
    eModDrvWinlineRegGetUbusP           = 0x0008,                               //只读, 浮点数, PFC0 电压（正半母线）
    eModDrvWinlineRegGetUbusN           = 0x000A,                               //只读, 浮点数, PFC1 电压（负半母线）
    eModDrvWinlineRegGetTenv            = 0x000B,                               //只读, 浮点数, 面板（环境）温度
    eModDrvWinlineRegGetUa              = 0x000C,                               //只读, 浮点数, 交流 A 相电压
    eModDrvWinlineRegGetUb              = 0x000D,                               //只读, 浮点数, 交流 B 相电压
    eModDrvWinlineRegGetUc              = 0x000E,                               //只读, 浮点数, 交流 C 相电压
    eModDrvWinlineRegGetTpfc            = 0x0010,                               //只读, 浮点数, PFC 板温度
    eModDrvWinlineRegGetPdcOutRated     = 0x0011,                               //只读, 浮点数, 额定输出功率
    eModDrvWinlineRegGetIdcOutRated     = 0x0012,                               //只读, 浮点数, 额定输出电流
    eModDrvWinlineRegSetAltitude        = 0x0017,                               //只写, 整  型, 工作海拔值, 单位：米；范围为 1000~5000，海拔低于 1000 米不需要设置（不降额），高于 5000 米设置 5000，设置值掉电保存
    eModDrvWinlineRegSetIdcOut          = 0x001B,                               //只写, 整  型, 输出电流值, 设 置 值 为 输 出 电 流 *1024 倍 ， 例 如 ：10240=10A*1024
    eModDrvWinlineRegSetGroup           = 0x001E,                               //只写, 整  型, 组号, byte7 低三位（范围 0~7） 其余 byte4~byte6 和 byte7 高五位为零
    eModDrvWinlineRegSetAddrMode        = 0x001F,                               //只写, 整  型, 地址分配方式, 0x00000000:自动分配   0x00010000:拨码设置（默认）
    eModDrvWinlineRegSetPdcOut          = 0x0020,                               //只写, 浮点数, 输出功率, 0.1~1 例如：0.1 对应为 0.1*20000W(额定功率)
    eModDrvWinlineRegSetUdcOut          = 0x0021,                               //只写, 浮点数, 输出电压
    eModDrvWinlineRegSetIdcOutLimit     = 0x0022,                               //只写, 浮点数, 模块限流点---其实与0x1b是一样的，只是单位是额定电流的百分比
    eModDrvWinlineRegSetUdcOutHighLimit = 0x0023,                               //只写, 浮点数, 输出过压点, 无特殊需求不要设置
    eModDrvWinlineRegSetTurnOnOff       = 0x0030,                               //只写, 整  型, 开关机, 0x00010000:关机 ； 0x00000000:开机
    eModDrvWinlineRegSetRstByVoltHigh   = 0x0031,                               //只写, 整  型, 过压复位, 0x00000000:禁止；0x00010000:(允许)复位
    eModDrvWinlineRegSetUdcOutHighAssoc = 0x003E,                               //只写, 整  型, 输出过压保护关联是否允许, 0x00000000:允许；0x00010000:禁止
    eModDrvWinlineRegGetWarningCode     = 0x0040,                               //只读, 整  型, 当前告警/状态, 
    eModDrvWinlineRegGetAddrBySwitch    = 0x0043,                               //只读, 整  型, 拨码地址, 
    eModDrvWinlineRegSetRstByShort      = 0x0044,                               //只写, 整  型, 模块短路复位, 0x00000000：禁止；0x00010000：(允许)复位
    eModDrvWinlineRegSetInputMode       = 0x0046,                               //只写, 整  型, 输入模式, 0x00000001：交流模式(默认)；0x00000002：直流模式
    eModDrvWinlineRegGetPowerInput      = 0x0048,                               //只读, 整  型, 输入功率, 单位 1W
    eModDrvWinlineRegGetAltitude        = 0x004A,                               //只读, 整  型, 当前设定的海拔值, 单位：米（默认 1000）。
    eModDrvWinlineRegGetInputMode       = 0x004B,                               //只读, 整  型, 当前模块输入工作模式, 0x00000001：单相交流； 0x00000002：直流； 0x00000003：三相交流； 0x00000005：模式不匹配（相序错误）；
    eModDrvWinlineRegGetSerialNoLow     = 0x0054,                               //只读, 整  型, 节点 SerialNo 号低位（ID 号）
    eModDrvWinlineRegGetSerialNoHigh    = 0x0055,                               //只读, 整  型, 节点 SerialNo 号高位（ID 号）
    eModDrvWinlineRegGetSwVerDcdc       = 0x0056,                               //只读, 整  型, DCDC 版本号, 返回数据的低 16 bit(即 byte6~byte7)，版本号以数据的
    eModDrvWinlineRegGetSwVerPfc        = 0x0057,                               //只读, 整  型, PFC  版本号, 同上
    eModDrvWinlineRegGetFanS            = 0x0075,                               //只读, 整  型, 风扇转速。0~2400，对应风扇转速比0~1
    
    eModDrvWinlineRegMax                = 0xFFFF                                //确保本字段占用2个字节内存
}__attribute__((packed)) eModDrvWinlineReg_t;












//永联模块modbus协议 地址分配方式 定义
typedef enum
{
    eModDrvWinlineAddrModeAlloc         = 0x00000000,                           //自动分配
    eModDrvWinlineAddrModeSwitch        = 0x00010000,                           //拨码设置（默认）
    
    eModDrvWinlineAddrModeMax           = 0xFFFFFFFF                            //确保占据4个字节
}__attribute__((packed)) eModDrvWinlineAddrMode_t;



//永联模块modbus协议 开关机 定义
typedef enum
{
    eModDrvWinlineTurnOn                = 0x00000000,                           //开机
    eModDrvWinlineTurnOff               = 0x00010000,                           //关机
    
    eModDrvWinlineTurnMax               = 0xFFFFFFFF                            //确保占据4个字节
}__attribute__((packed)) eModDrvWinlineTurn_t;



//永联模块modbus协议 模块过压复位 定义
typedef enum
{
    eModDrvWinlineRstByVoltHighDisable  = 0x00000000,                           //禁止过压复位
    eModDrvWinlineRstByVoltHighEnable   = 0x00010000,                           //要求过压复位
    
    eModDrvWinlineRstByVoltHighMax      = 0xFFFFFFFF                            //确保占据4个字节
}__attribute__((packed)) eModDrvWinlineRstByVoltHigh_t;



//永联模块modbus协议 模块输出过压保护关联是否允许 定义
typedef enum
{
    eModDrvWinlineUdcOutHighAssocEnable = 0x00000000,                           //允许
    eModDrvWinlineUdcOutHighAssocDisable= 0x00010000,                           //禁止
    
    eModDrvWinlineUdcOutHighAssocMax    = 0xFFFFFFFF                            //确保占据4个字节
}__attribute__((packed)) eModDrvWinlineUdcOutHighAssoc_t;




//永联模块modbus协议 告警/状态 定义
typedef struct
{
    u32                                 bFault          : 1;                    //模块故障（红指示灯亮）
    u32                                 bProtect        : 1;                    //模块保护（黄指示灯亮）
    u32                                 bRsvd2          : 1;                    //保留
    u32                                 bFaultSci       : 1;                    //模块内部 SCI 通信故障
    u32                                 bFaultInput     : 1;                    //输入模式检测错误（或输入接线错误）
    u32                                 bInputNotMatch  : 1;                    //监控下发输入模式与实际工作模式不匹配
    u32                                 bRsvd6          : 1;                    //保留
    u32                                 bUdcdcHigh      : 1;                    //DCDC 过压
    u32                                 bFaultUpfc      : 1;                    //PFC 电压异常（不平衡或过压或欠压）
    u32                                 bUacHigh        : 1;                    //交流过压
    u32                                 bRsvd10         : 1;                    //保留
    u32                                 bRsvd11         : 1;                    //保留
    u32                                 bRsvd12         : 1;                    //保留
    u32                                 bRsvd13         : 1;                    //保留
    u32                                 bUacLow         : 1;                    //交流欠压
    u32                                 bRsvd15         : 1;                    //保留
    u32                                 bFaultCan       : 1;                    //CAN 通信故障
    u32                                 bCurrImbalance  : 1;                    //模块不均流
    u32                                 bRsvd18         : 1;                    //保留
    u32                                 bRsvd19         : 1;                    //保留
    u32                                 bRsvd20         : 1;                    //保留
    u32                                 bRsvd21         : 1;                    //保留
    u32                                 bStsDcdc        : 1;                    //DCDC 开关机状态 0：开机，1：关机
    u32                                 bPdcLimit       : 1;                    //模块限功率
    u32                                 bPdcLimitByTemp : 1;                    //温度限功率
    u32                                 bPdcLimitByAc   : 1;                    //交流限功率
    u32                                 bRsvd26         : 1;                    //保留
    u32                                 bFaultFan       : 1;                    //风扇故障
    u32                                 bShortDcdc      : 1;                    //DCDC 短路
    u32                                 bRsvd29         : 1;                    //保留
    u32                                 bTempDcdcHigh   : 1;                    //DCDC 过温
    u32                                 bUdcdcOutHigh   : 1;                    //DCDC 输出过压
    
}__attribute__((packed)) stModDrvWinlineWarningCode_t;



//永联模块modbus协议 模块短路复位 定义
typedef enum
{
    eModDrvWinlineRstByShortDisable     = 0x00000000,                           //禁止短路复位
    eModDrvWinlineRstByShortEnable      = 0x00010000,                           //要求短路复位
    
    eModDrvWinlineRstByShortMax         = 0xFFFFFFFF                            //确保占据4个字节
}__attribute__((packed)) eModDrvWinlineRstByShort_t;



//永联模块modbus协议 模块短路复位 定义                                          //设置时              读取时
typedef enum
{
    eModDrvWinlineInputModeAc1          = 0x00000001,                           //交流模式(默认)      单相交流
    eModDrvWinlineInputModeDc           = 0x00000002,                           //直流模式            直流
    eModDrvWinlineInputModeAc3          = 0x00000003,                           //N/A                 三相交流
    eModDrvWinlineInputModeNotMatch     = 0x00000005,                           //N/A                 模式不匹配（相序错误）
    
    eModDrvWinlineInputModeMax          = 0xFFFFFFFF                            //确保占据4个字节
}__attribute__((packed)) eModDrvWinlineInputMode_t;
























#endif













