/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   bms_ccs_v2g_din.h
* Description                           :   DIN70121 协议实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2022-07-04
* notice                                :   
****************************************************************************************************/
#ifndef _bms_ccs_v2g_din_h_
#define _bms_ccs_v2g_din_h_
#include "EXITypes.h"
#include "dinEXIDatatypes.h"
#include "dinEXIDatatypesEncoder.h"
#include "dinEXIDatatypesDecoder.h"

#include "en_common.h"
#include "en_log.h"
#include "bms_ccs_net_v2gtp.h"


















//DIN70121 消息类型id定义
typedef enum
{
    eV2gDinMsgIdSupportedAppProtocol    = 0,                                    // 0a/0b,  支持应用程序协议&结果
    eV2gDinMsgIdSessionSetup            = 1,                                    // 1a/1b,  会话建立&结果
    eV2gDinMsgIdServiceDiscovery        = 2,                                    // 2a/2b,  服务发现和结果
    eV2gDinMsgIdServicePaymentSelection = 3,                                    // xa/xb,  服务支付选择&结果
    eV2gDinMsgIdContractAuthorization   = 4,                                    // ya/yb,  合同授权&结果
    eV2gDinMsgIdChargeParameterDiscovery= 5,                                    // 3a/3b,  充电参数发现和结果
    eV2gDinMsgIdCableCheck              = 6,                                    // 4a/4b,  线缆检测和结果
    eV2gDinMsgIdPreCharge               = 7,                                    // 5a/5b,  预充电和结果
    eV2gDinMsgIdPowerDelivery1          = 8,                                    // 6a/6b,  电力输送和结果(ReadyToCharge=1)
    eV2gDinMsgIdCurrentDemand           = 9,                                    // 7a/7b,  电流需求
    eV2gDinMsgIdPowerDelivery2          = 10,                                   // 8a/8b,  电力输送和结果(ReadyToCharge=0)
    eV2gDinMsgIdWeldingDetection        = 11,                                   // 9a/9b,  焊接检测及结果
    eV2gDinMsgIdSessionStop             = 12,                                   //10a/10b, 会话停止
    
    eV2gDinMsgIdMax
}eV2gDinMsgId_t;


//DIN70121 协议 消息map
typedef struct
{
    //消息类型id
    eV2gDinMsgId_t                      eId;
    
    //消息处理函数
    i32                                 (*pProc)(i32 i32DevIndex, struct dinEXIDocument *pDocIn, struct dinEXIDocument *pDocOut);
    
}stV2gDinMsgIdMap_t;














//DIN70121协议 抽象的缓存数据 结构定义
//用于缓存收发报文的结构体
typedef struct 
{
    
    struct dinSessionSetupReqType       stSessionSetupReq;                      //size:10
    struct dinSessionSetupResType       stSessionSetupRes;                      //size:56
    
    struct dinServiceDiscoveryReqType   stServiceDiscoveryReq;                  //size:136
    struct dinServiceDiscoveryResType   stServiceDiscoveryRes;                  //size:2544
    
    struct dinServicePaymentSelectionReqType  stServicePaymentSelectionReq;     //size:136
    struct dinServicePaymentSelectionResType  stServicePaymentSelectionRes;     //size:1
    
    //struct dinContractAuthenticationReqType stContractAuthReq;                //size:416---用不着
    struct dinContractAuthenticationResType   stContractAuthRes;                //size:2
    
    struct dinChargeParameterDiscoveryReqType stChargeParamDiscoveryReq;        //size:132
    struct dinChargeParameterDiscoveryResType stChargeParamDiscoveryRes;        //size:11784
    
    struct dinPowerDeliveryReqType      stPowerDeliveryReq;                     //size:252
    struct dinPowerDeliveryResType      stPowerDeliveryRes;                     //size:48
    
    struct dinSessionStopResType        stSessionStopRes;                       //size:1
    
    struct dinCableCheckReqType         stCableCheckReq;                        //size:20
    struct dinCableCheckResType         stCableCheckRes;                        //size:20
    
    struct dinPreChargeReqType          stPreChargeReq;                         //size:36
    struct dinPreChargeResType          stPreChargeRes;                         //size:24
    
    struct dinCurrentDemandReqType      stCurrentDemandReq;                     //size:108
    struct dinCurrentDemandResType      stCurrentDemandRes;                     //size:80
    
    struct dinWeldingDetectionReqType   stWeldingDetectionReq;                  //size:20
    struct dinWeldingDetectionResType   stWeldingDetectionRes;                  //size:24
    
}stV2gDinData_t;


