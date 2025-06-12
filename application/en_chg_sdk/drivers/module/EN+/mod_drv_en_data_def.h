/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   mod_drv_en_data_def.h
* Description                           :   EN+模块电源CAN通讯驱动实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2022-07-30
* notice                                :   
****************************************************************************************************/
#ifndef _mod_drv_en_data_def_h_
#define _mod_drv_en_data_def_h_
#include "en_common.h"
#include "en_log.h"













#define cModDrvEnSerialNumLen           (16)                                    //EN+模块 序列号长度
#define cModDrvEnModelNameLen           (16)                                    //EN+模块 model name长度
#define cModDrvEnManufacturerIdLen      (16)                                    //EN+模块 制造商ID长度
#define cModDrvEnSwVerLen               (16)                                    //EN+模块 软件版本号








//EN+ 模块电源CAN协议 CMD04指令 故障码表
typedef struct
{
    //byte0
    u8                                  bTctrlboardPfcHighFault      : 1;       //PFC控制板温度过高错误
    u8                                  bTradiatorPfcHighFault       : 1;       //PFC散热器温度过高错误
    u8                                  bUbusHighHWFault             : 1;       //bus硬件过压错误
    u8                                  bUbusHighSWFault             : 1;       //bus软件过压错误
    u8                                  bUbusContinueHighHWFailure   : 1;       //bus硬件连续过压故障
    u8                                  bUbusContinueHighSWFailure   : 1;       //bus软件连续过压故障
    u8                                  bIacContinueHighHWFailure    : 1;       //Iac硬件连续过流故障
    u8                                  bIacContinueHighSWFailure    : 1;       //Iac软件连续过流故障
    
    //byte1
    u8                                  bFacFault                    : 1;       //市电过欠频错误
    u8                                  bUacFault                    : 1;       //市电过欠压错误
    u8                                  bPfcEepromFault              : 1;       //PFC eeprom读写错误
    u8                                  bIacHighSWFault              : 1;       //Iac软件过流错误
    u8                                  bIacHighHWFault              : 1;       //Iac硬件过流错误
    u8                                  bReserved2D                  : 1;       //保留位
    u8                                  bReserved2E                  : 1;       //保留位
    u8                                  bReserved2F                  : 1;       //保留位
    
    //byte2
    u8                                  bFanFault                    : 1;       //风扇错误
    u8                                  bAddrFault                   : 1;       //地址异常错误
    u8                                  bCanDisConnectFault          : 1;       //can通讯中断错误
    u8                                  bVerisonMissMatchFailure     : 1;       //软硬件版本不匹配故障
    u8                                  bLlcEepromFault              : 1;       //LLC eeprom读写错误
    u8                                  bTtransformerLlcHighFault    : 1;       //LLC变压器温度过高错误
    u8                                  bTairLlcHighFault            : 1;       //LLC进风口温度过高错误
    u8                                  bTradiatorLlcHighFault       : 1;       //LLC散热器温度过高错误
    
    //byte3
    u8                                  bModuleUnbalanceFault        : 1;       //并机不均流错误
    u8                                  bIdcContinueHighHWFailure    : 1;       //输出电流Idc硬件连续过流故障
    u8                                  bIdcContinueHighSWFailure    : 1;       //输出电流Idc软件连续过流故障
    u8                                  bUdcContinueHighHWFailure    : 1;       //输出电压Udc硬件连续过压故障
    u8                                  bUdcContinueHighSWFailure    : 1;       //输出电压Udc软件连续过压故障
    u8                                  bIrContinueHighHWFailure     : 1;       //谐振槽电流硬件连续过流故障
    u8                                  bIrContinueHighSWFailure     : 1;       //谐振槽电流软件连续过流故障
    u8                                  bShortFault                  : 1;       //输出短路或输出低压错误
    
    //byte4
    u8                                  bIrHighSWFault               : 1;       //谐振槽电流软件过流错误
    u8                                  bIrHighHWFault               : 1;       //谐振槽电流硬件过流错误
    u8                                  bUdcHighSWFault              : 1;       //输出电压Udc软件过压错误
    u8                                  bUdcHighHWFault              : 1;       //输出电压Udc硬件过压错误
    u8                                  bIdcHighSWFault              : 1;       //输出电流Idc软件过流错误
    u8                                  bIdcHighHWFault              : 1;       //输出电流Idc硬件过流错误
    u8                                  bInterComLosFault            : 1;       //内部串口通讯中断错误
    u8                                  bReserved47                  : 1;
    
    //byte5
    u8                                  bTtransformerLlcDropWarning  : 1;       //LLC变压器过温降载告警
    u8                                  bTairLlcDropWarning          : 1;       //LLC进风口过温降载告警
    u8                                  bTradiatorLlcDropWarning     : 1;       //LLC散热器过温降载告警
    u8                                  bTctrlboardPfcDropWarning    : 1;       //PFC控制板过温降载告警
    u8                                  bTradiatorPfcDropWarning     : 1;       //PFC散热器过温降载告警
    u8                                  bLlcIrUevenflowDropWarning   : 1;       //LLC两路谐振电流不均流降额告警
    u8                                  bDischargeFailWarning        : 1;       //放电失败告警
    u8                                  bReserved4F                  : 1;

}stModDrvEnErrCode_t;

