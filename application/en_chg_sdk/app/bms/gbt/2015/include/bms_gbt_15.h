/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   bms_gbt_15.h
* Description                           :   GBT27930 协议实现
* Version                               :   
* Author                                :   haisheng.dang@en-plus.com.cn
* Creat Date                            :   2024-04-18
* notice                                :   
****************************************************************************************************/
#ifndef _bms_gbt_15_h_
#define _bms_gbt_15_h_

#include "en_log.h"
#include "en_common.h"

#include "can_drv_trans.h"
#include "bms_gbt_15_basic.h"
#include "bms_gbt_15_frame_def.h"

#include "j1939_drv.h"





//GBT27930 CAN错误消息帧标志定义
typedef enum 
{
    eBmsGbt15CanIdBit29DataFrame = 0,                                           //数据帧
    eBmsGbt15CanIdBit29ErrMsg = 1,                                              //错误消息
}eBmsGbt15CanIdBit29_t;



//GBT27930 CAN远程帧帧标志定义
typedef enum 
{
    eBmsGbt15CanIdBit30Data = 0,                                                //数据帧
    eBmsGbt15CanIdBit30Rtr = 1,                                                 //远程帧
}eBmsGbt15CanIdBit30_t;



//GBT27930 CAN扩展帧帧标志定义
typedef enum 
{
    eBmsGbt15CanIdBit31Standard  = 0,                                           //标准帧 SFF
    eBmsGbt15CanIdBit31Extended  = 1,                                           //扩展帧 EFF
}eBmsGbt15CanIdBit31_t;









//GBT27930 CAN协议 MsgId 定义
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
}__attribute__((packed)) stBmsGbt15CanId_t;


typedef union
{
    stBmsGbt15CanId_t                   stValue;                                //结构体访问
    u32                                 u32Value;                               //32位访问
}__attribute__((packed)) unBmsGbt15CanId_t;










//gbt27930-15协议自维护状态机
typedef enum
{
    eBmsGbt15StateWait                  = 0,                                    //待机
    eBmsGbt15StateHandshake             = 1,                                    //握手辨识阶段
    eBmsGbt15StateCableCheck            = 2,                                    //绝缘检测
    eBmsGbt15StateConfig                = 3,                                    //充电配置阶段
    eBmsGbt15StatePreCharge             = 4,                                    //预充电
    eBmsGbt15StateCharging              = 5,                                    //正常充电
    eBmsGbt15StateFinish                = 6,                                    //充电正常结束
    eBmsGbt15StateFaultAB               = 7,                                    //异常进入故障AB
    eBmsGbt15StateFaultC                = 8,                                    //超时进入故障C
    
    eBmsGbt15StateMax
}__attribute__((packed)) eBmsGbt15ProtState_t;









//GBT27930-15协议 消息map
typedef struct
{
    //消息类型 pgn
    eBmsGbt15Pgn_t                      ePgn;
    
    //消息处理函数
    i32                                 (*pProc)(ePileGunIndex_t eGunIndex, u8 *pMsg, u16 u16Len);
    
}__attribute__((packed)) stBmsGbt15MsgIdMap_t;




//GBT27930-2015 协议信息缓存结构定义
typedef struct
{
    //can报文收发端口
    i32                                 i32Port;
    
    //任务计数
    u32                                 u32TaskTick;
    
    //超充团标协议
    bool                                bSuperChgProt;
    
    //初始化标识(确保返回初始化阶段只执行一次)
    bool                                bInitOk;
    
    //断开辅助电源（协议要求只有等到接收到Bsd或Bsd超时才能断开辅助电源）
    bool                                bAllowStopAuxPower;
    
    //充电协议处于哪个阶段
    eBmsGbt15ProtState_t                eState;
    
    //用户层设置的一些关键信息
    stPacketSwitchCondition_t           stPacketSwitchCdt;
    
    //桩端待发送信息缓存
    stBmsGbt15Secc_t                    stSeccInfo;
    
    //车端接收信息缓存
    stBmsGbt15Evcc_t                    stEvccInfo;
    
    //Cem或Bem后尝试重新握手次数
    u16                                 u16TryAgainChgCount;
    
    //数据帧开始发送的时间，用于停止发送的额外判断条件
    u32                                 u32FirstSendTick;
    
    
    
    //状态机操作资源，回调函数用于紧急情况停机，100ms内掉电
    void                                (*pBmsGbt15ToShutdownEmg)(ePileGunIndex_t eGunIndex);
    
    //gbt15协议 时序资源                                                        //Timer1
    u32                                 u32Timer1Event;                         //当前检测的超时事件
    u32                                 u32Timer1Time;                          //事件的超时时间
    bool                                bTimer1IsRunning;                       //定时器运行中时置true
    bool                                bTimer1Timeout;                         //当前检测事件超时则该标志位置true
    TaskHandle_t                        xTaskTimer1;                            //时序定时器任务句柄
    
    u32                                 u32Timer2Event;                         //Timer2
    u32                                 u32Timer2Time;                          //
    bool                                bTimer2IsRunning;                       //
    bool                                bTimer2Timeout;                         //
    TaskHandle_t                        xTaskTimer2;                            //
    
    EventGroupHandle_t                  xEvtTiming;                             //时序事件组
    
}stBmsGbt15Cache_t;





extern bool   sBmsGbt15Init(ePileGunIndex_t eGunIndex, i32 i32Port, i32 i32Baudrate, void (*pCbFunc)(ePileGunIndex_t eGunIndex));
extern bool   sBmsGbt15CacheInit(ePileGunIndex_t eGunIndex);

extern i32    sBmsGbt15SendChm(ePileGunIndex_t eGunIndex);
extern i32    sBmsGbt15SendCrm(ePileGunIndex_t eGunIndex);
extern i32    sBmsGbt15SendCts(ePileGunIndex_t eGunIndex);
extern i32    sBmsGbt15SendCml(ePileGunIndex_t eGunIndex);
extern i32    sBmsGbt15SendCro(ePileGunIndex_t eGunIndex);
extern i32    sBmsGbt15SendCcs(ePileGunIndex_t eGunIndex);
extern i32    sBmsGbt15SendCst(ePileGunIndex_t eGunIndex);
extern i32    sBmsGbt15SendCsd(ePileGunIndex_t eGunIndex);
extern i32    sBmsGbt15SendCem(ePileGunIndex_t eGunIndex);

extern i32    sBmsGbt15RecvProc(ePileGunIndex_t eGunIndex);


#endif
