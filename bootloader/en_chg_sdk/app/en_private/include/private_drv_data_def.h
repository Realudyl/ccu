/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_data_def.h
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-10-12
* notice                                :     本文件定义协议中所有的数据类型：枚举，（变量）结构体，（变量）联合 等等
*
*                                             1:程序中涉及到枪数量时，一律使用 GunNum 来指代
*                                             2:            枪口  时，一律使用 GunId  来指代
****************************************************************************************************/
#ifndef _private_drv_data_def_H_
#define _private_drv_data_def_H_
#include "en_common.h"
#include "private_drv_fault_data_def.h"

#if (cSdkPrivDevType == cSdkPrivDevTypeM)                                       //Master 侧是IoT SDK
#include "en_iot_sdk_cfg.h"
#else                                                                           //Slave  侧是chg SDK
#include "en_log.h"
#include "en_chg_sdk_cfg.h"

#endif









//缓存区大小定义
#define cPrivDrvBufSize                 (1024)
#define cChgSwVerBootloader             "bootloader"  //boot模式





//--------------------------------------报文头----------------------------------
//报文头定义
typedef enum
{
    ePrivDrvHeader                      = 0xFA,                                 //header
    
    ePrivDrvHeaderMax
}__attribute__((packed)) ePrivDrvHeader_t;



//协议版本
typedef enum
{
    ePrivDrvVersion30                   = 0x03,                                 //version内网3.0版本
    ePrivDrvVersion                     = 0x36,                                 //version
    
    ePrivDrvVersionMax
}__attribute__((packed)) ePrivDrvVersion_t;



//报文类型定义
typedef enum
{
    ePrivDrvCmd01                       = 0x01,                                 //桩地址申请
    ePrivDrvCmdA1                       = 0xA1,                                 //桩地址申请应答
    
    ePrivDrvCmd02                       = 0x02,                                 //桩同步信息
    ePrivDrvCmdA2                       = 0xA2,                                 //同步应答信息
    
    ePrivDrvCmd03                       = 0x03,                                 //状态上报
    ePrivDrvCmdA3                       = 0xA3,                                 //状态应答
    
    ePrivDrvCmd04                       = 0x04,                                 //桩心跳
    ePrivDrvCmdA4                       = 0xA4,                                 //心跳应答
    
    ePrivDrvCmd05                       = 0x05,                                 //用户卡认证
    ePrivDrvCmdA5                       = 0xA5,                                 //卡认证应答
    
    ePrivDrvCmd06                       = 0x06,                                 //远程操作应答
    ePrivDrvCmdA6                       = 0xA6,                                 //远程操作(APP)
    
    ePrivDrvCmd07                       = 0x07,                                 //开始/结束充电
    ePrivDrvCmdA7                       = 0xA7,                                 //开始/结束充电应答
    
    ePrivDrvCmd08                       = 0x08,                                 //实时充电数据
    ePrivDrvCmdA8                       = 0xA8,                                 //实时充电数据应答
    
    ePrivDrvCmd09                       = 0x09,                                 //结算报文上报
    ePrivDrvCmdA9                       = 0xA9,                                 //结算报文应答
    
    ePrivDrvCmd0A                       = 0x0A,                                 //桩信息设置/查询应答
    ePrivDrvCmdAA                       = 0xAA,                                 //桩信息设置/查询
    
    ePrivDrvCmd0B                       = 0x0B,                                 //远程升级应答
    ePrivDrvCmdAB                       = 0xAB,                                 //远程升级
    
    ePrivDrvCmd0C                       = 0x0C,                                 //直流模块远程升级应答
    ePrivDrvCmdAC                       = 0xAC,                                 //直流模块远程升级
    
    ePrivDrvCmd0D                       = 0x0D,                                 //桩端打印日志
    ePrivDrvCmdAD                       = 0xAD,                                 //
    
    ePrivDrvCmd0E                       = 0x0E,                                 //桩端遥测及部分遥信数据上报
    ePrivDrvCmdAE                       = 0xAE,                                 //桩端遥测及部分遥信数据上报应答
    
    ePrivDrvCmd0F                       = 0x0F,                                 //Smart Charging申请充电计划
    ePrivDrvCmdAF                       = 0xAF,                                 //Smart Charging充电详细计划表
    
    ePrivDrvCmd11                       = 0x11,                                 //安全事件通知    （欧标使用）
    ePrivDrvCmdB1                       = 0xB1,                                 //安全事件通知应答（欧标使用）
    
    ePrivDrvCmd12                       = 0x12,                                 //占桩订单信息
    ePrivDrvCmdB2                       = 0xB2,                                 //占桩订单信息应答
    
    ePrivDrvCmd13                       = 0x13,                                 //桩上报BMS信息（直流桩使用）
    ePrivDrvCmdB3                       = 0xB3,                                 //网关应答桩上报BMS信息
    
    ePrivDrvCmd14                       = 0x14,                                 //预约/定时充电应答（蓝牙模式）
    ePrivDrvCmdB4                       = 0xB4,                                 //预约/定时充电    （蓝牙模式）
    
    ePrivDrvCmd15                       = 0x15,                                 //主柜状态信息
    ePrivDrvCmdB5                       = 0xB5,                                 //主柜状态信息应答
    
    ePrivDrvCmd16                       = 0x16,                                 //液冷机状态
    ePrivDrvCmdB6                       = 0xB6,                                 //液冷机状态应答
    
    ePrivDrvCmdMax
}__attribute__((packed)) ePrivDrvCmd_t;








//--------------------------------------协议内受SDK配置文件约束的一些参数-------
//枪数量,最多支持的枪数量,桩和网关必须一致
#ifndef cSdkPrivDrvGunNum
#define cPrivDrvGunNumMax               (1)
#else
#define cPrivDrvGunNumMax               (cSdkPrivDrvGunNum)
#endif
#define cPrivDrvGunIdChg                (0)                                     //GunId 整桩,       协议定义 0---整桩, 1,2...是GunId
#define cPrivDrvGunIdBase               (1)                                     //GunId 枪号基数,   协议定义 0---整桩, 1,2...是GunId




//用户id 长度
#ifndef cSdkPrivDrvUserIdLen
#define cPrivDrvUserIdLen               (32)
#else
#define cPrivDrvUserIdLen               (cSdkPrivDrvUserIdLen)
#endif



//订单编号 长度
#ifndef cSdkPrivDrvOrderIdLen
#define cPrivDrvOrderIdLen              (32)
#else
#define cPrivDrvOrderIdLen              (cSdkPrivDrvOrderIdLen)
#endif



//平台交易流水号 长度
#ifndef cSdkPrivDrvSrvTradeIdLen
#define cPrivDrvSrvTradeIdLen           (32)
#else
#define cPrivDrvSrvTradeIdLen           (cSdkPrivDrvSrvTradeIdLen)
#endif



//设备交易流水号 长度
#ifndef cSdkPrivDrvDevTradeIdLen
#define cPrivDrvDevTradeIdLen           (32)
#else
#define cPrivDrvDevTradeIdLen           (cSdkPrivDrvDevTradeIdLen)
#endif



//虚拟id号 长度
#ifndef cSdkPrivDrvParentIdLen
#define cPrivDrvParentIdLen             (20)
#else
#define cPrivDrvParentIdLen             (cSdkPrivDrvParentIdLen)
#endif



//02报文 预留字段长度
#ifndef cSdkPrivDrvRsvdLen02
#define cPrivDrvRsvdLen02               (1)
#else
#define cPrivDrvRsvdLen02               (cSdkPrivDrvRsvdLen02)
#endif




//A3报文 预留字段长度
#ifndef cSdkPrivDrvRsvdLenA3
#define cPrivDrvRsvdLenA3               (1)
#else
#define cPrivDrvRsvdLenA3               (cSdkPrivDrvRsvdLenA3)
#endif



//05报文 预留字段长度
#ifndef cSdkPrivDrvRsvdLen05
#define cPrivDrvRsvdLen05               (1)
#else
#define cPrivDrvRsvdLen05               (cSdkPrivDrvRsvdLen05)
#endif



//A5报文 预留字段长度
#ifndef cSdkPrivDrvRsvdLenA5
#define cPrivDrvRsvdLenA5               (1)
#else
#define cPrivDrvRsvdLenA5               (cSdkPrivDrvRsvdLenA5)
#endif



//06报文 预留字段长度
#ifndef cSdkPrivDrvRsvdLen06
#define cPrivDrvRsvdLen06               (1)
#else
#define cPrivDrvRsvdLen06               (cSdkPrivDrvRsvdLen06)
#endif



//A6报文 预留字段长度
#ifndef cSdkPrivDrvRsvdLenA6
#define cPrivDrvRsvdLenA6               (1)
#else
#define cPrivDrvRsvdLenA6               (cSdkPrivDrvRsvdLenA6)
#endif



//07报文 预留字段长度
#ifndef cSdkPrivDrvRsvdLen07
#define cPrivDrvRsvdLen07               (1)
#else
#define cPrivDrvRsvdLen07               (cSdkPrivDrvRsvdLen07)
#endif



//A7报文 预留字段长度
#ifndef cSdkPrivDrvRsvdLenA7
#define cPrivDrvRsvdLenA7               (1)
#else
#define cPrivDrvRsvdLenA7               (cSdkPrivDrvRsvdLenA7)
#endif



//A8报文 预留字段长度
#ifndef cSdkPrivDrvRsvdLenA8
#define cPrivDrvRsvdLenA8               (1)
#else
#define cPrivDrvRsvdLenA8               (cSdkPrivDrvRsvdLenA8)
#endif



//0E报文 预留字段长度
#ifndef cSdkPrivDrvRsvdLen0E
#define cPrivDrvRsvdLen0E               (1)
#else
#define cPrivDrvRsvdLen0E               (cSdkPrivDrvRsvdLen0E)
#endif



