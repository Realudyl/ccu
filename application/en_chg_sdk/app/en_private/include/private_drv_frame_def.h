/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_frame_def.h
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-10-13
* notice                                :     本文件定义协议中所有的帧结构：报文头，各种报文的payload，结构体或联合 等等
****************************************************************************************************/
#ifndef _private_drv_frame_def_H_
#define _private_drv_frame_def_H_



#include "en_common.h"
#include "private_drv_para.h"
#include "private_drv_data_def.h"













//--------------------------------------报文头----------------------------------
//报文帧头格式定义
typedef struct
{
    ePrivDrvHeader_t                    eHeader;                                //报文头
    ePrivDrvVersion_t                   eVersion;                               //版本
    u16                                 u16Addr;                                //桩地址
    ePrivDrvCmd_t                       eCmd;                                   //报文命令类型
    u8                                  u8Seqno;                                //报文序列号
    u16                                 u16Len;                                 //数据域长度
}__attribute__((packed)) stPrivDrvHead_t;
#define cPrivDrvHeadSize                sizeof(stPrivDrvHead_t)                 //报文head长度
#define cPrivDrvCrcSize                 2                                       //报文校验码长度







//--------------------------------------01、A1报文------------------------------
typedef struct 
{
    u16                                 u16SnBcd;                               //SN号最后4个有效数字的BCD码
    u8                                  u8Rsvd[cPrivDrv01RsvdLen];              //预留
}__attribute__((packed)) stPrivDrvCmd01_t;



typedef struct 
{
    ePrivDrvA1Cmd_t                     eCmd;                                   //命令类型
    u16                                 u16SnBcd;                               //SN号最后4个有效数字的BCD码
    u8                                  u8Addr;                                 //桩地址 2--255
    u8                                  u8Rsvd[cPrivDrvA1RsvdLen];              //预留
}__attribute__((packed)) stPrivDrvCmdA1_t;








//--------------------------------------02、A2报文------------------------------
//--------------------------------------内网3.0协议参数定义
typedef struct 
{
    char                                u8Sn[32];
    u8                                  u8DevCode;
    u16                                 u8GunNum        :4;                     //枪口数
    ePrivDrvChgType_t                   eChgType        :2;                     //交直流   0:ac   1:3phase ac   3:dc
    ePrivDrvRcdTiming_t                 eRcdTiming      :1;                     //结算时机 0:pull out gun     1:stop charging
    ePrivDrvBillSide_t                  eBillSide       :1;                     //计费端   0:charger          1:center system
    ePrivDrvDisable_t                   eDisable        :1;                     //桩禁用   0:enable           1:disabled
    ePrivDrvStartPower_t                eStartPower     :1;                     //功率分配启动模式
    ePrivDrvChgWorkMode_t               eChgWorkMode    :2;                     //桩工作模式
    ePrivDrvLoadPhase_t                 eLoadPhase      :2;                     //三相平衡挂载相位
    u16                                 u16Rsv          :2;                     //预留
    char                                u8DevName[24];
    char                                u8HwVersion[24];
    char                                u8SwVer[24];
    unPrivDrvWorkSts_t                  unWorkSts[4];                           //4只枪工作状态 see 5.2  
    u8                                  u8GunStatus;                            //枪1-4连接状态,2bits一个 eGunStatus_t 0:gunout_12v    1:gunin_9v   2:gunin_6v  3:undef
    u8                                  u8GuLockStatus;                         //枪1-4锁状态,2bits一个   eGunLock_t   0:none lock     1:closed     2:opened    3:unknown
    u16                                 u16WorkDataRptTime;                     //工作数据上报时间间隔  默认90s, 可配置为60-500s
    u16                                 u16HeartbeatRptTime;                    //心跳包时间间隔   默认120s,可配置为30-1000s
    u32                                 u32MaxPower;                            //最大功率限制    1W
    u32                                 u32MCurr;                               //最大电流限制  0.1A
}__attribute__((packed)) stPrivDrvCmd0230_t;



typedef struct 
{
    ePrivDrvCmdRst_t                    eRst;                                   //执行结果--->签到状态
}__attribute__((packed)) stPrivDrvCmdA230_t;




//--------------------------------------内网3.6协议参数定义
typedef struct 
{
    ePrivDrvChargerType_t               eType;                                  //充电桩类型
    ePrivDrvPayAt_t                     ePayAt;                                 //结算时机
    ePrivDrvMoneyCalcBy_t               eMoneyCalcBy;                           //计费端
    ePrivDrvChargerDisableFlag_t        eDisableFlag;                           //桩禁用
    ePrivDrvStartPowerMode_t            ePowerMode;                             //功率分配启动模式
    ePrivDrvChargerWorkMode_t           eWorkMode;                              //桩工作模式
    ePrivDrvBalancePhase_t              ePhase;                                 //三相平衡挂载相位
    i32                                 i32PowerMax;                            //最大功率              单位:1W
    i32                                 i32CurrMax;                             //最大电流              单位:0.1A
    i32                                 i32PowerRated;                          //额定功率              单位:1W
    i32                                 i32CurrRated;                           //额定电流              单位:0.1A
    ePrivDrvBookingViaBle_t             eBookingViaBle;                         //是否支持蓝牙预约
    ePrivDrvLoginedFlag_t               eLoginedFlag;                           //登录服务器标志
    ePrivDrvRebootReason_t              eReason;                                //重启原因
    u8                                  u8Sn[cPrivDrvLenSn];                    //桩SN号
    u8                                  u8DevCode[cPrivDrvLenDevCode];          //设备型号代码
    u8                                  u8DevName[cPrivDrvLenDevName];          //设备名称
    u8                                  u8HwVer[cPrivDrvLenHwVer];              //硬件版本号
    u8                                  u8SwVer[cPrivDrvLenSwVer];              //软件版本号
    u8                                  u8GunNum;                               //实际的枪口数----------部分工程下多个机型的枪口数不一样
    stPrivDrvGunStatus_t                stGunStatus;                            //枪状态数据
    u8                                  u8Data[cPrivDrvRsvdLen02];              //预留数据
    
}__attribute__((packed)) stPrivDrvCmd02_t;



