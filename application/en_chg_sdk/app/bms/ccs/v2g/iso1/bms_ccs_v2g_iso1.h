/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     bms_ccs_v2g_iso1.h
 * @Version               :     V1.0
 * @Author                :     Hall
 * @Date Created          :     2022-07-04
 * @Attention             :     
 * @Brief                 :     ISO15118 协议实现
 * 
 * @History:
 * 
 * 1.@Date: 2022-07-04
 *   @Author: Hall
 *   @Modify: 
 * 
 * 2.@Date: 2024-03-12
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#ifndef _bms_ccs_v2g_iso1_h_
#define _bms_ccs_v2g_iso1_h_
#include "EXITypes.h"
#include "iso1EXIDatatypes.h"
#include "iso1EXIDatatypesEncoder.h"
#include "iso1EXIDatatypesDecoder.h"

#include "en_common.h"
#include "en_log.h"

#include "adc_app_sample_spec.h"
#include "bms_ccs_net_v2gtp.h"











//ISO15118 消息类型id定义
typedef enum
{
    eV2gIso1MsgIdSupportedAppProtocol       = 0,                                    // 0a/0b,   支持应用程序协议&结果
    eV2gIso1MsgIdSessionSetup               = 1,                                    // 1a/1b,   会话建立&结果
    eV2gIso1MsgIdServiceDiscovery           = 2,                                    // 2a/2b,   服务发现和结果
    eV2gIso1MsgIdServiceDetail              = 3,                                    //          服务详情(opt VAS)
    
    //SelectedPaymentOption = ???
    eV2gIso1MsgIdPaymentServiceSelection    = 4,                                    // xa/xb,   服务和支付选择&结果
    
    //SelectedPaymentOption = Contract          ————  PnC模式(PnC比EIM多了以下3个消息)
    eV2gIso1MsgIdCertificateInstallation    = 5,                                    //          证书安装(opt Certificate Installation)
    eV2gIso1MsgIdCertificateUpdate          = 6,                                    //          证书更新(opt Certificate Update)
    eV2gIso1MsgIdPaymentDetails             = 7,                                    //          支付详情
    
    //SelectedPaymentOption = External Payment  ————  EIM模式
    eV2gIso1MsgIdAuthorization              = 8,                                    // ya/yb,   合同授权&结果(EIM和PnC都有这个消息)
    
    //PEVRequestedEnergyTransfer = ???
    eV2gIso1MsgIdChargeParameterDiscovery   = 9,                                    // 3a/3b,   充电参数发现和结果
    
    // PEVRequestedEnergyTransfer = DC_???      ————  DC Charger专用消息
    eV2gIso1MsgIdCableCheck                 = 10,                                   // 4a/4b,   线缆检测和结果
    eV2gIso1MsgIdPreCharge                  = 11,                                   // 5a/5b,   预充电和结果
    
    eV2gIso1MsgIdPowerDeliveryStart         = 12,                                   // 6a/6b,   电力输送和结果(ChargeProgress = iso1chargeProgressType_Start)
    
    // PEVRequestedEnergyTransfer = AC_???      ————  AC Charger专用消息
    eV2gIso1MsgIdChargingStatus             = 13,                                   //          充电状态
    
    // PEVRequestedEnergyTransfer = DC_???      ————  DC Charger专用消息
    eV2gIso1MsgIdCurrentDemand              = 14,                                   // 7a/7b,   电流需求
    
    // 只有在 消息ChargingStatus或CurrentDemand 字段ReceiptRequired=TRUE 时才会接收到该信息
    eV2gIso1MsgIdMeteringReceipt            = 15,                                   //          计量收据
    
    eV2gIso1MsgIdPowerDeliveryRenegotiate   = 16,                                   //          电力输送和结果(ChargeProgress = iso1chargeProgressType_Renegotiate)
    
    eV2gIso1MsgIdPowerDeliveryStop          = 17,                                   // 8a/8b,   电力输送和结果(ChargeProgress = iso1chargeProgressType_Stop)
    
    // PEVRequestedEnergyTransfer = DC_???      ————  DC Charger专用消息
    eV2gIso1MsgIdWeldingDetection           = 18,                                   // 9a/9b,   焊接检测及结果(opt Certificate Installation)
    
    eV2gIso1MsgIdSessionStop                = 19,                                   //10a/10b,  会话停止
    
    eV2gIso1MsgIdMax
    
}eV2gIso1MsgId_t;