//AE报文 预留字段长度
#ifndef cSdkPrivDrvRsvdLenAE
#define cPrivDrvRsvdLenAE               (1)
#else
#define cPrivDrvRsvdLenAE               (cSdkPrivDrvRsvdLenAE)
#endif



//14报文 预留字段长度
#ifndef cSdkPrivDrvRsvdLen14
#define cPrivDrvRsvdLen14               (1)
#else
#define cPrivDrvRsvdLen14               (cSdkPrivDrvRsvdLen14)
#endif



//B4报文 预留字段长度
#ifndef cSdkPrivDrvRsvdLenB4
#define cPrivDrvRsvdLenB4               (1)
#else
#define cPrivDrvRsvdLenB4               (cSdkPrivDrvRsvdLenB4)
#endif



//WorkStatus转换
//车端暂停和桩端暂停为了标准化和兼容旧状态,因此使用用户sdk_cfg选择20240226
//参考ocpp1.6-41也状态转换表
//1:需要转换(4-->5,5-->4), 0:标准模式
#ifndef cSdkPrivDrvWorkStsConvertEn
#define cPrivDrvWorkStsConvertEn        (1)
#else
#define cPrivDrvWorkStsConvertEn        (cSdkPrivDrvWorkStsConvertEn)
#endif




//价格字段精度配置
#ifndef cSdkPrivDrvPrecisionPrice
#define cPrivDrvPrecisionPrice          (5)
#elif   (cSdkPrivDrvPrecisionPrice < 0)
#define cPrivDrvPrecisionPrice          (5)
#elif   (cSdkPrivDrvPrecisionPrice > 8)
#define cPrivDrvPrecisionPrice          (5)
#else
#define cPrivDrvPrecisionPrice          (cSdkPrivDrvPrecisionPrice)
#endif




//金额字段精度配置
#ifndef cSdkPrivDrvPrecisionMoney
#define cPrivDrvPrecisionMoney          (3)
#elif   (cSdkPrivDrvPrecisionMoney < 0)
#define cPrivDrvPrecisionMoney          (3)
#elif   (cSdkPrivDrvPrecisionMoney > 8)
#define cPrivDrvPrecisionMoney          (3)
#else
#define cPrivDrvPrecisionMoney          (cSdkPrivDrvPrecisionMoney)
#endif







//--------------------------------------01、A1报文------------------------------
#define cPrivDrv01RsvdLen               (6)                                     //预留字节长度
#define cPrivDrvA1RsvdLen               (4)                                     //预留字节长度



//A1报文--->命令类型
typedef enum
{
    ePrivDrvA1CmdSetAddr                = 0x00,                                 //设置桩地址
    ePrivDrvA1CmdRelogin                = 0x01,                                 //请求桩重新登录
    
    ePrivDrvA1CmdMax
}__attribute__((packed)) ePrivDrvA1Cmd_t;








//--------------------------------------02、A2报文------------------------------
//--------------------------------------内网3.0协议参数定义
//交直流
typedef enum
{
    ePrivDrvChgTypeAc                   = 0,                                    //交流
    ePrivDrvChgTypeThreeAc,                                                     //交流 3相
    ePrivDrvChgTypeDc,                                                          //直流
}__attribute__((packed)) ePrivDrvChgType_t;



//结算时机
typedef enum
{
    ePrivDrvRcdTimingPullOutGun         = 0,                                    //拔枪结算
    ePrivDrvRcdTimingStopCharg,                                                 //停充结算
}__attribute__((packed)) ePrivDrvRcdTiming_t;



//计费端
typedef enum
{
    ePrivDrvBillSideCharg               = 0,                                    //桩端计费
    ePrivDrvBillSideServer                                                      //服务器计费
}__attribute__((packed)) ePrivDrvBillSide_t;



//桩禁用
typedef enum
{
    ePrivDrvDisableChargEnable          = 0,                                    //启用
    ePrivDrvDisableChargDisable                                                 //禁用
}__attribute__((packed)) ePrivDrvDisable_t;




//功率分配启动模式
typedef enum
{
    ePrivDrvStartPowerNormal            = 0,                                    //默认正常启动
    ePrivDrvStartPowerMinPower                                                  //以最小功率启动
}__attribute__((packed)) ePrivDrvStartPower_t;



//桩工作模式
typedef enum
{
    ePrivDrvChgWorkModeOnLine           = 0,                                    //联网运营
    ePrivDrvChgWorkModeOffLine,                                                 //离网计费(cpu卡)
    ePrivDrvChgWorkModePnp,                                                     //即插即用
    ePrivDrvChgWorkModeOcop,                                                    //一卡一桩
}__attribute__((packed)) ePrivDrvChgWorkMode_t;



//三相平衡挂载相位
typedef enum
{
    ePrivDrvLoadPhaseBalance            = 0,                                    //三相平衡/直流
    ePrivDrvLoadPhaseA,                                                         //A相
    ePrivDrvLoadPhaseB,                                                         //B相
    ePrivDrvLoadPhaseC,                                                         //C相
}__attribute__((packed)) ePrivDrvLoadPhase_t;




//附录5.2: 桩工作状态(ocpp)
typedef enum
{
    ePrivDrvOcppStsNo                   = 0,
    ePrivDrvOcppStsAvailable,                                                   //可用状态
    ePrivDrvOcppStsPreparing,                                                   //插枪状态
    ePrivDrvOcppStsCharging,                                                    //充电状态
    ePrivDrvOcppStsSuspendedevse,                                               //桩端暂停
    ePrivDrvOcppStsSuspendedev,                                                 //车端暂停
    ePrivDrvOcppStsFinishing,                                                   //充电结束态
    ePrivDrvOcppStsReserved,                                                    //预约状态
    ePrivDrvOcppStsUnAvailable,                                                 //不可用状态
    ePrivDrvOcppStsFault                                                        //故障态
}__attribute__((packed)) ePrivDrvOcppSts_t;

//附录5.2: 桩工作状态(国内)
typedef enum
{
    ePrivDrvGbStsFault = 0x01,                                                  //故障 
    ePrivDrvGbStsIdle,                                                          //空闲
    ePrivDrvGbStsCharging,                                                      //充电
    ePrivDrvGbStsStop,                                                          //停冲
    ePrivDrvGbStsReserved,                                                      //预约
    ePrivDrvGbStsUnAvailable,                                                   //禁用
    ePrivDrvGbStsBmsStarting,                                                   //BMS启动中(直流)
    ePrivDrvGbStsBmsPreparing,                                                  //BMS开机准备中(直流)
    ePrivDrvGbStsBmsStop                                                        //BMS运行暂停(直流)
}__attribute__((packed)) ePrivDrvGbSts_t;



typedef union
{
    ePrivDrvGbSts_t                     eGbSts;
    ePrivDrvOcppSts_t                   eOcppSts;
    u8                                  u8WorkStsAll;
}__attribute__((packed)) unPrivDrvWorkSts_t;





//--------------------------------------内网3.6协议参数定义
#define cPrivDrvLenSn                   32                                      //长度---桩SN号
#define cPrivDrvLenDevCode              32                                      //长度---设备型号代码 
#define cPrivDrvLenDevName              32                                      //长度---设备名称
#define cPrivDrvLenHwVer                32                                      //长度---硬件版本号
#define cPrivDrvLenSwVer                32                                      //长度---软件版本号



//充电桩类型
typedef enum
{
    ePrivDrvChargerTypeAcSingle         = 0x00,                                 //单相交流
    ePrivDrvChargerTypeAcThree          = 0x01,                                 //三相交流
    ePrivDrvChargerTypeDcSingle         = 0x02,                                 //单相直流
    ePrivDrvChargerTypeDcThree          = 0x03,                                 //三相直流
    
    ePrivDrvChargerTypeMax
}__attribute__((packed)) ePrivDrvChargerType_t;



//结算时机
typedef enum
{
    ePrivDrvPayAtPlugout                = 0x00,                                 //拔枪结算
    ePrivDrvPayAtChargingStop           = 0x01,                                 //停充结算
    
    ePrivDrvPayAtMax
}__attribute__((packed)) ePrivDrvPayAt_t;



//计费端
typedef enum
{
    ePrivDrvMoneyCalcByChg              = 0x00,                                 //桩端计费
    ePrivDrvMoneyCalcBySrv              = 0x01,                                 //服务器计费
    
    ePrivDrvMoneyCalcByMax
}__attribute__((packed)) ePrivDrvMoneyCalcBy_t;



//桩禁用状态
typedef enum
{
    ePrivDrvChargerDisableFlagFalse     = 0x00,                                 //未禁用
    ePrivDrvChargerDisableFlagTrue      = 0x01,                                 //禁用态
    
    ePrivDrvChargerDisableFlagMax
}__attribute__((packed)) ePrivDrvChargerDisableFlag_t;



//功率分配启动模式
typedef enum
{
    ePrivDrvStartPowerModeNormal        = 0x00,                                 //默认正常启动
    ePrivDrvStartPowerModeMin           = 0x01,                                 //以最小功率启动
    
    ePrivDrvStartPowerModeMax
}__attribute__((packed)) ePrivDrvStartPowerMode_t;



//桩工作模式--->桩工作模式 字段值定义
typedef enum
{
    ePrivDrvChargerWorkModeNetwork      = 0x00,                                 //联网运营
    ePrivDrvChargerWorkModeOffline      = 0x01,                                 //离网计费(cpu卡)
    ePrivDrvChargerWorkModePnc          = 0x02,                                 //即插即用
    ePrivDrvChargerWorkModeOnlyCard     = 0x03,                                 //一卡一桩
    ePrivDrvChargerWorkModeTimer        = 0x04,                                 //定时充电
    ePrivDrvChargerWorkModeBle          = 0x05,                                 //蓝牙模式
    ePrivDrvChargerWorkModeVin          = 0x06,                                 //VIN码充电模式
    
    ePrivDrvChargerWorkModeMax
}__attribute__((packed)) ePrivDrvChargerWorkMode_t;