typedef union
{
    stModDrvEnErrCode_t                 stValue;                                //结构体访问
    u8                                  u8Value[6];                             //数组访问
}unModDrvEnErrCode_t;




//EN+ 模块电源CAN协议 CMD06指令 工作状态定义
typedef enum 
{
    eModDrvEnWorkStateWait = 0x00,                                              //等待开机状态，等待上位机的开机指令。
    eModDrvEnWorkStateCheck,                                                    //自检状态，已经收到开机指令，做开机前自检。
    eModDrvEnWorkStateSoftStart,                                                //软启动，已经开机。
    eModDrvEnWorkStateNormal,                                                   //已经开机，正常输出。
    eModDrvEnWorkStateFault,                                                    //故障状态，模块内部发生故障。
    
    eModDrvEnWorkStateMax
}__attribute__((packed)) eModDrvEnWorkState_t;


//EN+ 模块电源CAN协议 CMD06指令 & CMD21指令 智能并机状态定义
typedef enum 
{
    eModDrvEnSmartParaStateOff = 0x00,                                          //非智能并机模式
    eModDrvEnSmartParaStateOn,                                                  //  智能并机模式
    
    eModDrvEnSmartParaStateMax
}__attribute__((packed)) eModDrvEnSmartParaState_t;





//EN+ 模块电源CAN协议 CMD19指令 休眠设置值定义
typedef enum 
{
    eModDrvEnSleepValueExit = 0x00,                                             //不进入休眠
    eModDrvEnSleepValueEnter,                                                   //  进入休眠
    
    eModDrvEnSleepValueMax
}__attribute__((packed)) eModDrvEnSleepValue_t;





//EN+ 模块电源CAN协议 CMD1A指令 开关机设置值定义
typedef enum 
{
    eModDrvEnTurnValueOn = 0x00,                                                //开机
    eModDrvEnTurnValueOff,                                                      //关机
    
    eModDrvEnTurnValueMax
}__attribute__((packed)) eModDrvEnTurnValue_t;

//EN+ 模块电源CAN协议  CMD1A指令 返回状态定义
typedef struct
{
    u16                                 bStateAddrMode : 1;                     //    地址模式初始化状态
    u16                                 bStateParaMode : 1;                     //智能并机模式初始化状态
    u16                                 bStateVoltMode : 1;                     //  高低压模式初始化状态
    u16                                 bRsvd : 13;
}__attribute__((packed)) stModDrvEnState3_t;

typedef union
{
    stModDrvEnState3_t                  stValue;                                //结构体访问
    u16                                 u16Value;                               //数组访问
}unModDrvEnState3_t;





//EN+ 模块电源CAN协议 CMD1F指令 地址模式设置值定义
typedef enum 
{
    eModDrvEnAddrModeAutoAlloc = 0x00,                                          //自动分配
    eModDrvEnAddrModeSwitchSet,                                                 //拨码设定
    
    eModDrvEnAddrModeMax
}__attribute__((packed)) eModDrvEnAddrMode_t;





//EN+ 模块电源CAN协议 CMD22指令 CAN广播报文使能禁止设置值定义
typedef enum 
{
    eModDrvEnCanBroadcastDisable = 0x00,                                        //禁止广播
    eModDrvEnCanBroadcastEnable,                                                //使能广播
    
    eModDrvEnCanBroadcastMax
}__attribute__((packed)) eModDrvEnCanBroadcast_t;