typedef struct 
{
    ePrivDrvCmdRst_t                    eRst;                                   //执行结果--->签到状态
    ePrivDrvLoginFailReason_t           eReason;                                //签到失败原因
    u32                                 u32Time;                                //重复签到间隔  失败情况下，下一次签到间隔。单位：S
    ePrivDrvNetHwType_t                 eHwType;                                //联网类型
    u8                                  u8Ver[cPrivDrvGwVerLen];                //网关版本号
}__attribute__((packed)) stPrivDrvCmdA2_t;








//--------------------------------------03、A3报文------------------------------
typedef struct 
{
    u8                                  u8GunId;                                //枪号
    ePrivDrvGunConnStatus_t             eGunConnStatus;                         //枪连接状态
    ePrivDrvGunEvent_t                  eGunevent;                              //枪事件
    ePrivDrvGunWorkStatus_t             eGunWorkStatus;                         //枪工作状态当前值
    ePrivDrvGunWorkStatus_t             eGunWorkStatusOld;                      //枪工作状态上一次值
    ePrivDrvGunWorkStsRsn_t             eReason;                                //枪工作状态变化原因
    unPrivDrvFaultByte_t                unFaultByte;                            //故障编码----旧的故障位
    unPrivDrvWarningByte_t              unWarningByte;                          //告警编码----旧的告警位
    stTime_t                            stTime;                                 //当前时间
    stPrivDrvFaultCode_t                stFaultCode;                            //故障编码----新的故障告警编码列表
    
}__attribute__((packed)) stPrivDrvCmd03_t;



typedef struct 
{
    u8                                  u8GunId;                                //枪号
    ePrivDrvNetStatus_t                 eNetStatus;                             //桩连网状态
    stTime_t                            stTime;                                 //当前时间
    u8                                  u8Data[cPrivDrvRsvdLenA3];              //预留数据
    
}__attribute__((packed)) stPrivDrvCmdA3_t;








//--------------------------------------04、A4报文------------------------------
typedef struct 
{
    u8                                  u8GunNum;                               //实际的枪口数----------部分工程下多个机型的枪口数不一样
    stPrivDrvGunStatus_t                stGunStatus;                            //枪状态数据
}__attribute__((packed)) stPrivDrvCmd04_t;



typedef struct 
{
    ePrivDrvNetStatus_t                 eNetStatus;                             //桩连网状态
    stTime_t                            stTime;                                 //当前时间
    i8                                  i8Rssi;                                 //信号值
}__attribute__((packed)) stPrivDrvCmdA4_t;








//--------------------------------------05、A5报文------------------------------
typedef struct 
{
    u8                                  u8GunId;                                //枪号
    ePrivDrvCardType_t                  eType;                                  //卡类型
    ePrivDrvCardLockStatus_t            eStatus;                                //锁卡状态
    u32                                 u32CardBalance;                         //卡内余额
    stTime_t                            stTime;                                 //读卡时间---(用于后台去重)
    ePrivDrvChargingMode_t              eMode;                                  //充电模式
    unPrivDrvChargingParam_t            unParam;                                //充电参数
    u8                                  u8UserId[cPrivDrvUserIdLen];            //用户ID
    u8                                  u8Data[cPrivDrvRsvdLen05];              //预留数据
    
}__attribute__((packed)) stPrivDrvCmd05_t;



typedef struct 
{
    u8                                  u8GunId;                                //枪号
    ePrivDrvCardAccountStatus_t         eAccountStatus;                         //卡账户状态
    ePrivDrvCardBalanceStatus_t         eBalanceStatus;                         //卡余额状态
    ePrivDrvCardAuthFailRsn_t           eFailReason;                            //认证失败原因
    u32                                 u32AccountBalance;                      //账户余额--0.01元
    u8                                  u8UserId[cPrivDrvUserIdLen];            //用户ID
    u8                                  u8SrvTradeId[cPrivDrvSrvTradeIdLen];    //平台交易流水号
    ePrivDrvParentIdTag_t               eParentIdTag;                           //parentIdTag标志
    u8                                  u8ParentId[cPrivDrvParentIdLen];        //parentIdTag
    u8                                  u8Data[cPrivDrvRsvdLenA5];              //预留数据
    
}__attribute__((packed)) stPrivDrvCmdA5_t;








//--------------------------------------06、A6报文------------------------------
typedef struct 
{
    u8                                  u8GunId;                                //枪号
    ePrivDrvRmtCmd_t                    eCmd;                                   //操作指令
    ePrivDrvRmtRst_t                    eRst;                                   //操作结果
    ePrivDrvRmtFailReason_t             eReason;                                //操作失败原因
    u8                                  u8OrderId[cPrivDrvOrderIdLen];          //订单编号
    u8                                  u8SrvTradeId[cPrivDrvSrvTradeIdLen];    //平台交易流水号
    u8                                  u8DevTradeId[cPrivDrvDevTradeIdLen];    //设备交易流水号
    u8                                  u8Data[cPrivDrvRsvdLen06];              //预留数据
    
}__attribute__((packed)) stPrivDrvCmd06_t;



typedef struct 
{
    u8                                  u8GunId;                                //枪号
    ePrivDrvRmtCmd_t                    eCmd;                                   //操作指令
    ePrivDrvChargingMode_t              eMode;                                  //充电模式
    unPrivDrvChargingParam_t            unParam;                                //充电参数
    u32                                 u32AccountBalance;                      //账户余额
    ePrivDrvStartTransType_t            eType;                                  //启动类型
    u8                                  u8UserId[cPrivDrvUserIdLen];            //用户ID
    u8                                  u8OrderId[cPrivDrvOrderIdLen];          //订单编号
    u8                                  u8SrvTradeId[cPrivDrvSrvTradeIdLen];    //平台交易流水号
    ePrivDrvParentIdTag_t               eParentIdTag;                           //parentIdTag标志
    u8                                  u8ParentId[cPrivDrvParentIdLen];        //parentIdTag
    u8                                  u8Data[cPrivDrvRsvdLenA6];              //预留数据
    
}__attribute__((packed)) stPrivDrvCmdA6_t;