//ISO15118 协议 消息map
typedef struct
{
    //消息类型id
    eV2gIso1MsgId_t                      eId;
    
    //消息处理函数
    i32                                 (*pProc)(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut);
    
}stV2gIso1MsgIdMap_t;








//ISO15118 协议 抽象的缓存数据 结构定义
//用于缓存收发报文的结构体 ———— 由sizeof计算得出该结构体共消耗77507Byte,大约76K Byte
typedef struct 
{
    struct iso1SessionSetupReqType                  stSessionSetupReq;                      //size:8
    struct iso1SessionSetupResType                  stSessionSetupRes;                      //size:176
    
    struct iso1ServiceDiscoveryReqType              stServiceDiscoveryReq;                  //size:264
    struct iso1ServiceDiscoveryResType              stServiceDiscoveryRes;                  //size:3696
    
    struct iso1ServiceDetailReqType                 stServiceDetailReq;                     //size:2
    struct iso1ServiceDetailResType                 stServiceDetailRes;                     //size:34932
    
    struct iso1PaymentServiceSelectionReqType       stPaymentServiceSelectionReq;           //size:136
    struct iso1PaymentServiceSelectionResType       stPaymentServiceSelectionRes;           //size:1
    
    struct iso1CertificateInstallationReqType       stCertificateInstallationReq;           //size:2172
    struct iso1CertificateInstallationResType       stCertificateInstallationRes;           //size:9964
    
    struct iso1CertificateUpdateReqType             stCertificateUpdateReq;                 //size:5652
    struct iso1CertificateUpdateResType             stCertificateUpdateRes;                 //size:9968
    
    struct iso1PaymentDetailsReqType                stPaymentDetailsReq;                    //size:4284
    struct iso1PaymentDetailsResType                stPaymentDetailsRes;                    //size:32
    
    struct iso1AuthorizationReqType                 stAuthorizationReq;                     //size:228
    struct iso1AuthorizationResType                 stAuthorizationRes;                     //size:2
    
    struct iso1ChargeParameterDiscoveryReqType      stChargeParamDiscoveryReq;              //size:96
    struct iso1ChargeParameterDiscoveryResType      stChargeParamDiscoveryRes;              //size:3996
    
    struct iso1PowerDeliveryReqType                 stPowerDeliveryReq;                     //size:332
    struct iso1PowerDeliveryResType                 stPowerDeliveryRes;                     //size:32
    
    struct iso1CableCheckReqType                    stCableCheckReq;                        //size:8
    struct iso1CableCheckResType                    stCableCheckRes;                        //size:16
    
    struct iso1PreChargeReqType                     stPreChargeReq;                         //size:16
    struct iso1PreChargeResType                     stPreChargeRes;                         //size:16
    
    //struct iso1ChargingStatusReqType              stChargingStatusReq;                    //该请求消息无内容需解析
    struct iso1ChargingStatusResType                stChargingStatusRes;                    //size:432
    
    struct iso1CurrentDemandReqType                 stCurrentDemandReq;                     //size:60
    struct iso1CurrentDemandResType                 stCurrentDemandRes;                     //size:464
    
    struct iso1MeteringReceiptReqType               stMeteringReceiptReq;                   //size:464
    struct iso1MeteringReceiptResType               stMeteringReceiptRes;                   //size:32
    
    struct iso1WeldingDetectionReqType              stWeldingDetectionReq;                  //size:8
    struct iso1WeldingDetectionResType              stWeldingDetectionRes;                  //size:16
    
    struct iso1SessionStopReqType                   stSessionStopReq;                       //size:1
    struct iso1SessionStopResType                   stSessionStopRes;                       //size:1
    
}stV2gIso1Data_t;