//三相平衡挂载相位
typedef enum
{
    ePrivDrvBalancePhaseAll             = 0x00,                                 //三相平衡/直流
    ePrivDrvBalancePhaseA               = 0x01,                                 //A相
    ePrivDrvBalancePhaseB               = 0x02,                                 //B相
    ePrivDrvBalancePhaseC               = 0x03,                                 //C相
    
    ePrivDrvBalancePhaseMax
}__attribute__((packed)) ePrivDrvBalancePhase_t;



//是否支持蓝牙预约
typedef enum
{
    ePrivDrvBookingViaBleFalse          = 0x00,                                 //不支持
    ePrivDrvBookingViaBleTrue           = 0x01,                                 //  支持
    
    ePrivDrvBookingViaBleMax
}__attribute__((packed)) ePrivDrvBookingViaBle_t;



//上电期间登录服务器标志
typedef enum
{
    ePrivDrvLoginedFlagFalse            = 0x00,                                 //未登录成功过
    ePrivDrvLoginedFlagTrue             = 0x01,                                 //已登录成功过
    
    ePrivDrvLoginedFlagMax
}__attribute__((packed)) ePrivDrvLoginedFlag_t;



//重启原因
typedef enum
{
    ePrivDrvRebootReasonNormal          = 0x01,                                 //正常断电
    ePrivDrvRebootReasonSw              = 0x02,                                 //软件重启----软件控制（网关需重启）
    ePrivDrvRebootReasonHw              = 0x03,                                 //硬件重启----软件控制（网关需重启）
    ePrivDrvRebootReasonWdt             = 0x04,                                 //看门狗复位----异常死机情况
    ePrivDrvRebootReasonPwrErr          = 0x05,                                 //异常断电
    
    ePrivDrvRebootReasonMax
}__attribute__((packed)) ePrivDrvRebootReason_t;



//附录5.2---枪工作状态-----并非02报文专用，只是本字段最早出现在02报文中，其他字段也一样
typedef enum
{
//内网3.6协议统一集合桩状态,欧标/国标/其他标准都是共用欧标枚举
//应用层使用时应该根据当前标准、机型、桩类型判断对应状态
//------------------------------------------------------------------------------欧标------------------国标
    ePrivDrvGunWorkStatusCheck          = 0x00,                                 //无                  初始/自检中
    ePrivDrvGunWorkStatusAvailable      = 0x01,                                 //可用状态            空闲态
    ePrivDrvGunWorkStatusPreparing      = 0x02,                                 //插枪状态            空闲态
    ePrivDrvGunWorkStatusCharging       = 0x03,                                 //充电状态            充电态
#if(cPrivDrvWorkStsConvertEn == 1)
    ePrivDrvGunWorkStatusSuspendedByEvse= 0x04,                                 //桩端暂停            停机
    ePrivDrvGunWorkStatusSuspendedByEv  = 0x05,                                 //车端暂停            停机
#else
    ePrivDrvGunWorkStatusSuspendedByEv  = 0x04,                                 //车端暂停            停机
    ePrivDrvGunWorkStatusSuspendedByEvse= 0x05,                                 //桩端暂停            停机
#endif
    ePrivDrvGunWorkStatusFinishing      = 0x06,                                 //充电结束态          停机
    ePrivDrvGunWorkStatusReserved       = 0x07,                                 //预约状态            预约/定时
    ePrivDrvGunWorkStatusUnavailable    = 0x08,                                 //不可用状态          禁止充电状态/升级状态
    ePrivDrvGunWorkStatusFaulted        = 0x09,                                 //故障态              故障态
    
    ePrivDrvGunWorkStatusMax
}__attribute__((packed)) ePrivDrvGunWorkStatus_t;



//枪连接状态
typedef enum
{
    ePrivDrvGunConnStatusDisconn        = 0x00,                                 //断开
    ePrivDrvGunConnStatusConnHalf       = 0x01,                                 //半连接---9V
    ePrivDrvGunConnStatusConn           = 0x02,                                 //连接-----6V
    ePrivDrvGunConnStatusUnknow         = 0x03,                                 //未知
    
    ePrivDrvGunConnStatusMax
}__attribute__((packed)) ePrivDrvGunConnStatus_t;



//枪锁状态
typedef enum
{
    ePrivDrvGunLockStatusNone           = 0x00,                                 //无状态(无锁填0)
    ePrivDrvGunLockStatusLock           = 0x01,                                 //上锁
    ePrivDrvGunLockStatusUnlock         = 0x02,                                 //解锁
    ePrivDrvGunLockStatusUnknow         = 0x03,                                 //未知
    
    ePrivDrvGunLockStatusMax
}__attribute__((packed)) ePrivDrvGunLockStatus_t;



//枪状态数据
typedef struct 
{
    ePrivDrvGunWorkStatus_t             eWorkStatus[cPrivDrvGunNumMax];         //枪工作状态
    ePrivDrvGunConnStatus_t             eConnStatus[cPrivDrvGunNumMax];         //枪连接状态
    ePrivDrvGunLockStatus_t             eLockStatus[cPrivDrvGunNumMax];         //枪  锁状态
    
}__attribute__((packed)) stPrivDrvGunStatus_t;



//指令执行结果:签到状态
typedef enum
{
    ePrivDrvCmdRstSuccess               = 0x00,                                 //成功
    ePrivDrvCmdRstFail                  = 0x01,                                 //失败
    
    ePrivDrvCmdRstMax
}__attribute__((packed)) ePrivDrvCmdRst_t;



//签到失败原因
typedef enum
{
    ePrivDrvLoginFailReasonNull         = 0,                                    //空，签到成功时填充
    
    ePrivDrvLoginFailReasonMax
}__attribute__((packed)) ePrivDrvLoginFailReason_t;


//联网类型
typedef enum 
{
    ePrivDrvNetHwTypeWifi               = 0x00,                                 //Wifi   无线
    ePrivDrvNetHwTypeLan,                                                       //LAN    以太网接口
    ePrivDrvNetHwTypeMo,                                                        //Mobile 移动通信
    ePrivDrvNetHwTypeBle,                                                       //蓝牙
    
    ePrivDrvNetHwTypeMax
}__attribute__((packed)) ePrivDrvNetHwType_t;


#define cPrivDrvGwVerLen                (24)                                    //网关版本号长度






//--------------------------------------03、A3报文------------------------------
//枪事件
typedef enum
{
    ePrivDrvGunEventNone                = 0x00,                                 //无
    ePrivDrvGunEventPlugIn              = 0x01,                                 //插枪
    ePrivDrvGunEventPlugOut             = 0x02,                                 //拔枪
    
    ePrivDrvGunEventMax
}__attribute__((packed)) ePrivDrvGunEvent_t;



//附录5.10---枪工作状态变化原因
typedef enum
{
    ePrivDrvGunWorkStsRsnNone           = 0,                                    //无变化
    ePrivDrvGunWorkStsRsnStartByCard    = 1,                                    //刷卡启动
    ePrivDrvGunWorkStsRsnStopByCard     = 2,                                    //刷卡停充
    ePrivDrvGunWorkStsRsnStartBySrv     = 3,                                    //远程启动
    ePrivDrvGunWorkStsRsnStopBySrv      = 4,                                    //远程停充
    ePrivDrvGunWorkStsRsnPlugIn         = 5,                                    //插枪
    ePrivDrvGunWorkStsRsnPlugOut        = 6,                                    //拔枪
    ePrivDrvGunWorkStsRsnS2On           = 7,                                    //S2断开
    ePrivDrvGunWorkStsRsnS2Off          = 8,                                    //S2闭合
    ePrivDrvGunWorkStsRsnStartCharging  = 9,                                    //正常启动充电
    ePrivDrvGunWorkStsRsn10             = 10,                                   //空
    ePrivDrvGunWorkStsRsn11             = 11,                                   //空
    ePrivDrvGunWorkStsRsn12             = 12,                                   //空
    ePrivDrvGunWorkStsRsnFault          = 13,                                   //故障产生
    ePrivDrvGunWorkStsRsnDisable        = 14,                                   //桩被禁用
    ePrivDrvGunWorkStsRsnFull           = 15,                                   //满状态下停充
    ePrivDrvGunWorkStsRsn16             = 16,                                   //空
    ePrivDrvGunWorkStsRsn17             = 17,                                   //空
    ePrivDrvGunWorkStsRsn18             = 18,                                   //空
    ePrivDrvGunWorkStsRsnBooking        = 19,                                   //预约
    ePrivDrvGunWorkStsRsnBySrv          = 20,                                   //服务器下发指令导致
    ePrivDrvGunWorkStsRsnPlugOutFinise  = 21,                                   //充电完成之后，第一次拔枪
    
    ePrivDrvGunWorkStsRsnMax
}__attribute__((packed)) ePrivDrvGunWorkStsRsn_t;