//--------------------------------------07、A7报文------------------------------
typedef struct 
{
    u8                                  u8GunId;                                //枪号
    ePrivDrvStartTransCmd_t             eCmd;                                   //操作指令
    ePrivDrvStartTransType_t            eType;                                  //启动类型
    stTime_t                            stTime;                                 //启动/结束时间
    u32                                 u32Meter;                               //启动/结束时电表读数
    u8                                  u8OrderId[cPrivDrvOrderIdLen];          //订单编号
    u8                                  u8SrvTradeId[cPrivDrvSrvTradeIdLen];    //平台交易流水号
    u8                                  u8DevTradeId[cPrivDrvDevTradeIdLen];    //设备交易流水号
    u8                                  u8Data[cPrivDrvRsvdLen07];              //预留数据
    
}__attribute__((packed)) stPrivDrvCmd07_t;



typedef struct 
{
    u8                                  u8GunId;                                //枪号
    ePrivDrvStartTransCmd_t             eCmd;                                   //操作指令
    ePrivDrvCmdRst_t                    eRst;                                   //操作结果
    u8                                  u8OrderId[cPrivDrvOrderIdLen];          //订单编号
    u8                                  u8SrvTradeId[cPrivDrvSrvTradeIdLen];    //平台交易流水号
    u8                                  u8Data[cPrivDrvRsvdLenA7];              //预留数据
}__attribute__((packed)) stPrivDrvCmdA7_t;








//--------------------------------------08、A8报文------------------------------
typedef struct 
{
    u8                                  u8GunId;                                //枪号
    ePrivDrvRtDataRptType_t             eRptType;                               //报文上报时机
    ePrivDrvStartTransType_t            eType;                                  //启动类型
    ePrivDrvChargingMode_t              eMode;                                  //充电模式
    unPrivDrvChargingParam_t            unParam;                                //充电参数
    ePrivDrvGunWorkStatus_t             eWorkStatus;                            //枪工作状态当前值
    u32                                 u32Power;                               //充电功率-------------1W
    u32                                 u32ChargingTime;                        //当前充电时长---------秒
    stTime_t                            stTimeStart;                            //启动充电时间
    u32                                 u32MeterStart;                          //启动电表读数---------1Wh
    u32                                 u32MeterNow;                            //当前电表读数---------1Wh
    u8                                  u8SocStart;                             //启动SOC--------------%1 (直流有效,交流置0)
    u8                                  u8SocNow;                               //当前SOC--------------%1 (直流有效,交流置0)
    u32                                 u32UsedMoney;                           //累计消费金额---------桩计费模式时有效,0.01元
    u8                                  u8UserId[cPrivDrvUserIdLen];            //用户ID
    u8                                  u8OrderId[cPrivDrvOrderIdLen];          //订单编号
    u8                                  u8SrvTradeId[cPrivDrvSrvTradeIdLen];    //平台交易流水号
    u8                                  u8DevTradeId[cPrivDrvDevTradeIdLen];    //设备交易流水号
    u16                                 u16MoneyDataLen;                        //分段电量费用明细数据长度
    unPrivDrvMoney_t                    unMoney;                                //电量费用明细
}__attribute__((packed)) stPrivDrvCmd08_t;



typedef struct 
{
    u8                                  u8GunId;                                //枪号
    ePrivDrvCmdRst_t                    eRst;                                   //操作结果
    ePrivDrvNetStatus_t                 eNetStatus;                             //桩连网状态
    u32                                 u32AccountBalance;                      //账户余额-------------单位0.01元
    u8                                  u8Data[cPrivDrvRsvdLenA8];              //预留数据
}__attribute__((packed)) stPrivDrvCmdA8_t;








//--------------------------------------09、A9报文------------------------------
typedef struct 
{
    u8                                  u8GunId;                                //枪号
    ePrivDrvStartTransType_t            eType;                                  //启动类型
    ePrivDrvChargingMode_t              eMode;                                  //充电模式
    unPrivDrvChargingParam_t            unParam;                                //充电参数
    ePrivDrvStopReason_t                eStopReason;                            //停充原因
    u32                                 u32ChargingTime;                        //本次充电时长---------秒
    stTime_t                            stTimeStart;                            //启动充电时间
    stTime_t                            stTimeStop;                             //结束充电时间
    u32                                 u32MeterStart;                          //启动电表读数---------1Wh
    u32                                 u32MeterStop;                           //结束电表读数---------1Wh
    u8                                  u8SocStart;                             //启动SOC--------------%1 (直流有效,交流置0)
    u8                                  u8SocStop;                              //结束SOC--------------%1 (直流有效,交流置0)
    u32                                 u32UsedMoney;                           //累计消费金额---------桩计费模式时有效,0.01元
    
    u8                                  u8UserId[cPrivDrvUserIdLen];            //用户ID
    u8                                  u8OrderId[cPrivDrvOrderIdLen];          //订单编号
    u8                                  u8SrvTradeId[cPrivDrvSrvTradeIdLen];    //平台交易流水号
    u8                                  u8DevTradeId[cPrivDrvDevTradeIdLen];    //设备交易流水号
    u16                                 u16MoneyDataLen;                        //分段电量费用明细数据长度
    unPrivDrvMoney_t                    unMoney;                                //电量费用明细
}__attribute__((packed)) stPrivDrvCmd09_t;



typedef struct 
{
    u8                                  u8GunId;                                //枪号
    ePrivDrvCmdRst_t                    eRst;                                   //操作结果
    u8                                  u8OrderId[cPrivDrvOrderIdLen];          //订单编号
    u8                                  u8SrvTradeId[cPrivDrvSrvTradeIdLen];    //平台交易流水号
}__attribute__((packed)) stPrivDrvCmdA9_t;








//--------------------------------------0A、AA报文------------------------------
typedef struct 
{
    u8                                  u8GunId;                                //枪号
    ePrivDrvCfgCmdType_t                eType;                                  //操作类型
    ePrivDrvCmdRst_t                    eRst;                                   //操作结果
    ePrivDrvParamAddr_t                 eAddr;                                  //参数地址
    u16                                 u16Len;                                 //参数内容长度
    u8                                  u8Data[cPrioDrvLenParamData];           //参数内容
}__attribute__((packed)) stPrivDrvCmd0A_t;



