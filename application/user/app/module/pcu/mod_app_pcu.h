/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   mod_app_pcu.h
* Description                           :   PCU通讯
* Version                               :   
* Author                                :   haisheng.dang@en-plus.com.cn
* Creat Date                            :   2024-04-18
* notice                                :   
****************************************************************************************************/
#ifndef _mod_app_pcu_h_
#define _mod_app_pcu_h_

#include "en_log.h"
#include "en_common.h"
#include "bms_gbt_15_api.h"
#include "bms_gbt_15_state.h"
#include "adc_app_sample.h"
#include "io_app.h"
#include "eeprom_app.h"

#include "meter_drv_opt.h"





//端口配置（暂时没地方放，先丢这定义）
#ifndef cSdkPcuCanPort
#define cPcuCanPort                     (eCanNum0)
#else
#define cPcuCanPort                     (cSdkPcuCanPort)
#endif

#ifndef cSdkPcuCanBaudrate
#define cPcuCanBaudrate                 (125000)
#else
#define cPcuCanBaudrate                 (cSdkPcuCanBaudrate)
#endif
















//pcu地址
#define cPcuAddress                     0xA0

//广播地址
#define cBroadcast                      0xFF


//心跳回复最大失败次数，用于Pcu掉线判断
#define cPcuHeartCommFaultCnt           3


//Uid长度
#define cPcuUidLength                   4















//数据帧类型
typedef enum
{
    ePgnCodeRegister                    = 0x21,                                 //注册
    ePgnCodeRegisterResp                = 0x22,                                 //注册响应
    ePgnCodeHeartBeat                   = 0x23,                                 //心跳
    ePgnCodeHeartBetResp                = 0x24,                                 //心跳响应
    ePgnCodeEnergyReq                   = 0x25,                                 //功率申请
    ePgnCodeEnergyResp                  = 0x26,                                 //功率申请响应
    ePgnCodeVoltNotify                  = 0x27,                                 //枪内侧电压通知
    ePgnCodePcuErrCode                  = 0x29,                                 //Pcu发送故障码
}ePcuPgnCode_t;







//pcu通讯 CAN协议 MsgId 定义
typedef struct
{
    u32                                 bAddSrc  : 8;                           //源地址
    u32                                 bAddDst  : 8;                           //目的地址
    u32                                 bPgn     : 8;                           //pgn码
    u32                                 bDp      : 1;                           //数据页，27930标准默认未0
    u32                                 bR       : 1;                           //保留位
    u32                                 bPri     : 3;                           //优先级，从最高0-7
    eBmsGbt15CanIdBit29_t               ebit29   : 1;                           //bit29
    eBmsGbt15CanIdBit30_t               ebit30   : 1;                           //bit30
    eBmsGbt15CanIdBit31_t               ebit31   : 1;                           //bit31
}stPcuCanId_t;


typedef union
{
    stPcuCanId_t                        stValue;                                //结构体访问
    u32                                 u32Value;                               //32位访问
}unPcuCanId_t;









//CCU向PCU注册结果
typedef enum

{
    eRegistering                        = 0,                                    //注册ing
    eRegisterSuccess                    = 1,                                    //注册成功
    eRegisterRepeat                     = 2,                                    //重复注册
    eRegisterOccup                      = 3,                                    //枪编号被占用
    eRegisterPcuErr                     = 4,                                    //主柜故障
}eRegisterResult_t;





//注册失败原因
typedef enum
{
    eRegisterFailReasonRepeat           = 0,                                    //枪编号重复
    eRegisterFailReasonMax,
}eRegisterFailReason_t;





//当前CCU和PCU通讯状态
typedef enum
{
    ePcuCommStateFailed                 = 0,                                    //通讯故障
    ePcuCommStateNormal                 = 1,                                    //通讯正常
}ePcuCommState_t;







typedef struct
{
    u16                                 u16MaxChgVolt;                          //Bcp报文里最大允许充电电压
    u16                                 u16MaxChgCurr;                          //Bcp报文里最大允许充电电流
}stBcpReqParam_t;






typedef struct
{
    u32                                 u32InitTick;                            //初始化时间记录
    u32                                 u32HeartTick;                           //心跳时间记录
}stTaskTick_t;








//与PCU交互的数据管理类
typedef struct
{
    i32                                 i32Port;                                //通讯口
    u8                                  u8GunAddr;                              //枪地址
    u8                                  u8ChgPrio;                              //充电优先级
    u8                                  u8Uid[cPcuUidLength];                   //Uid，桩唯一的标识，Pcu注册用
    bool                                bIsSuperChgGun;                         //是否是超充枪（液冷枪是超充枪）
    bool                                bPcuEnergyErr;                          //Pcu能量无法提供
    
    
    stTaskTick_t                        stTaskTick;                             //保存系统时间
    eRegisterResult_t                   eRegisterResult;                        //注册结果
    ePcuCommState_t                     ePcuCommState;                          //和PCU通讯状态
    u16                                 u16CommFailCnt;                         //通讯失败计数
    
    
    u16                                 u16ReqVolt;                             //需求电压，单位：1V
    u16                                 u16ReqCurr;                             //需求电流，单位：1A
    u32                                 u32RatePower;                           //枪额定功率，单位：W
    
    u16                                 u16AvailVolt;                           //pcu可以提供的电压，单位：1V
    u16                                 u16AvailCurr;                           //pcu可以提供的电流，单位：1A
    u16                                 u16AvailPdc;                            //pcu可以提供的功率，单位：1kW
    u16                                 u16CurVolt;                             //pcu当前输出的电压，单位：1V
    stBcpReqParam_t                     stBcpReqParam;                          //Bcp报文里的一些字段
    
    u16                                 u16BusVolt;                             //直流母线电压
    
    u16                                 u16VoltIn;                              //枪内侧电压
    
}stModAppPcuInfo_t;



//u8GunAddr For B枪 的偏移
#define cGunAddrOffset                  (6)




























extern bool sModAppPcuInit(u8 u8SwitchAddr, bool bIsSuperCharger);

extern bool sModAppPcuOpen(ePileGunIndex_t eGunIndex);
extern bool sModAppPcuClose(ePileGunIndex_t eGunIndex);
extern bool sModAppPcuCloseContactor(void);
extern bool sModAppPcuSetOutput(ePileGunIndex_t eGunIndex, f32 f32Udc, f32 f32IdcLimit);
extern bool sModAppPcuGetUdcMax(ePileGunIndex_t eGunIndex, i32 *pMaxVolt);
extern bool sModAppPcuGetIdcMax(ePileGunIndex_t eGunIndex, i32 *pMaxCurr);
extern bool sModAppPcuGetUdcNow(ePileGunIndex_t eGunIndex, i32 *pCurVolt);
bool sModAppPcuGetReqVolt(ePileGunIndex_t eGunIndex, u16* u16ReqVolt);
bool sModAppPcuGetReqCurr(ePileGunIndex_t eGunIndex, u16* u16ReqCurr);


extern bool sModAppPcuGetCommSt(ePileGunIndex_t eGunIndex, ePcuCommState_t *pCommSt);
extern bool sModAppPcuGetEnergyErrSt(ePileGunIndex_t eGunIndex);









#endif