//附录5.4---桩故障代码--->交流桩故障编码----旧的故障位定义
typedef struct
{
    //Byte[0]
    u8                                  bTrelayHigh         : 1;                //继电器过温
    u8                                  bRsvd01             : 1;                //预留
    u8                                  bUacHigh            : 1;                //市电过压
    u8                                  bUacLow             : 1;                //市电欠压
    u8                                  bIacHigh            : 1;                //市电过流
    u8                                  bFacHigh            : 1;                //市电过频
    u8                                  bFacLow             : 1;                //市电欠频
    u8                                  bIleakDcA           : 1;                //直流漏电流异常A
    
    //Byte[1]
    u8                                  bEmgShutdown        : 1;                //紧急停机
    u8                                  bRsvd11             : 1;                //预留
    u8                                  bIleakAc            : 1;                //交流漏电流
    u8                                  bTpluginHigh        : 1;                //输入端子过温
    u8                                  bRsvd12             : 1;                //预留
    u8                                  bIleakDcB           : 1;                //直流漏电流异常B
    u8                                  bRelayAbnormal      : 1;                //继电器异常
    u8                                  bGrounding          : 1;                //接地错误
    
    //Byte[2]
    u8                                  bPhaseInver         : 1;                //反相
    u8                                  bIleakHw            : 1;                //漏电流电路异常
    u8                                  bRsvd21             : 1;                //预留
    u8                                  bElectAbnormal      : 1;                //输入端子过温
    u8                                  bShortCircuit       : 1;                //输出短路故障
    u8                                  bCtAbnormal         : 1;                //CT环采集异常
    u8                                  bMeterCommFault     : 1;                //电表通讯故障
    u8                                  bUcpAbnormal        : 1;                //CP电压异常
    
    //Byte[3]
    u8                                  bRsvd31             : 5;                //预留
    u8                                  bGunLockAbnormal    : 1;                //输入端子过温
    u8                                  bIgunAbnormal       : 1;                //输出短路故障
    u8                                  bIleakDcC           : 1;                //直流漏电流异常C
    
    //Byte[4] - Byte[15]
    u8                                  bByte4              : 8;                //预留
    u8                                  bByte5              : 8;                //预留
    u8                                  bByte6              : 8;                //预留
    u8                                  bByte7              : 8;                //预留
    u8                                  bByte8              : 8;                //预留
    u8                                  bByte9              : 8;                //预留
    u8                                  bByte10             : 8;                //预留
    u8                                  bByte11             : 8;                //预留
    u8                                  bByte12             : 8;                //预留
    u8                                  bByte13             : 8;                //预留
    u8                                  bByte14             : 8;                //预留
    u8                                  bByte15             : 8;                //预留
    
}__attribute__((packed)) stPrivDrvAcFaultByte_t;



//附录5.4---桩故障代码--->直流桩故障编码----旧的故障位定义
typedef struct
{
    //Byte[0]
    u8                                  bCtrlTempHigh       : 1;                //控制板过温
    u8                                  bOverLoad           : 1;                //过载
    u8                                  bUdcHigh            : 1;                //市电过压
    u8                                  bUdcLow             : 1;                //市电欠压
    u8                                  bIdcHigh            : 1;                //市电过流
    u8                                  bFdcHigh            : 1;                //市电过频
    u8                                  bFdcLow             : 1;                //市电欠频
    u8                                  bPreChargFail       : 1;                //预充失败
    
    //Byte[1]
    u8                                  bEmgShutdown        : 1;                //紧急停机
    u8                                  bTempLow            : 1;                //温度过低
    u8                                  bIleak              : 1;                //漏电流
    u8                                  bRsvd1              : 1;                //预留
    u8                                  bRsvd2              : 1;                //预留
    u8                                  bRsvd3              : 1;                //预留
    u8                                  bRelayAbnormal      : 1;                //relay异常
    u8                                  bGrounding          : 1;                //接地错误
    
    //Byte[2]
    u8                                  bCtrlTempRev        : 1;                //电表反接
    u8                                  bIleakDcB           : 1;                //漏电流电路异常
    u8                                  bRsvd4              : 1;                //预留
    u8                                  bElecTotal          : 1;                //电量统计异常
    u8                                  bDistM              : 1;                //测距模块匹配错误
    u8                                  bCanCumm            : 1;                //Can通讯异常
    u8                                  bBusVolt            : 1;                //Bus电压异常
    u8                                  bOutCurrHigh        : 1;                //输出过流
    
    //Byte[3]
    u8                                  bBmsErr             : 1;                //Bms检测到错误
    u8                                  bHumAbnormal        : 1;                //湿度异常
    u8                                  bPhaVoltImba        : 1;                //三相电压不平衡
    u8                                  bPhaCurrImba        : 1;                //三相电流不平衡
    u8                                  bRsvd5              : 1;                //预留
    u8                                  bDcContAbn          : 1;                //直流接触器异常
    u8                                  bDischaIgbtAbn      : 1;                //放电IGBT异常
    u8                                  bInsulImpAbn        : 1;                //绝缘阻抗异常
    
    // Byte[4]
    u8                                  bEnvTempHigh        : 1;                // 环境温度过高
    u8                                  bEnvTempLow         : 1;                // 环境温度过低
    u8                                  bZigbeeCommErr      : 1;                // ZIGBEE通讯异常
    u8                                  bDisplayOverVolt    : 1;                // 显示屏电源过压
    u8                                  bDisplayUnderVolt   : 1;                // 显示屏电源欠压
    u8                                  bBmsOutputMismatch  : 1;                // 实际输出与BMS需求不一致
    u8                                  bDoorMagneticErr    : 1;                // 门磁异常
    u8                                  bCardReaderCommErr  : 1;                // 刷卡器通讯异常
    
    // Byte[5]
    u8                                  bDisplayCommErr     : 1;                // 显示屏通讯异常
    u8                                  bMainCtrlCommErr    : 1;                // 主板与控制板通讯异常
    u8                                  b485AddrConflict    : 1;                // 485通讯地址冲突
    u8                                  bMeterCommFault     : 1;                // 电表通信故障
    u8                                  bAuxPowerAbnormal   : 1;                // 辅助电源异常
    u8                                  bBusVoltageDevia    : 1;                // bus电压枪电压偏差过大
    u8                                  bGunGuideConnErr    : 1;                // 枪连接引导异常
    u8                                  bOutputReverseConn  : 1;                // 输出反接
    // Byte[6]
    u8                                  bInsBusVoltAbn      : 1;                // 绝缘检测BUS电压异常
    u8                                  bVehContStuck       : 1;                // 车辆接触器粘连
    u8                                  bElecLockAbn        : 1;                // 电子锁异常
    u8                                  bDischargResAbn     : 1;                // 放电电阻异常
    u8                                  bK1K2Stuck          : 1;                // K1 K2粘连
    u8                                  bProModuleVer       : 1;                // 烧录程序与模块不匹配
    u8                                  bGunTempAbn         : 1;                // 枪温度异常
    u8                                  bDisplayDcConAbn    : 1;                // 显示板直流接触器异常
    
    // Byte[7]
    u8                                  bBatteryVoltAbn     : 1;                // 电池电压异常
    u8                                  bModuleAddrAbn      : 1;                // 模块地址异常
    u8                                  bRsvd6              : 1;                // 预留
    u8                                  bRsvd7              : 1;                // 预留
    u8                                  bRsvd8              : 1;                // 预留
    u8                                  bRsvd9              : 1;                // 预留
    u8                                  bModuleUnavail      : 1;                // 模块不可用
    u8                                  bPolePowerOff       : 1;                // 桩断电
    //Byte[8] - Byte[11]
    u8                                  bByte8              : 8;                // 预留
    u8                                  bByte9              : 8;                // 预留
    u8                                  bByte10             : 8;                // 预留
    u8                                  bByte11             : 8;                // 预留
    
    // Byte[12]
    u8                                  bModPfcCtrlTempHigh : 1;                // PFC控制板温度过高错误
    u8                                  bModPfcSinkTempHigh : 1;                // PFC散热器温度过高错误
    u8                                  bModBusHwOverVoltage: 1;                // BUS硬件连续过压故障
    u8                                  bModuleBusSwOverVol : 1;                // BUS软件连续过压故障
    u8                                  bModuleIacHwOverCur : 1;                // Iac硬件连续过流故障
    u8                                  bModuleFanErr       : 1;                // 风扇错误
    u8                                  bModuleAddrErr      : 1;                // 地址异常错误
    u8                                  bModuleRsvd1        : 1;                // 模块预留
    
    // Byte[13]
    u8                                  bVerMismatch        : 1;                // 软硬件版本不匹配故障
    u8                                  bEepRWErr           : 1;                // EEPROM读写错误
    u8                                  bLlcTranTempHigh    : 1;                // LLC变压器温度过高错误
    u8                                  bLlcInletTempHigh   : 1;                // LLC进风口温度过高错误
    u8                                  bLlcHeatSinkTempHigh: 1;                // LLC散热器温度过高错误
    u8                                  bIdcHwOverCurr      : 1;                // Idc硬件连续过流故障
    u8                                  bIdcSwOverCurr      : 1;                // Idc软件连续过流故障
    u8                                  bUdcHwOverVolt      : 1;                // Udc硬件连续过压故障
    
    // Byte[14]
    u8                                  bUdcSwOverVolt      : 1;                // Udc软件过压故障
    u8                                  bIrHwOverCurr       : 1;                // Ir硬件过流故障
    u8                                  bOutShortCircuit    : 1;                // 输出短路故障
    u8                                  bInterSeriCommErr   : 1;                // 内部串口通讯错误
    u8                                  bLlcTransrTempWarn  : 1;                // LLC变压器过温降载告警
    u8                                  bLlcInletTempWarn   : 1;                // LLC进风口过温降载告警
    u8                                  bLlcSinkTempWarn    : 1;                // LLC散热器过温降载告警
    u8                                  bPfcCtrlTempWarn    : 1;                // PFC控制板过温降载告警
    
    // Byte[15]
    u8                                  bPfcSinkTempWarn    : 1;                // PFC散热器过温降载告警
    u8                                  bDiscFailureAlarm   : 1;                // 放电失败告警
    u8                                  bModuleRsvd2        : 1;                // 模块预留
    u8                                  bModuleRsvd3        : 1;                // 模块预留
    u8                                  bModuleRsvd4        : 1;                // 模块预留
    u8                                  bModuleRsvd5        : 1;                // 模块预留
    u8                                  bModuleRsvd6        : 1;                // 模块预留
    u8                                  bOtherFault         : 1;                // 其它故障
}__attribute__((packed)) stPrivDrvDcFaultByte_t;



//附录5.4---桩故障编码----旧的故障位定义
#define cPrivDrvFaultCodeLen            16                                      //故障代码长度
typedef union
{
    stPrivDrvAcFaultByte_t              stAc;
    stPrivDrvDcFaultByte_t              stDc;
    u8                                  u8Buf[cPrivDrvFaultCodeLen];
}__attribute__((packed)) unPrivDrvFaultByte_t;