typedef struct 
{
    u8                                  u8GunId;                                //枪号
    ePrivDrvCfgCmdType_t                eType;                                  //操作类型
    ePrivDrvCmdRst_t                    eRst;                                   //操作结果
    ePrivDrvParamAddr_t                 eAddr;                                  //参数地址
    u16                                 u16Len;                                 //参数内容长度
    u8                                  u8Data[cPrioDrvLenParamData];           //参数内容
    
}__attribute__((packed)) stPrivDrvCmdAA_t;








//--------------------------------------0B、AB报文------------------------------
typedef struct 
{
    u16                                 u16AddrCpu;                             //目标cpu地址
    ePrivDrvUpdateCmd_t                 eCmd;                                   //操作命令
    u16                                 u16Rst;                                 //操作结果 0:成功         >0:失败
    
}__attribute__((packed)) stPrivDrvCmd0B_t;



typedef struct 
{
    u16                                 u16AddrCpu;                             //目标cpu地址
    u16                                 u16AddrBase;                            //基地址
    u16                                 u16Offset;                              //偏移地址
    ePrivDrvUpdateCmd_t                 eCmd;                                   //操作命令
    u16                                 u16Len;                                 //有效数据长度
    u16                                 u16Data[cPrioDrvLenUpdateData];         //有效数据包
    
}__attribute__((packed)) stPrivDrvCmdAB_t;








//--------------------------------------0C、AC报文------------------------------
typedef struct 
{
    u16                                 u16AddrCpu;                             //目标cpu地址
    ePrivDrvUpdateCmd_t                 eCmd;                                   //操作命令
    u16                                 u16Rst;                                 //操作结果 0:成功         >0:失败
    
}__attribute__((packed)) stPrivDrvCmd0C_t;



typedef struct 
{
    u16                                 u16AddrCpu;                             //目标cpu地址
    u16                                 u16AddrBase;                            //基地址
    u16                                 u16Offset;                              //偏移地址
    ePrivDrvUpdateCmd_t                 eCmd;                                   //操作命令
    u16                                 u16SegCrc;                              //校验码
    u8                                  u8LineNum;                              //有效行数
    u8                                  u8Len[cPrioDrvUpdateLineMax];           //每行数据有效长度
    u8                                  u8Data[cPrioDrvLenUpdateData];          //有效数据包
    
}__attribute__((packed)) stPrivDrvCmdAC_t;








//--------------------------------------0D、AD报文------------------------------
typedef struct 
{
    u8                                  u8Data[cPrioDrvLenLogData];             //日志内容
    
}__attribute__((packed)) stPrivDrvCmd0D_t;



typedef struct 
{
    u8                                  u8Resv;                                 //预留
}__attribute__((packed)) stPrivDrvCmdAD_t;








//--------------------------------------0E、AE报文------------------------------
typedef struct 
{
    u8                                  u8GunId;                                //枪号
    stPrivDrvPhaseData_t                stUacIn;                                //市电电压---输入-------0.1V
    stPrivDrvPhaseDataI_t               stIacIn;                                //市电电流---输入-------0.01A
    i16                                 i16Fac;                                 //市电频率--------------0.01Hz
    stPrivDrvPhaseData_t                stUacOut;                               //市电电压---输出-------0.1V
    stPrivDrvPhaseDataI_t               stIacOut;                               //市电电流---输出-------0.01A
    i16                                 i16PfTotal;                             //功率因数---总---------0.01KVA
    stPrivDrvPhaseData_t                stPf;                                   //功率因数---三相-------0.01KVA
    i16                                 i16Duty;                                //占空比----------------0.1%,
    i16                                 i16Ucp;                                 //CP电压----------------0.01V
    i16                                 i16Ucc;                                 //CC电压----------------0.01V
    i16                                 i16Offset0;                             //Offset0
    i16                                 i16Offset1;                             //Offset1
    i32                                 i32Ict;                                 //CT电流----------------0.01A
    stPrivDrvPhaseDataN_t               stUpeIn;                                //输入端对PE电压--------0.1V
    stPrivDrvPhaseDataN_t               stUpeOut;                               //输出端对PE电压--------0.1V
    i16                                 i16IleakAc;                             //交流漏电流------------1mA
    i16                                 i16IleakDc;                             //直流漏电流------------1mA
    stPrivDrvChgTempData_t              stChgTemp;                              //桩温度
    stPrivDrvGunTempData_t              stGunTemp;                              //枪温度
    u32                                 u32Power;                               //充电功率--------------1W
    u32                                 u32MeterNow;                            //当前电表读数----------1Wh
    u8                                  u8Data[cPrivDrvRsvdLen0E];              //预留数据
    
}__attribute__((packed)) stPrivDrvCmd0E_t;


typedef enum
{
    ePrivDrvCmdAeStsSuccess             = 0x00,                                 //成功
    ePrivDrvCmdAeStsFail                = 0x01,                                 //失败
    
    ePrivDrvCmdAeStsMax
}__attribute__((packed)) ePrivDrvCmdAeSts_t;

typedef struct 
{
    u8                                  u8GunId;                                //枪号
    ePrivDrvCmdAeSts_t                  eSts;                                   //状态
    u8                                  u8Data[cPrivDrvRsvdLenAE];              //预留数据
}__attribute__((packed)) stPrivDrvCmdAE_t;








//--------------------------------------11、B1报文------------------------------
typedef struct 
{
    eDismantleEvente_t                  eDismantleEvente;                       //拆机事件
    stTime_t                            stDismantleTime;                        //拆机事件的时间点
    eRebootEvente_t                     eRebootEvente;                          //重启事件
    stTime_t                            stRebootTime;                           //重启事件的时间点
    u8                                  u8Rsv[cPrioDrvLenSecurityEvt];          //预留
}__attribute__((packed)) stPrivDrvCmd11_t;

typedef struct 
{
    eSecurityEvtRst_t                   eSecurityEvtRst;                        //应答状态
    u8                                  u8Rsv[cPrioDrvLenSecurityEvtAck];       //预留
}__attribute__((packed)) stPrivDrvCmdB1_t;









//--------------------------------------13、B3报文------------------------------