//EN+ 模块电源CAN协议 CMD23指令 高低压模式设置值定义
typedef enum 
{
    eModDrvEnVoltModeLow = 0x01,                                                //低压模式
    eModDrvEnVoltModeHigh,                                                      //高压模式
    
    eModDrvEnVoltModeMax
}__attribute__((packed)) eModDrvEnVoltMode_t;





//EN+ 模块电源CAN协议 CMD25 CMD26指令 mod值定义
typedef enum 
{
    eModDrvEnFunModPassive = 0x00,                                              //被动
    eModDrvEnFunModActive,                                                      //主动
    
    eModDrvEnFunModMax
}__attribute__((packed)) eModDrvEnFunMod_t;







//EN+ 模块电源CAN协议 CMD31指令 模块特征字结构--->效率类型定义
typedef enum 
{
    eModDrvEnEfficiencyTypeLess90 = 0x00,                                       //<90%
    eModDrvEnEfficiencyType90_91,                                               //90~91%
    eModDrvEnEfficiencyType91_92,                                               //91~92%
    eModDrvEnEfficiencyType92_93,                                               //92~93%
    eModDrvEnEfficiencyType93_94,                                               //93~94%
    eModDrvEnEfficiencyType94_95,                                               //94~95%
    eModDrvEnEfficiencyType95_96,                                               //95~96%
    eModDrvEnEfficiencyType96_97,                                               //96~97%
    eModDrvEnEfficiencyType97_98,                                               //97~98%
    eModDrvEnEfficiencyType98_99,                                               //98~99%
    eModDrvEnEfficiencyTypeMore99,                                              //>99%
    
    eModDrvEnEfficiencyTypeMax
}eModDrvEnEfficiencyType_t;

//EN+ 模块电源CAN协议 CMD31指令 模块特征字结构--->输出电压类型定义
typedef enum 
{
    eModDrvEnUoutType500 = 0x00,                                                //500V
    eModDrvEnUoutType750,                                                       //750V
    eModDrvEnUoutType1000,                                                      //1000V
    
    eModDrvEnUoutTypeMax
}eModDrvEnUoutType_t;

//EN+ 模块电源CAN协议 CMD31指令 模块特征字结构--->输入电压类型定义
typedef enum 
{
    eModDrvEnUinTypeAc220 = 0x00,                                               //交流220V输入
    eModDrvEnUinTypeAc110,                                                      //交流110V输入
    eModDrvEnUinTypeAcDcSun = 0x03,                                             //交流、直流、太阳能输入
    
    //eModDrvEnUinTypeMax
}eModDrvEnUinType_t;

//EN+ 模块电源CAN协议 CMD31指令 模块特征字结构--->交流相数定义
typedef enum 
{
    eModDrvEnPhaseNumSingle = 0x00,                                             //单相输入
    eModDrvEnPhaseNumThree,                                                     //三相输入
    
    eModDrvEnPhaseNumMax
}eModDrvEnPhaseNum_t;

//EN+ 模块电源CAN协议 CMD31指令 模块特征字结构--->供电方式定义
typedef enum 
{
    eModDrvEnPowerSupplyModeSingle = 0x00,                                      //单供电
    eModDrvEnPowerSupplyModeDouble,                                             //双供电
    
    eModDrvEnPowerSupplyModeMax
}eModDrvEnPowerSupplyMode_t;


//EN+ 模块电源CAN协议  CMD31指令 模块特征字结构定义
typedef struct
{
    u32                                 bRsvd               : 9;                //保留
    eModDrvEnEfficiencyType_t           eEfficiencyType     : 4;
    eModDrvEnUoutType_t                 eUoutTpye           : 3;
    u32                                 bIrated             : 10;               //额定电流 单位：0.1
    eModDrvEnUinType_t                  eUinType            : 2;
    eModDrvEnPhaseNum_t                 ePhaseNum           : 2;
    eModDrvEnPowerSupplyMode_t          ePowerSupplyMode    : 2;
}stModDrvEnFeatureWord_t;

typedef union
{
    stModDrvEnFeatureWord_t             stValue;                                //结构体访问
    u32                                 u32Value;                               //32位访问
}unModDrvEnFeatureWord_t;














#endif