//附录5.5---桩告警编码--->交流桩告警编码----旧的告警位定义
typedef struct
{
    //Byte[0]
    u8                                  bDropByTplugin      : 1;                //输入端子过温降载
    u8                                  bDropByUac          : 1;                //市电过低电压降载
    u8                                  bGrounding          : 1;                //接地警告
    u8                                  bDropByTrelay       : 1;                //继电器过温降载
    u8                                  bRsvd01             : 1;                //预留
    u8                                  bDisassembly        : 1;                //拆机告警
    u8                                  bRsvd02             : 1;                //预留
    u8                                  bRsvd03             : 1;                //预留
    
    //Byte[1] - Byte[3]
    u8                                  bByte1              : 8;                //预留
    u8                                  bByte2              : 8;                //预留
    u8                                  bByte3              : 8;                //预留
}__attribute__((packed)) stPrivDrvAcWarningByte_t;



//附录5.4---桩告警编码--->直流桩告警编码----旧的告警位定义
typedef struct
{
    //Byte[0] - Byte[3]
    u8                                  bByte0              : 8;                //预留
    u8                                  bByte1              : 8;                //预留
    u8                                  bByte2              : 8;                //预留
    u8                                  bByte3              : 8;                //预留
    
}__attribute__((packed)) stPrivDrvDcWarningByte_t;



//附录5.4---桩告警编码----旧的告警位定义
#define cPrivDrvWarningCodeLen            4                                     //告警代码长度
typedef union
{
    stPrivDrvAcWarningByte_t            stAc;
    stPrivDrvDcWarningByte_t            stDc;
    u8                                  u8Buf[cPrivDrvWarningCodeLen];
}__attribute__((packed)) unPrivDrvWarningByte_t;





//新的故障告警编码列表结构
typedef struct
{
    u16                                 u16SumF;                                //当前故障总数
    u16                                 u16SumW;                                //当前告警总数
    ePrivDrvFaultCode_t                 eCodeF[cPrivDrvFaultCodeLen];           //当前故障码列表
    ePrivDrvFaultCode_t                 eCodeW[cPrivDrvFaultCodeLen];           //当前告警码列表
    
}__attribute__((packed)) stPrivDrvFaultCode_t;








//桩连网状态
typedef enum
{
    ePrivDrvNetStatusDisconn            = 0x00,                                 //未连接上网络
    ePrivDrvNetStatusConn               = 0x01,                                 //已经连接上网络
    
    ePrivDrvNetStatusMax
}__attribute__((packed)) ePrivDrvNetStatus_t;








//--------------------------------------04、A4报文------------------------------

//--------------------------------------05、A5报文------------------------------
//卡类型
typedef enum
{
    ePrivDrvCardTypeCpu                 = 0x00,                                 //CPU卡
    ePrivDrvCardTypeM1                  = 0x01,                                 //M1 卡
    ePrivDrvCardTypeVin                 = 0x02,                                 //VIN码
    ePrivDrvCardTypePos                 = 0x03,                                 //POS卡
    
    ePrivDrvCardTypeMax
}__attribute__((packed)) ePrivDrvCardType_t;



//锁卡状态
typedef enum
{
    ePrivDrvCardLockStatusUnlock        = 0x00,                                 //无锁
    ePrivDrvCardLockStatusLock          = 0x01,                                 //锁卡
    
    ePrivDrvCardLockStatusMax
}__attribute__((packed)) ePrivDrvCardLockStatus_t;



//充电模式
typedef enum
{
    ePrivDrvChargingModeFull            = 0x00,                                 //充满为止
    ePrivDrvChargingModeTime            = 0x01,                                 //定时充
    ePrivDrvChargingModeElect           = 0x02,                                 //定电量充
    ePrivDrvChargingModeMoney           = 0x03,                                 //定金额充
    ePrivDrvChargingModeStop            = 0x04,                                 //停车
    ePrivDrvChargingModeBalance         = 0x05,                                 //余额不足停充
    ePrivDrvChargingModeReserve         = 0x06,                                 //预约
    ePrivDrvChargingModeCurrent         = 0x07,                                 //定电流充
    
    ePrivDrvChargingModeMax
}__attribute__((packed)) ePrivDrvChargingMode_t;



//充电参数---针对不同充电模式 单位不一样
typedef enum
{
    ePrivDrvChargingParamnNull          = 0x00,                                 //无
    ePrivDrvChargingParamErr            = 0xFFFFFFFF                            //异常订单
}__attribute__((packed)) ePrivDrvChargingParam_t;

typedef union
{
    u32                                 u32Time;                                //单位: 秒,       for: ePrivDrvChargingModeTime & ePrivDrvChargingModeReserve
    u32                                 u32Elect;                               //单位: 1Wh,      for: ePrivDrvChargingModeElect
    u32                                 u32Money;                               //单位: 0.01元,   for: ePrivDrvChargingModeMoney
    u32                                 u32Current;                             //单位: A,        for: ePrivDrvChargingModeCurrent
    ePrivDrvChargingParam_t             eParam;
    u32                                 u32Value;
}__attribute__((packed)) unPrivDrvChargingParam_t;



//卡账户状态
typedef enum
{
    ePrivDrvCardAccountStatusValid      = 0x00,                                 //有效
    ePrivDrvCardAccountStatusInvalid    = 0x01,                                 //非法
    ePrivDrvCardAccountStatusCharging   = 0x02,                                 //正在另一桩上使用
    
    ePrivDrvCardAccountStatusMax
}__attribute__((packed)) ePrivDrvCardAccountStatus_t;



//卡余额状态
typedef enum
{
    ePrivDrvCardBalanceStatusNotEnough  = 0x00,                                 //欠费
    ePrivDrvCardBalanceStatusEnough     = 0x01,                                 //充足
    ePrivDrvCardBalanceStatusNeedUpdate = 0x02,                                 //充足但需更新
    ePrivDrvCardBalanceStatusDisable    = 0x03,                                 //不允许充电
    
    ePrivDrvCardBalanceStatusMax
}__attribute__((packed)) ePrivDrvCardBalanceStatus_t;



//附录5.1---鉴权失败原因
typedef enum
{
    ePrivDrvCardAuthFailRsn00           = 0,                                    //空（鉴权成功是填写）
    ePrivDrvCardAuthFailRsn01           = 1,                                    //系统异常
    ePrivDrvCardAuthFailRsn02           = 2,                                    //账户状态已冻结或失效
    ePrivDrvCardAuthFailRsn03           = 3,                                    //账户不存在
    ePrivDrvCardAuthFailRsn04           = 4,                                    //交易流水号重复
    ePrivDrvCardAuthFailRsn05           = 5,                                    //授信额度不足
    ePrivDrvCardAuthFailRsn06           = 6,                                    //存在待支付订单
    ePrivDrvCardAuthFailRsn07           = 7,                                    //用户余额不足
    ePrivDrvCardAuthFailRsn08           = 8,                                    //流水号为空
    ePrivDrvCardAuthFailRsn09           = 9,                                    //企业不存在
    ePrivDrvCardAuthFailRsn10           = 10,                                   //用户未设置停机码
    ePrivDrvCardAuthFailRsn11           = 11,                                   //车辆信息校验失败（车云）
    ePrivDrvCardAuthFailRsn12           = 12,                                   //未开通即插即充服务
    ePrivDrvCardAuthFailRsn13           = 13,                                   //互联互通平台校验失败
    ePrivDrvCardAuthFailRsn14           = 14,                                   //无效 VIN 码
    ePrivDrvCardAuthFailRsn15           = 15,                                   //VIN 码未绑定用户
    ePrivDrvCardAuthFailRsn16           = 16,                                   //VIN码已锁定(拒绝该VIN码鉴 权)
    ePrivDrvCardAuthFailRsn17           = 17,                                   //服务异常
    
    ePrivDrvCardAuthFailRsnMax
}__attribute__((packed)) ePrivDrvCardAuthFailRsn_t;



//父ID状态
typedef enum
{
    ePrivDrvParentIdTagNo               = 0x00,                                 //无parentIdTag
    ePrivDrvParentIdTagYes              = 0x01,                                 //有parentIdTag
    
    ePrivDrvParentIdTagMax
}__attribute__((packed)) ePrivDrvParentIdTag_t;








//--------------------------------------06、A6报文------------------------------
//附录5.3---远程操作指令
typedef enum
{
    ePrivDrvRmtCmdNone                  = 0x00,                                 //无
    ePrivDrvRmtCmdUnlock                = 0x01,                                 //解锁
    ePrivDrvRmtCmdLock                  = 0x02,                                 //上锁
    ePrivDrvRmtCmdStart                 = 0x03,                                 //开始充电
    ePrivDrvRmtCmdStop                  = 0x04,                                 //停止充电
    ePrivDrvRmtCmdReserve               = 0x05,                                 //预约
    ePrivDrvRmtCmdUnreserve             = 0x06,                                 //取消预约
    ePrivDrvRmtCmdAddPower              = 0x07,                                 //加电充电
    
    ePrivDrvRmtCmdMax
}__attribute__((packed)) ePrivDrvRmtCmd_t;



//附录5.7---远程操作结果
typedef enum
{
    ePrivDrvRmtRstSuccess               = 0x00,                                 //成功
    ePrivDrvRmtRstFailCharg             = 0x13,                                 //充电失败
    ePrivDrvRmtRstFailUnlockStop        = 0x31,                                 //解锁失败停机态操作
    ePrivDrvRmtRstFailFault             = 0x56,                                 //桩处于故障态操作失败
    
    ePrivDrvRmtRstMax
}__attribute__((packed)) ePrivDrvRmtRst_t;



//附录5.15:---远程操作失败原因
typedef enum
{
    ePrivDrvRmtFailReasonNull           = 0x00,                                 //空，操作成功时填充
    
    ePrivDrvRmtFailReasonMax
}__attribute__((packed)) ePrivDrvRmtFailReason_t;