typedef struct
{
    //BRM
    u8                                  u8BmsVersion[cPrioDrvLenBmsVer];        //BMS通信版本 旧国标V1.0 新国标V1.1
    ePrioDrvBatType_t                   eBatteryType;                           //电池类型
    u32                                 u32BatteryCap;                          //电池容量 0.1AH
    u32                                 u32BatteryVolt;                         //电池额定电压 0.1V
    u8                                  u8ManuName[cPrioDrvLenManuName];        //电池生产厂商
    u8                                  u8BatteryIndex[cPrioDrvLenCmd13Rsv1];   //rsv
    
    u16                                 u16ManuYear;                            //电池生产年 1985年偏移 1985~2235
    u8                                  u8ManuMonth;                            //电池生产月 1~12
    u8                                  u8ManuDay;                              //电池生产日 1~31
    u32                                 u32ChargeCnt;                           //电池充电次数
    u8                                  u8BatteryProperty;                      //电池组产权标志 0:租赁 1:车自有
    u8                                  u8Rsv2;                                 //保留
    
    u8                                  u8CarVin[cPrioDrvLenVin];               //车辆识别码
    u8                                  u8BmsSwVer[cPrioDrvLenBmsSwVer];        //新国标增加的8字节BMS信息
    //BCP
    u32                                 u32CellVoltMax;                         //单体动力蓄电池最高允许充电电压 0.01v 范围0~24v
    u32                                 u32BatteryCurrMax;                      //最高允许充电电流    0.1A
    u32                                 u32BcpBatteryCap;                       //电池标称总能量 0.1kwh 范围0~1000kwh
    u32                                 u32BatteryVoltMax;                      //电池最大充电电压 0.1v
    i8                                  i8BatteryTempMax;                       //电池允许温度 -50度偏移 范围-50~+200
    u16                                 u16BatteryCapStatus;                    //电磁荷电状态 0.1% 0~100%
    u32                                 u32BcpBatteryVolt;                      //电池总电压 0.1v
    //BRO
    u8                                  u8Ready;                                //BRO-BMS是否充电准备好
    //充电需求BCL
    u32                                 u32BmsVolt;                             //电池充电电压 0.1v
    u32                                 u32BmsCurr;                             //电池充电电流 0.1A
    u8                                  u8BmsMode;                              //电池充电模式
    //充电总状态BCS
    u32                                 u32BcsBmsVolt;                          //充电电压测量值 0.1v
    u32                                 u32BcsBmsCurr;                          //充电电流测量值 0.1A
    u32                                 u32CellMax;                             //单体电压最高及组号-电压
    u8                                  u32CellCnt;                             //单体电压最高及组号-组号
    u16                                 u16BmsSoc;                              //当前荷电状态 1% 0~100%
    u32                                 u32TimeRemain;                          //剩余充电时间 1min 0~600
    //电池状态信息BSM
    u8                                  u8CellVoltMaxIndex;                     //最高单体电压所在编号
    u8                                  u8BsmBatteryTempMax;                    //蓄电池最高温度
    u8                                  u8BatteryTempMaxIdx;                    //最高温度监测点
    u8                                  u8BatteryTempMin;                       //蓄电池最低温度
    u8                                  u8BatteryTempMinIdx;                    //最低温度监测点
    u8                                  u8BatteryOverHighLow;                   //BSM-单体动力蓄电池电压过高或过低
    u8                                  u8SocOverHighLow;                       //BSM-整车动力蓄电池荷电状态soc过高或过低
    u8                                  u8BatteryChargOverCurr;                 //BSM-动力蓄电池充电过电流
    u8                                  u8BatteryTempOverHigh;                  //BSM-动力蓄电池温度过高
    u8                                  u8BatteryInsSts;                        //BSM-动力蓄电池绝缘状态
    u8                                  u8BatteryConnectorSts;                  //BSM-动力蓄电池组输出连接器连接状态
    u8                                  u8AllowCharg;                           //BSM-允许充电
    //中止充电BST
    u8                                  u8BmsStopSoc;                           //BST-BMS达到所需求的SOC目标值
    u8                                  u8BmsStopTotalVolt;                     //BST-BMS达到总电压的设定值
    u8                                  u8StopUnitVolt;                         //BST-达到单体电压的设定值
    u8                                  u8StopChargerActively;                  //BST-充电机主动终止
    u8                                  u8FaultIns;                             //BST-绝缘故障
    u8                                  u8FaultConnector;                       //BST-输出连接器过温故障
    u8                                  u8FaultBmsConnector;                    //BST-BMS元件，输出连接器过温
    u8                                  u8FaultChargConnecto;                   //BST-充电连接器故障
    u8                                  u8FaultBatteryOverTemp;                 //BST-电池组温度过高故障
    u8                                  u8FaultHigtRelay;                       //BST-高压继电器故障
    u8                                  u8FaultCheck2;                          //BST-检测点2电压检测故障
    u8                                  u8FaultOther;                           //BST-其他故障
    u8                                  u8FaultCurr;                            //BST-电流过大
    u8                                  u8FaultVolt;                            //BST-电压异常
    
    //统计数据BSD
    u16                                 u16StatisticSoc;                        //中止荷电状态
    u32                                 u32StatisticVoltMin;                    //单体最低电压 0.01v 0~24v
    u32                                 u32StatisticVoltMax;                    //单体最高电压 0.01v 0~24v
    u8                                  u8StatisticTempMin;                     //最低温度 -50度偏移 范围-50~+200
    u8                                  u8StatisticTempMax;                     //最高温度 -50度偏移 范围-50~+200
    
    //BEM错误报文
    u8                                  u8BemTimeOut00;                         //BEM-接收SPN2560=0x00的CRM充电机辨识报文超时
    u8                                  u8BemTimeOutAA;                         //BEM-接收SPN2560=0xaa的CRM充电机辨识报文超时
    u8                                  u8BemTimeOutSync;                       //BEM-接收充电机的CML时间同步和最大输出能力报文超时
    u8                                  u8BemTimeOutCharging;                   //BEM-接收充电机CRO完成充电准备报文超时
    u8                                  u8BemTimeOutChargSts;                   //BEM-接收充电机CCS充电状态报文超时
    u8                                  u8BemTimeOutChargEnd;                   //BEM-接收充电机CST终止充电报文超时
    u8                                  u8BemTimeOutChargStatistics;            //BEM-接收充电机CSD充电统计报文超时
    u8                                  u8BemOther;                             //BEM-其他
}__attribute__((packed)) stPrivDrvBmsInfo_t;