//ISO15118 协议 抽象的缓存数据 结构定义
//用于缓存接收报文结构体中的公共字段
typedef struct 
{
    //都是 ISO15118 协议文本的专有名词  不使用命名风格
    /* =============================================== Public =============================================== */
    i32                                             ServiceID;
    
    
    
    /* ===============================================   AC   =============================================== */
    //没有必要存入公共数据缓存
    
    
    /* ===============================================   DC   =============================================== */
    struct iso1DC_EVStatusType                      DC_EVStatus;
    
    i32                                             EVMaximumCurrentLimit_isUsed:1;
    i32                                             EVMaximumPowerLimit_isUsed:1;
    i32                                             EVMaximumVoltageLimit_isUsed:1;
    struct iso1PhysicalValueType                    EVMaximumCurrentLimit;
    struct iso1PhysicalValueType                    EVMaximumPowerLimit;
    struct iso1PhysicalValueType                    EVMaximumVoltageLimit;
    
    i32                                             BulkChargingComplete;
    i32                                             BulkChargingComplete_isUsed:1;
    i32                                             ChargingComplete;
    
    struct iso1PhysicalValueType                    EVTargetVoltage;
    struct iso1PhysicalValueType                    EVTargetCurrent;
    
}stV2gIso1DataPublic_t;













//ISO15118 协议缓存结构定义
typedef struct
{
    //本地缓存的cp状态
    eChgCpState_t                                   eCpStateOld;
    
    
    //ISO15118协议解析收发doc的结构体---这一对结构体大约消耗96k的ram
    //接收doc解析和发送doc填充不存在多线程冲突 不需要配备信号量保护
    struct iso1EXIDocument                          stDocIn;                                //size:48720
    struct iso1EXIDocument                          stDocOut;                               //size:48720
    
    
    //记录最近收到的v2g消息类型
    eV2gIso1MsgId_t                                 eId;
    
    //SessionID
    u8                                              u8SessionId[iso1MessageHeaderType_SessionID_BYTES_SIZE];
    
    
    //数据缓存结构体
    SemaphoreHandle_t                               xSemHandshake;                          //握手信号
    stV2gIso1Data_t                                 stData;
    stV2gIso1DataPublic_t                           stDataPublic;
    
    
    //状态机操作资源
    void                                            (*pCbShutdownEmg)(i32 i32DevIndex);     //回调函数:EVSE紧急停机
    
    
    //时序操作资源
    void                                            (*pCbSeqTimerRst)(i32 i32DevIndex);     //回调函数:V2G_SECC_Sequence_Timer 复位
    void                                            (*pCbSeqTimerStart)(i32 i32DevIndex);   //回调函数:V2G_SECC_Sequence_Timer 启动
    void                                            (*pCbCpStateDetectionTimerStart)(i32 i32DevIndex, eChgCpState_t eCpState);//回调函数:V2G_SECC_CPState_Detection_Timeout 启动
    bool                                            (*pCbSetTcpConnClose)(i32 i32DevIndex); //回调函数:sBmsCcsNetSetTcpConnClose
    
}stV2gIso1Cache_t;
















extern bool  sV2gIso1Init(i32 i32DevIndex, void (*pCb0)(i32 i32DevIndex), void (*pCb1)(i32 i32DevIndex), void (*pCb2)(i32 i32DevIndex), void (*pCb3)(i32 i32DevIndex, eChgCpState_t eCpState), bool (*pCb4)(i32 i32DevIndex));
extern bool  sV2gIso1CacheInit(i32 i32DevIndex);
extern i32   sV2gIso1Proc(i32 i32DevIndex, const u8 *pData, i32 i32DataLen, u8 *pRxBuf, u8 *pTxBuf, i32 i32BufLen);

extern bool  sV2gIso1SetCpState(i32 i32DevIndex, eChgCpState_t eCpState);
extern bool  sV2gIso1GetSessionStop(i32 i32DevIndex);
extern bool  sV2gIso1SetResponseCodeForCpStateDetectionTimeout(i32 i32DevIndex, eChgCpState_t eCpState);














#endif