//--------------------------------------07、A7报文------------------------------
//操作指令
typedef enum
{
    ePrivDrvStartTransCmdStart          = 0x00,                                 //开始充电
    ePrivDrvStartTransCmdStop           = 0x01,                                 //结束充电
    ePrivDrvStartTransCmdStopInvalidCard= 0x02,                                 //离线无效卡停充(停充原因上报未认证)
    ePrivDrvStartTransCmdUpdateOrderId  = 0x03,                                 //更新订单号
    
    ePrivDrvStartTransCmdMax
}__attribute__((packed)) ePrivDrvStartTransCmd_t;



//启动类型
typedef enum
{
    ePrivDrvStartTransTypeApp               = 0x00,                             //0: App 启动
    ePrivDrvStartTransTypeCard              = 0x01,                             //1: 本地刷卡启动
    ePrivDrvStartTransTypeVin               = 0x02,                             //2: VIN 码启动
    ePrivDrvStartTransTypeAdmin             = 0x03,                             //3: 本地管理员启动
    ePrivDrvStartTransTypeEms               = 0x04,                             //4: EMS 启动
    ePrivDrvStartTransTypeBle               = 0x05,                             //5: 蓝牙启动
    ePrivDrvStartTransTypePnc               = 0x06,                             //6: 即插即用启动
    ePrivDrvStartTransTypeEvccId            = 0x07,                             //7: EVCCID启动
    ePrivDrvStartTransTypeBleRegularTimer   = 0x08,                             //8: 蓝牙定时充电启动
    ePrivDrvStartTransTypeBleNoFeel         = 0x09,                             //9: 蓝牙无感启动
    ePrivDrvStartTransTypeBleNoFeelTimer    = 0x0A,                             //10:蓝牙无感定时启动
    ePrivDrvStartTransTypeBleLock           = 0x0B,                             //11:蓝牙锁启动
    ePrivDrvStartTransTypeServer            = 0x0C,                             //12:平台启动
    
    ePrivDrvStartTransTypeMax
}__attribute__((packed)) ePrivDrvStartTransType_t;








//--------------------------------------08、A8报文------------------------------
//cmd_08上报时机
typedef enum
{
    ePrivDrvRtDataRptTypeCharging       = 0x00,                                 //充电中上报的电表数据
    ePrivDrvRtDataRptTypeClock          = 0x01,                                 //时钟对齐时的电表数据（OCPP中应用到）
    
    ePrivDrvRtDataRptTypeMax
}__attribute__((packed)) ePrivDrvRtDataRptType_t;








//--------------------------------------09、A9报文------------------------------
//附录5.6---停充原因定义
typedef enum
{
    ePrivDrvStopReasonDefault           = 0,                                    //默认
    ePrivDrvStopReasonApp               = 1,                                    //APP停充
    ePrivDrvStopReasonPlugout           = 2,                                    //拔枪停充
    ePrivDrvStopReasonFull              = 3,                                    //充满
    ePrivDrvStopReasonTime              = 4,                                    //按时间充,时间到
    ePrivDrvStopReasonElect             = 5,                                    //按电量充, 电量到
    ePrivDrvStopReasonCard              = 6,                                    //刷卡停充
    ePrivDrvStopReasonAccountBalance    = 7,                                    //余额不足被服务器停充
    ePrivDrvStopReasonFault             = 8,                                    //故障
    ePrivDrvStopReasonCar               = 9,                                    //车端主动停(直流)
    ePrivDrvStopReasonEmgShutdown       = 10,                                   //急停按下
    ePrivDrvStopReasonHwReset           = 11,                                   //硬件复位
    ePrivDrvStopReasonReset             = 12,                                   //重启
    ePrivDrvStopReasonResetBySrv        = 13,                                   //收到服务器的复位指令停充
    ePrivDrvStopReasonUnlockBySrv       = 14,                                   //收到服务器的解锁枪指令停充
    ePrivDrvStopReasonGunLockAbnormal   = 15,                                   //枪锁异常
    ePrivDrvStopReasonInvalidCard       = 16,                                   //未认证的卡号
    ePrivDrvStopReasonPwrErr            = 17,                                   //异常断电
    ePrivDrvStopReasonAccount           = 18,                                   //按金额充,金额到
    ePrivDrvStopReasonVinMismatch       = 19,                                   //VIN码不匹配
    ePrivDrvStopReasonCarRunning        = 20,                                   //车未熄火
    ePrivDrvStopReasonDoorOpen          = 21,                                   //门禁打开

    
    ePrivDrvStopReasonMax
}__attribute__((packed)) ePrivDrvStopReason_t;








//--------------------------------------0A、AA报文------------------------------
//操作类型
typedef enum
{
    ePrivDrvCfgCmdTypeGet               = 0,                                    //查询
    ePrivDrvCfgCmdTypeSet               = 1,                                    //设置
    
    ePrivDrvCfgCmdTypeChargGet          = 0xFE,                                 //桩查询(需要回复结果)
    ePrivDrvCfgCmdTypeChargSet          = 0xFF                                  //桩设置(需要回复结果)
}__attribute__((packed)) ePrivDrvCfgCmdType_t;



//参数地址表及其数据结构定义 引用<private_drv_para.h>








//--------------------------------------0B、AB报文------------------------------
//--------------------------------------0C、AC报文------------------------------
#define cPrioDrvLenUpdateData           400                                     //升级包数据长度
#define cPrioDrvUpdateLineMax           20                                      //模块电源升级包单包行数最大值

//操作命令
typedef enum
{
    ePrivDrvUpdateCmdRebootApp          = 0,                                    //复位命令. 进入应用程序模式
    ePrivDrvUpdateCmdHandshake          = 1,                                    //握手
    ePrivDrvUpdateCmdErase              = 2,                                    //flash擦除指令
    ePrivDrvUpdateCmdFlash              = 3,                                    //flash写命令
    ePrivDrvUpdateCmdVerify             = 4,                                    //flash验证
    ePrivDrvUpdateCmdRebootBoot         = 5,                                    //复位命令,进入boot模式
    
    ePrivDrvUpdateCmdMax                = 0xFFFF                                //确保占用2个字节
}__attribute__((packed)) ePrivDrvUpdateCmd_t;

//操作结果
typedef enum
{
    ePrivDrvUpdateCmdRstSuccess         = 0,                                    //成功
    ePrivDrvUpdateCmdRstFail            = 1,                                    //失败
    
    ePrivDrvUpdateCmdRstMax             = 0xFFFF                                //确保占用2个字节
}__attribute__((packed)) ePrivDrvUpdateCmdRst_t;








//--------------------------------------0D、AD报文------------------------------
#define cPrioDrvLenLogData              512                                     //桩端日志信息最大长度








//--------------------------------------0E、AE报文------------------------------
//三相数据
typedef struct 
{
    i16                                 i16DataA;                               //A相值
    i16                                 i16DataB;                               //B相值
    i16                                 i16DataC;                               //C相值
    
}__attribute__((packed)) stPrivDrvPhaseData_t;

//三相电流数据
typedef struct 
{
    i32                                 i32DataA;                               //A相值
    i32                                 i32DataB;                               //B相值
    i32                                 i32DataC;                               //C相值
    
}__attribute__((packed)) stPrivDrvPhaseDataI_t;



//三相带N点数据
typedef struct 
{
    i16                                 i16DataA;                               //A相值
    i16                                 i16DataB;                               //B相值
    i16                                 i16DataC;                               //C相值
    i16                                 i16DataN;                               //N点值
    
}__attribute__((packed)) stPrivDrvPhaseDataN_t;



//桩温度数据
typedef struct 
{
    i16                                 i16RealayL;                             //继电器温度L
    i16                                 i16Chg;                                 //充电桩温度
    i16                                 i16InPortL;                             //输入端子温度L温度
    i16                                 i16InPortN;                             //输入端子温度N温度
    i16                                 i16LeakCurr;                            //漏电流温度
    i16                                 i16RealayN;                             //继电器温度N
    i16                                 i16Data2;                               //预留温度2
    i16                                 i16Data3;                               //预留温度3
    i16                                 i16Data4;                               //预留温度4
    i16                                 i16Data5;                               //预留温度5
}__attribute__((packed)) stPrivDrvChgTempData_t;



//枪温度数据
typedef struct 
{
    i16                                 i16Gun;                                 //充电枪温度
    i16                                 i16Data1;                               //预留温度1
    i16                                 i16Data2;                               //预留温度2
    i16                                 i16Data3;                               //预留温度3
    i16                                 i16Data4;                               //预留温度4
    i16                                 i16Data5;                               //预留温度5
    
}__attribute__((packed)) stPrivDrvGunTempData_t;







//--------------------------------------11、B1报文------------------------------
#define  cPrioDrvLenSecurityEvt         40                                      //预留长度
#define  cPrioDrvLenSecurityEvtAck      20                                      //预留应答长度

typedef enum
{
    eDismantleEventDis                  = 0,                                    //未发生
    eDismantleEventEn,                                                          //发生
}__attribute__((packed)) eDismantleEvente_t;

typedef enum
{
    eRebootNone                         = 0,                                    //无
    eRebootFromPowerDown,                                                       //掉电
    eRebootFromSoftReset,                                                       //软件复位重启
    eRebootFromHardReset,                                                       //硬件复位重启
    eRebootFromSoftBug,                                                         //软件Bug导致重启
}__attribute__((packed))eRebootEvente_t;

typedef enum
{
    eSecurityEvtRstSuccess              = 0,                                    //成功
    eSecurityEvtRstFail,                                                        //失败
}__attribute__((packed)) eSecurityEvtRst_t;








//--------------------------------------13、B3报文------------------------------
#define cPrioDrvLenBmsVer               3                                       //BRM-BMS通讯协议版本号长度
#define cPrioDrvLenManuName             4                                       //电池生产厂商长度
#define cPrioDrvLenCmd13Rsv1            4                                       //预留1长度
#define cPrioDrvLenVin                  17                                      //车辆识别码vin长度
#define cPrioDrvLenBmsSwVer             8                                       //BMS软件版本长度
#define cPrioDrvLenModuleSn             16                                      //模块长度
#define cPrioDrvLenModuleFault          6                                       //模块故障长度
#define cPrioDrvLenCmd13Rsv3            4                                       //预留3长度