typedef struct 
{
    u16                                 u16VoltConfig;                          //设定电压 *10V
    u16                                 u16CurrConfig;                          //设定电流 *10A
    u8                                  u8ModuleSts;                            //模块状态
    u8                                  u8ModuleSn[cPrioDrvLenModuleSn];        //Sn
    u8                                  u8ModuleFault[cPrioDrvLenModuleFault];  //模块故障组
    u8                                  u8Tmos2Temp;                            //LLC MOS管温度
    u8                                  u8Tdiod2Temp;                           //防倒灌二极管温度
    u8                                  u8LlcTemp;                              //LLC 变压器温度
    u8                                  u8PfcTemp;                              //PFC整流桥温度
    u8                                  u8Rsv[cPrioDrvLenCmd13Rsv3];            //预留
}__attribute__((packed)) stPrivDrvModuleInfo_t;



typedef struct 
{
    u16                                 u16Seq;                                 //报文次序计数
    u8                                  u8GunId;                                //枪号
    u8                                  u8Sn[cPrivDrvLenSn];                    //桩SN号
    ePrivDrvGunWorkStatus_t             eWorkStatus;                            //枪工作状态当前值
    ePrivDrvGunConnStatus_t             eConnStatus;                            //枪连接状态
    stPrivDrvBmsInfo_t                  stBmsInfo;                              //BMS/车数据
    stPrivDrvModuleInfo_t               stModuleInfo;                           //模块数据
}__attribute__((packed)) stPrivDrvCmd13_t;



typedef struct 
{
    u16                                 u16Seq;                                 //报文次序计数
    u8                                  u8GunId;                                //枪号
    u8                                  u8Sn[cPrivDrvLenSn];                    //桩SN号
}__attribute__((packed)) stPrivDrvCmdB3_t;









//--------------------------------------B4、14报文------------------------------

typedef struct 
{
    ePrivDrvAppointType_t               eType;                                  //操作类型
    stTime_t                            stTimeStart;                            //开始时间
    stTime_t                            stTimeEnd;                              //结束时间
    u32                                 u32ChgCurr;                             //预约充电电流,单位：0.1A
    u8                                  u8UserId[cPrivDrvUserIdLen];            //用户ID
    u8                                  u8OrderId[cPrivDrvOrderIdLen];          //订单编号
    ePrivDrvAppointMode_t               eMode;                                  //预约模式
    u8                                  u8GunId;                                //枪号
    ePrivDrvAppointFlg_t                eFlg;                                   //充电标志
    ePrivDrvStartTransType_t            eStartType;                             //启动类型
    u8                                  u8Data[cPrivDrvRsvdLenB4];              //预留数据
}__attribute__((packed)) stPrivDrvCmdB4_t;

typedef struct 
{
    ePrivDrvAppointType_t               eType;                                  //操作类型
    ePrivDrvAppointRsn_t                eRst;                                   //结果
    stTime_t                            stTimeStart;                            //开始时间
    stTime_t                            stTimeEnd;                              //结束时间
    u32                                 u32ChgCurr;                             //预约充电电流,单位：0.1A
    stTime_t                            stTimeExit;                             //预约退出时间
    ePrivDrvAppointRsn_t                eExitRsn;                               //退出原因
    u8                                  u8UserId[cPrivDrvUserIdLen];            //用户ID
    u8                                  u8OrderId[cPrivDrvOrderIdLen];          //订单编号
    ePrivDrvAppointSts_t                eSts;                                   //是否处于预约
    ePrivDrvAppointMode_t               eMode;                                  //预约模式
    u8                                  u8GunId;                                //枪号
    ePrivDrvAppointFlg_t                eFlg;                                   //充电标志
    ePrivDrvStartTransType_t            eStartType;                             //启动类型
    u8                                  u8Data[cPrivDrvRsvdLen14];              //预留数据
}__attribute__((packed)) stPrivDrvCmd14_t;









//--------------------------------------15、B5报文------------------------------

typedef struct 
{
    stPrivDrvMpcDataFlag_t              stDataFlag;                             //帧  数据标志
    stPrivDrvPcuData_t                  stDataPcu;                              //PCU 数据
    stPrivDrvModData_t                  stDataMod;                              //模块数据
    stPrivDrvScuData_t                  stDataScu;                              //SCU 数据
    
}__attribute__((packed)) stPrivDrvCmd15_t;

typedef struct 
{
    ePrivDrvCmdRst_t                    eRst;                                   //操作结果
}__attribute__((packed)) stPrivDrvCmdB5_t;









//--------------------------------------16、B6报文------------------------------

typedef struct 
{
    u8                                  u8GunId;                                //枪号
    i32                                 i32IdcOut;                              //充电电流----单位：0.01A
    i16                                 i16Tgun;                                //枪头温度----单位：0.1℃
    u16                                 u16Qliquid;                             //液体流量----单位：0.01 L/min
    u16                                 u16Pliquid;                             //液体压力----单位：0.01 bar
    u16                                 u16TliquidIn;                           //回液温度----单位：0.1 ℃，
    u16                                 u16TliquidOut;                          //供液温度----单位：0.1 ℃，
    u16                                 u16PumpSpeed;                           //液泵转速----
    u16                                 u16FanSpeed;                            //风机转速----
    stPrivDrvCoolingStatus_t            stCoolingSts;                           //液冷机告警状态
}__attribute__((packed)) stPrivDrvCmd16_t;

typedef struct 
{
    ePrivDrvCmdRst_t                    eRst;                                   //操作结果
}__attribute__((packed)) stPrivDrvCmdB6_t;