//DIN70121协议 抽象的缓存数据 结构定义
//用于缓存接收报文结构体中的公共字段
typedef struct 
{
    //都是 DIN70121 协议文本的专有名词  不使用命名风格
    struct dinDC_EVStatusType           DC_EVStatus;
    
    i32                                 EVMaximumCurrentLimit_isUsed;
    i32                                 EVMaximumPowerLimit_isUsed;
    i32                                 EVMaximumVoltageLimit_isUsed;
    struct dinPhysicalValueType         EVMaximumCurrentLimit;
    struct dinPhysicalValueType         EVMaximumPowerLimit;
    struct dinPhysicalValueType         EVMaximumVoltageLimit;
    
    i32                                 BulkChargingComplete;
    i32                                 BulkChargingComplete_isUsed;
    i32                                 ChargingComplete;
    
    struct dinPhysicalValueType         EVTargetVoltage;
    struct dinPhysicalValueType         EVTargetCurrent;
    
}stV2gDinDataPublic_t;























//DIN70121 协议缓存结构定义
typedef struct
{
    //本地缓存的cp状态
    eChgCpState_t                       eCpStateOld;
    
    
    //DIN70121协议解析收发doc的结构体---这一对结构体消耗99k的ram
    //接收doc解析和发送doc填充不存在多线程冲突 不需要配备信号量保护
    struct dinEXIDocument               stDocIn;
    struct dinEXIDocument               stDocOut;
    
    
    //记录最近收到的v2g消息类型
    eV2gDinMsgId_t                      eId;
    
    //SessionID
    u8                                  u8SessionId[dinMessageHeaderType_SessionID_BYTES_SIZE];
    
    
    //数据缓存结构体
    SemaphoreHandle_t                   xSemHandshake;                          //握手信号
    stV2gDinData_t                      stData;
    stV2gDinDataPublic_t                stDataPublic;
    
    
    //状态机操作资源
    void                                (*pCbSwitchV2gToShutdownEmg)(i32 i32DevIndex);//回调函数:EVSE紧急停机
    
    
    //时序操作资源
    void                                (*pCbSeqTimerRst)(i32 i32DevIndex);     //回调函数:V2G_SECC_Sequence_Timer 复位
    void                                (*pCbSeqTimerStart)(i32 i32DevIndex);   //回调函数:V2G_SECC_Sequence_Timer 启动
    void                                (*pCbCpStateDetectionTimerStart)(i32 i32DevIndex, eChgCpState_t eCpState);//回调函数:V2G_SECC_CPState_Detection_Timeout 启动
    bool                                (*pCbSetTcpConnClose)(i32 i32DevIndex); //回调函数:sBmsCcsNetSetTcpConnClose
    
}stV2gDinCache_t;






































extern bool  sV2gDinInit(i32 i32DevIndex, void (*pCb0)(i32 i32DevIndex), void (*pCb1)(i32 i32DevIndex), void (*pCb2)(i32 i32DevIndex), void (*pCb3)(i32 i32DevIndex, eChgCpState_t eCpState), bool (*pCb4)(i32 i32DevIndex));
extern bool  sV2gDinCacheInit(i32 i32DevIndex);
extern i32   sV2gDinProc(i32 i32DevIndex, const u8 *pData, i32 i32DataLen, u8 *pRxBuf, u8 *pTxBuf, i32 i32BufLen);

extern bool  sV2gDinSetCpState(i32 i32DevIndex, eChgCpState_t eCpState);
extern bool  sV2gDinGetSessionStop(i32 i32DevIndex);
extern bool  sV2gDinSetResponseCodeForCpStateDetectionTimeout(i32 i32DevIndex, eChgCpState_t eCpState);






#endif