typedef enum
{
    ePrioDrvBatTypeTypeNo               = 0,                                    //无
    ePrioDrvBatTypeTypeLa               = 1,                                    //铅酸电池
    ePrioDrvBatTypeTypeNimh             = 2,                                    //镍氢电池
    ePrioDrvBatTypeTypeLfp              = 3,                                    //磷酸铁锂电池
    ePrioDrvBatTypeTypeLiMn             = 4,                                    //锰酸锂电池
    ePrioDrvBatTypeTypeLco              = 5,                                    //钴酸锂电池
    ePrioDrvBatTypeTypeTli              = 6,                                    //三元材料电池
    ePrioDrvBatTypeTypeLiPo             = 7,                                    //聚合物锂离子电池
    ePrioDrvBatTypeTypeLto              = 8,                                    //钛酸锂电池
    ePrioDrvBatTypeTypeOther            = 0xFF,                                 //其他电池
}__attribute__((packed))ePrioDrvBatType_t;








//--------------------------------------14、B4报文-----------------
typedef enum
{
    ePrivDrvAppointTypeGet              = 0x00,                                 //查询
    ePrivDrvAppointTypeSet,                                                     //设置
    ePrivDrvAppointTypeCancel,                                                  //取消
    
    ePrivDrvAppointTypeMax
}__attribute__((packed)) ePrivDrvAppointType_t;
    
typedef enum
{
    ePrivDrvAppointStsUnAppoint         = 0x00,                                 //未处于预约
    ePrivDrvAppointStsAppointed,                                                //预约中
    
    ePrivDrvAppointStsMax
}__attribute__((packed)) ePrivDrvAppointSts_t;


typedef enum
{
    ePrivDrvAppointModeTimer            = 0x00,                                 //按时间充
    ePrivDrvAppointModeFull,
    
    ePrivDrvAppointModeMax
}__attribute__((packed)) ePrivDrvAppointMode_t;


typedef enum
{
    ePrivDrvAppointFlgOne               = 0x00,                                 //一次有效
    ePrivDrvAppointFlgEveryDay,                                                 //每天生效
    
    ePrivDrvAppointFlgMax
}__attribute__((packed)) ePrivDrvAppointFlg_t;

typedef enum
{
    ePrivDrvAppointRsnSuccess            = 0x00,                                //成功
    ePrivDrvAppointRsnUnAvailable        = 0x13,                                //桩禁用
    ePrivDrvAppointRsnFault              = 0x56,                                //桩故障
    ePrivDrvAppointRsnStop               = 0x57,                                //桩停充
    ePrivDrvAppointRsnInit               = 0x58,                                //桩停充
    ePrivDrvAppointRsnCheck              = 0x59,                                //桩自检
    ePrivDrvAppointRsnCharging           = 0x5A,                                //桩充电中
    ePrivDrvAppointRsnAppoint            = 0x5B,                                //预约状态
    ePrivDrvAppointRsnIdle               = 0x5C,                                //空闲态,未插枪
    ePrivDrvAppointRsnModeSw             = 0x5D,                                //模式切换
    ePrivDrvAppointRsnPreparing          = 0x5E,                                //拔枪-退出预约
    ePrivDrvAppointRsnCancel             = 0x5F,                                //手动取消
    ePrivDrvAppointRsnFull               = 0x60,                                //充满-退出预约
    
    
    ePrivDrvAppointRsnMax
}__attribute__((packed)) ePrivDrvAppointRsn_t;








//--------------------------------------15、B5报文-----------------
//主柜数据标志
typedef struct
{
    u8                                  bPcu  : 1;                              //PCU 数据标志 0---无数据， 1---有数据
    u8                                  bMod  : 1;                              //模块数据标志
    u8                                  bScu  : 1;                              //SCU 数据标志
    u8                                  bRsvd : 5;                              //预留
    
}__attribute__((packed)) stPrivDrvMpcDataFlag_t;




//PCU数据--->主柜状态
typedef struct
{
    u32                                 bDoor : 1;                              //门禁         0：关闭  1：触发
    u32                                 bEmg  : 1;                              //急停
    u32                                 bFan  : 1;                              //风扇
    u32                                 bRsvd : 29;                             //预留
    
}__attribute__((packed)) stPrivDrvMpcStatus_t;

//PCU数据
typedef struct
{
    stPrivDrvMpcStatus_t                stMpcSts;                               //主柜状态
    i8                                  i8TairIn;                               //主柜进风口温度---1 ℃
    i8                                  i8TairOut;                              //主柜出风口温度---1 ℃
    
}__attribute__((packed)) stPrivDrvPcuData_t;




//模块数据--->模块状态标志
typedef struct
{
    u8                                  bOnline : 1;                            //在线状态     0：离线  1：在线
    u8                                  bOutput : 1;                            //开机状态     0：关机  1：开机
    u8                                  bFaulted: 1;                            //故障状态     0：正常  1：故障
    u8                                  bRsvd   : 5;                            //预留
    
}__attribute__((packed)) stPrivDrvModStatus_t;

//模块的最大个数
#define cPrivDrvModNumMax               (32)
#define cPrivDrvModNameLen              (3)

//单个模块
typedef struct
{
    stPrivDrvModStatus_t                stModSts;                               //模块状态
    i8                                  i8Tmod;                                 //模块温度---1 ℃
    u16                                 u16UdcSet;                              //模块设置电压---单位：1V
    u16                                 u16IdcSet;                              //模块设置电流---单位：1A
    u16                                 u16UdcOut;                              //模块输出电压---单位：1V
    u16                                 u16IdcOut;                              //模块输出电流---单位：1A
    char                                u8GunName[cPrivDrvModNameLen];          //模块所绑定的枪名字 1A/1B, 5A/5B...
}__attribute__((packed)) stPrivDrvMod_t;

//模块数据
typedef struct
{
    u8                                  u8ModNum;                               //模块个数
    stPrivDrvMod_t                      stMod[cPrivDrvModNumMax];               //单个模块数据
    
}__attribute__((packed)) stPrivDrvModData_t;




//SCU的最大个数
#define cPrivDrvScuNumMax               (32)

//单个SCU(32路)继电器状态
typedef struct
{
    u32                                 u32WorkSts;                             //Relay1-32工作状态---0 - 断开    1 - 闭合
    u32                                 u32FaultSts;                            //Relay1-32故障状态---0 - 正常    1 - 故障
    
}__attribute__((packed)) stPrivDrvScuRelaySts_t;

//模块数据
typedef struct
{
    u8                                  u8ScuNum;                               //SCU 个数
    u32                                 u32OnlineSts;                           //SCU1-32在线状态---0 - 离线    1 - 在线
    stPrivDrvScuRelaySts_t              stRelaySts[cPrivDrvScuNumMax];          //SCU1-32板载继电器状态
}__attribute__((packed)) stPrivDrvScuData_t;









//--------------------------------------16、B6报文-----------------

//液冷机告警状态
typedef struct
{
    u16                                 bPressHigh  : 1;                        //压力高
    u16                                 bPressLow   : 1;                        //压力低
    u16                                 bRsvd1      : 4;                        //预留
    u16                                 bLiquidLow  : 1;                        //液位低
    u16                                 bIpumpHigh  : 1;                        //循环泵过流---pump--->泵
    u16                                 bPpumpLow   : 1;                        //循环泵欠压
    u16                                 bPpumpHigh  : 1;                        //循环泵过压
    u16                                 bTempHigh   : 1;                        //过温
    u16                                 bPpumpStall : 1;                        //循环泵堵转
    u16                                 bRsvd2      : 3;                        //预留
    u16                                 bIfanHigh   : 1;                        //风机过流
    
}__attribute__((packed)) stPrivDrvCoolingStatus_t;





















































//--------------------------------------费率模型--------------------------------
#define cPrivDrvRateSection             48                                      //48段

//费率模型
typedef enum
{
    ePrivDrvRateModelDivEqually         = 0x00,                                 //平均分段  模型
    ePrivDrvRateModelTopDown1           = 0x01,                                 //尖峰平谷1 模型
    ePrivDrvRateModelTopDown2           = 0x02,                                 //尖峰平谷2 模型
    ePrivDrvRateModelUserDef            = 0x03,                                 //用户自定义模型
    
    ePrivDrvRateModelMax
}__attribute__((packed)) ePrivDrvRateModel_t;



//服务费、占桩费、预约费的模式
typedef enum
{
    ePrivDrvFeeModeFix                  = 0x00,                                 //固定一个费率
    ePrivDrvFeeModeSection              = 0x01,                                 //与分段时间一致
    
    ePrivDrvFeeModeMax
}__attribute__((packed)) ePrivDrvFeeMode_t;








//--------------------------------------平均分段模型----------------------------
//平均分段模型
typedef struct 
{
    ePrivDrvRateModel_t                 eMode;                                  //费率模型
    ePrivDrvFeeMode_t                   eModeCover;                             //服务费模式
    u32                                 u32RateCoverFix;                        //固定的服务费价格----------单位：0.00001元
    u8                                  u8Section;                              //分段段数------------------最多48段
    u32                                 u32RateElect[cPrivDrvRateSection];      //电  费价格----------------单位：0.00001元
    u32                                 u32RateCover[cPrivDrvRateSection];      //服务费价格----------------单位：0.00001元
    
}__attribute__((packed)) stPrivDrvRateDiv_t;








//--------------------------------------尖峰平谷模型----------------------------
#define cPrivDrvRateTimeSection         12                                      //尖峰平谷模型最大时段数
#define cPrivDrvRateModelIdLen          32                                      //费率模型id长度



//尖峰平谷类型定义
typedef enum
{
    ePrivDrvRateTypeSharp               = 0x00,                                 //尖
    ePrivDrvRateTypePeak                = 0x01,                                 //峰
    ePrivDrvRateTypeFlat                = 0x02,                                 //平
    ePrivDrvRateTypeValley              = 0x03,                                 //谷
    
    ePrivDrvRateTypeMax
}__attribute__((packed)) ePrivDrvRateType_t;