//payload union结构定义
typedef union
{
    stPrivDrvCmd01_t                    stCmd01;
    stPrivDrvCmdA1_t                    stCmdA1;

    stPrivDrvCmd0230_t                  stCmd0230;
    stPrivDrvCmdA230_t                  stCmdA230;
    
    stPrivDrvCmd02_t                    stCmd02;
    stPrivDrvCmdA2_t                    stCmdA2;
    
    stPrivDrvCmd03_t                    stCmd03;
    stPrivDrvCmdA3_t                    stCmdA3;
    
    stPrivDrvCmd04_t                    stCmd04;
    stPrivDrvCmdA4_t                    stCmdA4;
    
    stPrivDrvCmd05_t                    stCmd05;
    stPrivDrvCmdA5_t                    stCmdA5;
    
    stPrivDrvCmd06_t                    stCmd06;
    stPrivDrvCmdA6_t                    stCmdA6;
    
    stPrivDrvCmd07_t                    stCmd07;
    stPrivDrvCmdA7_t                    stCmdA7;
    
    stPrivDrvCmd08_t                    stCmd08;
    stPrivDrvCmdA8_t                    stCmdA8;
    
    stPrivDrvCmd09_t                    stCmd09;
    stPrivDrvCmdA9_t                    stCmdA9;
    
    stPrivDrvCmd0A_t                    stCmd0A;
    stPrivDrvCmdAA_t                    stCmdAA;
    
    stPrivDrvCmd0B_t                    stCmd0B;
    stPrivDrvCmdAB_t                    stCmdAB;
    
    stPrivDrvCmd0C_t                    stCmd0C;
    stPrivDrvCmdAC_t                    stCmdAC;
    
    stPrivDrvCmd0D_t                    stCmd0D;
    stPrivDrvCmdAD_t                    stCmdAD;
    
    stPrivDrvCmd0E_t                    stCmd0E;
    stPrivDrvCmdAE_t                    stCmdAE;
    
    stPrivDrvCmd11_t                    stCmd11;
    stPrivDrvCmdB1_t                    stCmdB1;
    
    stPrivDrvCmd13_t                    stCmd13;
    stPrivDrvCmdB3_t                    stCmdB3;
    
    stPrivDrvCmdB4_t                    stCmdB4;
    stPrivDrvCmd14_t                    stCmd14;
    
    stPrivDrvCmd15_t                    stCmd15;
    stPrivDrvCmdB5_t                    stCmdB5;
    
    stPrivDrvCmd16_t                    stCmd16;
    stPrivDrvCmdB6_t                    stCmdB6;
    
    //使用数组访问 便于拷贝
    u8                                  u8Data[cPrivDrvBufSize - cPrivDrvHeadSize - cPrivDrvCrcSize];
}__attribute__((packed)) unPrivDrvPayload_t;




//整个帧结构定义
typedef struct
{
    stPrivDrvHead_t                     stHead;
    unPrivDrvPayload_t                  unPayload;
}__attribute__((packed)) stPrivDrvPkt_t;

typedef union 
{
    stPrivDrvPkt_t                      stPkt;
    u8                                  u8Buf[cPrivDrvBufSize];
}__attribute__((packed)) unPrivDrvPkt_t;


































//--------------------------------------协议可读可写数据缓存---------------------------------

//Master 侧桩的缓存数据结构
typedef struct 
{
    ePrivDrvVersion_t                   eVersion;                               //当前接收版本
    u16                                 u16Addr;                                //当前接收桩端地址
    u8                                  u8AckSeqno;                             //供给 ack发送 使用的报文序列号
    u8                                  u8RptSeqno;                             //供给 rpt发送 使用的报文序列号
    
    u8                                  u8GunNum;                               //实际的枪口数量
    ePrivDrvCmdRst_t                    eLoginAck;                              //签到状态
    ePrivDrvNetHwType_t                 eHwType;                                //联网类型
    ePrivDrvNetStatus_t                 eNetStatus;                             //桩连网状态
    i8                                  i8Rssi;                                 //信号质量
    stPrivDrvChgTempData_t              stChgTemp;                              //充电桩温度
    u16                                 u16EleLoss;                             //电损率
    
    stPrivDrvCmd02_t                    stCache02;                              //02报文缓存
    stPrivDrvCmdA2_t                    stCacheA2;                              //A2报文缓存
    bool                                bCache02Flag;                           //02报文签到标志----收到02报文时置true
    
    stPrivDrvCmd03_t                    stCache03;                              //03
    stPrivDrvCmd04_t                    stCache04;                              //04
    SemaphoreHandle_t                   hCache03Mutex;                          //03报文信号--------收到03报文时发出此信号---（枪号为0）桩的03报文
    SemaphoreHandle_t                   hCache04Mutex;                          //04报文信号--------收到04报文时发出此信号
    
    
    stPrivDrvCmd0A_t                    stCache0AAck;                           //0A报文缓存--------桩端被动应答的0A报文
    stPrivDrvCmd0A_t                    stCache0ARpt;                           //0A报文缓存--------桩端主动上报的0A报文
    SemaphoreHandle_t                   hCache0ARptMutex;                       //0A报文信号--------收到桩端主动上报的0A报文时发出此信号
    
    stPrivDrvCmd11_t                    stCache11;                              //11报文缓存
    SemaphoreHandle_t                   hCache11Mutex;                          //11报文信号--------收到11报文时发出此信号(桩主动上报信号量)
    
    stPrivDrvCmd15_t                    stMpcData;                              //15报文缓存--------
    
}__attribute__((packed)) stPrivDrvChgDataForMaster_t;







//Slave 侧桩的缓存数据结构
typedef struct 
{
    u16                                 u16Addr;                                //桩端通讯地址
    u8                                  u8AckSeqno;                             //供给 ack发送 使用的报文序列号
    u8                                  u8RptSeqno;                             //供给 rpt发送 使用的报文序列号
    
    u8                                  u8GunNum;                               //实际的枪口数量
    ePrivDrvCmdRst_t                    eLoginAck;                              //签到状态
    ePrivDrvNetHwType_t                 eHwType;                                //联网类型
    u8                                  u8GwVer[cPrivDrvGwVerLen];              //网关版本号
    ePrivDrvNetStatus_t                 eNetStatus;                             //桩连网状态
    i8                                  i8Rssi;                                 //信号质量
    stPrivDrvChgTempData_t              stChgTemp;                              //充电桩温度
    u16                                 u16EleLoss;                             //电损率
    
    stPrivDrvCmd02_t                    stCache02;                              //02报文缓存
    stPrivDrvCmdA2_t                    stCacheA2;                              //A2报文缓存
    bool                                bCacheA2Flag;                           //A2报文签到应答-----收到A2报文时置true
    
    stPrivDrvCmd03_t                    stCache03;                              //03
    stPrivDrvCmdA3_t                    stCacheA3;                              //A3
    stPrivDrvCmdA4_t                    stCacheA4;                              //A4
    stTime_t                            stMasterTime;                           //主站时间
    SemaphoreHandle_t                   hTimeMutex;                             //主站时间信号-------收到主站时间时发出此信号
    
    
    stPrivDrvCmdAA_t                    stCacheAARpt;                           //AA报文缓存---------网关主动下发的AA报文
    stPrivDrvCmdAA_t                    stCacheAAAck;                           //AA报文缓存---------网关被动应答的AA报文
    SemaphoreHandle_t                   hCacheAARptMutex;                       //AA报文信号---------收到AA报文时发出此信号(桩主动上报)
    
    
    stPrivDrvCmdAB_t                    stCacheAB;                              //AB
    SemaphoreHandle_t                   hCacheABMutex;                          //AB报文信号--------收到AB报文时发出此信号
    
    stPrivDrvCmdAC_t                    stCacheAC;                              //AC
    SemaphoreHandle_t                   hCacheACMutex;                          //AC报文信号--------收到AC报文时发出此信号
    
    stPrivDrvCmd0D_t                    stCache0D;                              //0D
    
    stPrivDrvCmdB1_t                    stCacheB1;                              //B1报文缓存
    
}__attribute__((packed)) stPrivDrvChgDataForSlave_t;





//枪的缓存数据字段
typedef struct 
{
    SemaphoreHandle_t                   hCache03Mutex;                          //03报文信号--------收到03报文时发出此信号---（枪号为1,2...）枪的03报文
    ePrivDrvGunWorkStatus_t             eWorkStatus;                            //枪工作状态
    ePrivDrvGunWorkStatus_t             eWorkStatusOld;                         //枪工作状态上一次值
    ePrivDrvGunConnStatus_t             eConnStatus;                            //枪连接状态
    ePrivDrvGunLockStatus_t             eLockStatus;                            //枪  锁状态
    ePrivDrvGunEvent_t                  eEvent;                                 //枪    事件
    ePrivDrvGunWorkStsRsn_t             eStsReason;                             //状态变化原因
    stPrivDrvFaultCode_t                stFaultCode;                            //故障告警码
    u8                                  u8RsvdDataStsAck[cPrivDrvRsvdLenA3];    //预留数据
    
    
    SemaphoreHandle_t                   hCache05Mutex;                          //05报文信号--------收到新的05报文时发出此信号
    SemaphoreHandle_t                   hCacheA5Mutex;                          //A5报文信号--------收到新的05报文时发出此信号
    stPrivDrvCmd05_t                    stCardAuthData;                         //05报文缓存--------
    stPrivDrvCmdA5_t                    stCardAuthAckData;                      //A5报文缓存--------
    
    SemaphoreHandle_t                   hCache06Mutex;                          //06报文信号--------收到新的A6报文时发出此信号
    SemaphoreHandle_t                   hCacheA6Mutex;                          //A6报文信号--------收到新的A6报文时发出此信号
    stPrivDrvCmd06_t                    stRmtAck;                               //06报文缓存--------
    stPrivDrvCmdA6_t                    stRmt;                                  //A6报文缓存--------
    
    
    SemaphoreHandle_t                   hCache07Mutex;                          //07报文信号--------收到新的07报文时发出此信号
    SemaphoreHandle_t                   hCacheA7Mutex;                          //A7报文信号--------收到新的07报文时发出此信号
    stPrivDrvCmd07_t                    stTrans;                                //07报文缓存--------
    stPrivDrvCmdA7_t                    stTransAck;                             //A7报文缓存--------
    
    
    SemaphoreHandle_t                   hCache08Mutex;                          //08报文信号--------收到08报文时发出此信号
    SemaphoreHandle_t                   hCacheA8Mutex;                          //A8报文信号--------收到A8报文时发出此信号
    stPrivDrvCmd08_t                    stRtData;                               //08报文缓存--------存储充电实时数据
    stPrivDrvCmdA8_t                    stRtDataAck;                            //A8报文缓存
    ePrivDrvRtDataRptType_t             eRptType;                               //报文上报时机
    u32                                 u32ChargingPower;                       //充电功率-------------1W
    
    SemaphoreHandle_t                   hCache09Mutex;                          //09报文信号--------收到09报文时发出此信号
    SemaphoreHandle_t                   hCacheA9Mutex;                          //A9报文信号---------收到A9报文时发出此信号
    stPrivDrvCmd09_t                    stRcdData;                              //09报文缓存--------存储充电结算数据
    stPrivDrvCmdA9_t                    stRcdDataAck;                           //A9报文缓存
    
    SemaphoreHandle_t                   hCache0EMutex;                          //0E报文信号--------收到0E报文时发出此信号
    stPrivDrvCmd0E_t                    stCache0E;                              //0E数据
    u8                                  u8RsvdDataYcAck[cPrivDrvRsvdLenAE];     //AE预留数据
    
    SemaphoreHandle_t                   hCache13Mutex;                          //13报文信号--------收到13报文时发出此信号
    stPrivDrvCmd13_t                    stBmsData;                              //BMS数据
    bool                                bBmsDataUpdata;                         //BMS数据更新,0未更新,1已更新
    
    SemaphoreHandle_t                   hCacheB4Mutex;                          //B4报文信号--------收到B4报文时发出此信号
    stPrivDrvCmd14_t                    stBookingDataAck;                       //14报文缓存--------存储预约命令数据
    stPrivDrvCmdB4_t                    stBookingData;                          //B4报文缓存
    
    stPrivDrvCmd16_t                    stCoolingData;                          //16报文缓存--------
    
    
}__attribute__((packed)) stPrivDrvGunData_t;








typedef struct 
{
    stPrivDrvChgDataForMaster_t         stChg;                                  //桩数据
    stPrivDrvGunData_t                  stGun[cPrivDrvGunNumMax];               //枪数据
    
}stPrivDrvDataMaster_t;

typedef struct 
{
    stPrivDrvChgDataForSlave_t          stChg;                                  //桩数据
    stPrivDrvGunData_t                  stGun[cPrivDrvGunNumMax];               //枪数据
}stPrivDrvDataSlave_t;


typedef union 
{
    stPrivDrvDataMaster_t               stMaster;                               //Master侧使用
    stPrivDrvDataSlave_t                stSlave;                                //Slave 侧使用
}unPrivDrvData_t;










#endif