//跨天分段标志定义
typedef enum
{
    ePrivDrvRateCrossDayNoMerge         = 0x00,                                 //不合并
    ePrivDrvRateCrossDayMerge           = 0x01,                                 //合并
    
    ePrivDrvRateCrossDayMax
}__attribute__((packed)) ePrivDrvRateCrossDay_t;



//尖峰平谷模型--->公用的 head 结构体定义, 价格信息
typedef struct 
{
    ePrivDrvRateModel_t                 eMode;                                  //费率模型
    ePrivDrvFeeMode_t                   eModeCover;                             //服务费模式
    ePrivDrvFeeMode_t                   eModeParking;                           //占桩费模式
    ePrivDrvFeeMode_t                   eModeBooking;                           //预约费模式
    u32                                 u32RateCoverFix;                        //固定的服务费价格----------单位：0.00001元
    u32                                 u32RateParkingFix;                      //固定的服务费价格----------单位：0.00001元
    u32                                 u32RateBookingFix;                      //固定的服务费价格----------单位：0.00001元
    u32                                 u32RateElect[ePrivDrvRateTypeMax];      //电  费价格----------------单位：0.00001元
    u32                                 u32RateCover[ePrivDrvRateTypeMax];      //服务费价格----------------单位：0.00001元
    u32                                 u32RateParking[ePrivDrvRateTypeMax];    //占桩费价格----------------单位：0.00001元
    u32                                 u32RateBooking[ePrivDrvRateTypeMax];    //预约费价格----------------单位：0.00001元
}__attribute__((packed)) stPrivDrvRateTopDownHead_t;



//尖峰平谷1模型--->单个时段数据结构定义
typedef struct 
{
    u8                                  u8Hour;                                 //起点时间 hour
    u8                                  u8Minute;                               //起点时间 minute
    ePrivDrvRateType_t                  eElectType;                             //电  费价格
    ePrivDrvRateType_t                  eCoverType;                             //服务费价格
    ePrivDrvRateType_t                  eParkingType;                           //占桩费价格
    ePrivDrvRateType_t                  eBookingType;                           //预约费价格
    
}__attribute__((packed)) stPrivDrvRateTopDownUnit_t;



//尖峰平谷1模型
typedef struct 
{
    stPrivDrvRateTopDownHead_t          stHead;                                 //尖峰平谷模型--->价格信息
    u8                                  u8RateModelId[cPrivDrvRateModelIdLen];  //费率模型编号
    ePrivDrvRateCrossDay_t              eCrossDay;                              //跨天分段标志
    u8                                  u8Section;                              //分段段数------------------最多12段
    stPrivDrvRateTopDownUnit_t          stUint[cPrivDrvRateTimeSection];        //分段费率
}__attribute__((packed)) stPrivDrvRateTopDown1_t;



//尖峰平谷2模型
typedef struct 
{
    stPrivDrvRateTopDownHead_t          stHead;                                 //尖峰平谷模型--->价格信息
    u8                                  u8RateModelId[cPrivDrvRateModelIdLen];  //费率模型编号
    ePrivDrvRateType_t                  eRateType[cPrivDrvRateSection];         //分段费率
}__attribute__((packed)) stPrivDrvRateTopDown2_t;








//--------------------------------------用户自定义模型--------------------------
#ifndef cSdkPrivDrvRateUserDefineLen
#define cPrivDrvRateUserDefineLen       (1)
#else
#define cPrivDrvRateUserDefineLen       (cSdkPrivDrvRateUserDefineLen)
#endif



//用户自定义模型
typedef struct 
{
    ePrivDrvRateModel_t                 eMode;                                  //费率模型
    u8                                  u8Data[cPrivDrvRateUserDefineLen];      //自定义数据格式
}__attribute__((packed)) stPrivDrvRateUserDef_t;



//附录5.8--->费率模型
typedef union 
{
    stPrivDrvRateDiv_t                  stDiv;                                  //平均分段  模型
    stPrivDrvRateTopDown1_t             stTopDown1;                             //尖峰平谷1 模型
    stPrivDrvRateTopDown2_t             stTopDown2;                             //尖峰平谷2 模型
    stPrivDrvRateUserDef_t              stUserDef;                              //用户自定义模型
}__attribute__((packed)) unPrivDrvRate_t;








//--------------------------------------电量明细数据----------------------------
//--------------------------------------平均分段明细----------------------------
//平均分段  模型 数据格式
typedef struct 
{
    ePrivDrvRateModel_t                 eMode;                                  //费率模型
    ePrivDrvFeeMode_t                   eModeCover;                             //服务费模式
    u32                                 u32MoneyCoverFix;                       //固定的服务费--------------单位：0.01元
    u8                                  u8Section;                              //分段段数------------------最多48段
    u32                                 u32Elect[cPrivDrvRateSection];          //分段电  量明细------------单位：1wh
    u32                                 u32MoneyElect[cPrivDrvRateSection];     //分段电  费明细------------单位：0.01元
    u32                                 u32MoneyCover[cPrivDrvRateSection];     //分段服务费明细------------单位：0.01元
    
}__attribute__((packed)) stPrivDrvMoneyDiv_t;



//尖峰平谷1模型 数据格式--->单个时段的明细数据
typedef struct 
{
    ePrivDrvRateType_t                  eType;                                  //分段类型
    u8                                  u8StartDay;                             //起点时间 day
    u8                                  u8StartHour;                            //起点时间 hour
    u8                                  u8StartMin;                             //起点时间 minute
    u8                                  u8EndDay;                               //结束时间 day
    u8                                  u8EndHour;                              //结束时间 hour
    u8                                  u8EndMin;                               //结束时间 minute
    u32                                 u32Elect;                               //分段电  量明细------------单位：1wh
    u32                                 u32MoneyElect;                          //分段电  费明细------------单位：0.01元
    u32                                 u32MoneyCover;                          //分段服务费明细------------单位：0.01元
    u32                                 u32MoneyParking;                        //分段占桩费明细------------单位：0.01元
    u32                                 u32MoneyBooking;                        //分段预约费明细------------单位：0.01元
    
}__attribute__((packed)) stPrivDrvMoneyTopDownUnit;



//尖峰平谷1模型 数据格式
typedef struct 
{
    ePrivDrvRateModel_t                 eMode;                                  //费率模型
    ePrivDrvFeeMode_t                   eModeCover;                             //服务费模式
    ePrivDrvFeeMode_t                   eModeParking;                           //占桩费模式
    ePrivDrvFeeMode_t                   eModeBooking;                           //预约费模式
    u32                                 u32MoneyCoverFix;                       //固定的服务费--------------单位：0.01元
    u32                                 u32MoneyParkingFix;                     //固定的占桩费--------------单位：0.01元
    u32                                 u32MoneyBookingFix;                     //固定的预约费--------------单位：0.01元
    u8                                  u8Section;                              //分段段数------------------最多12段
    stPrivDrvMoneyTopDownUnit           stUnit[cPrivDrvRateTimeSection];        //分段明细数据
    
}__attribute__((packed)) stPrivDrvMoneyTopDown1_t;



//尖峰平谷2模型 数据格式
typedef struct 
{
    ePrivDrvRateModel_t                 eMode;                                  //费率模型
    ePrivDrvFeeMode_t                   eModeCover;                             //服务费模式
    ePrivDrvFeeMode_t                   eModeParking;                           //占桩费模式
    ePrivDrvFeeMode_t                   eModeBooking;                           //预约费模式
    u32                                 u32MoneyCoverFix;                       //固定的服务费--------------单位：0.01元
    u32                                 u32MoneyParkingFix;                     //固定的占桩费--------------单位：0.01元
    u32                                 u32MoneyBookingFix;                     //固定的预约费--------------单位：0.01元
    u8                                  u8Section;                              //分段段数------------------最多48段
    u32                                 u32Elect[ePrivDrvRateTypeMax];          //尖峰平谷电  量------------单位：1wh
    u32                                 u32MoneyElect[ePrivDrvRateTypeMax];     //尖峰平谷电  费------------单位：0.01元
    u32                                 u32MoneyCover[ePrivDrvRateTypeMax];     //尖峰平谷服务费------------单位：0.01元
    u32                                 u32MoneyParking[ePrivDrvRateTypeMax];   //尖峰平谷占桩费------------单位：0.01元
    u32                                 u32MoneyBooking[ePrivDrvRateTypeMax];   //尖峰平谷预约费------------单位：0.01元
    u8                                  u8SegType[cPrivDrvRateSection];         //时段费率标志--------------单位：0-3对应尖峰平谷
    u32                                 u32SegElect[cPrivDrvRateSection];       //48个段的电量--------------单位：1wh
    
}__attribute__((packed)) stPrivDrvMoneyTopDown2_t;








//--------------------------------------用户自定义模型 数据格式-----------------
#define cPrivDrvMoneyUserDefineLen      512                                     //自定义数据格式长度



//用户自定义模型 数据格式
typedef struct 
{
    ePrivDrvRateModel_t                 eMode;                                  //费率模型
    u8                                  u8Data[cPrivDrvMoneyUserDefineLen];     //自定义数据格式
}__attribute__((packed)) stPrivDrvMoneyUserDef_t;



//附录5.9--->分段电量及费用格式
typedef union 
{
    stPrivDrvMoneyDiv_t                 stDiv;                                  //平均分段  模型 数据格式
    stPrivDrvMoneyTopDown1_t            stTopDown1;                             //尖峰平谷1 模型 数据格式
    stPrivDrvMoneyTopDown2_t            stTopDown2;                             //尖峰平谷2 模型 数据格式
    stPrivDrvMoneyUserDef_t             stUserDef;                              //用户自定义模型 数据格式
}__attribute__((packed)) unPrivDrvMoney_t;









#endif




