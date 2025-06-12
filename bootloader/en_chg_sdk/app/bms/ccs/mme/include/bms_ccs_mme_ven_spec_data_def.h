/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   bms_ccs_mme_ven_spec_data_def.h
* Description                           :   MME协议实现 之 供应商特定的MME帧 数据结构定义
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-07-05
* notice                                :   基于联芯通文档 <MSE102x Vertexcom Vender-Specific MME Specification> 开发
****************************************************************************************************/
#ifndef _bms_ccs_mme_ven_spec_data_def_h
#define _bms_ccs_mme_ven_spec_data_def_h
#include "en_common_eth.h"






//供应商特定的 MME 消息的结构payload内字段值定义-----------------------------------------------------------



//xxx.cnf--->Result 字段值定义  适用用几乎所有的 供应商特定MME 消息
typedef enum
{
    eMmeVsRstSuccess = 0x00,                                           //
    eMmeVsRstFail,                                                     //
    
    eMmeVsRstMax
}__attribute__((packed)) eMmeVsRst_t;






//VS_GET_VERSION.req & VS_GET_VERSION.cnf
//VS_GET_STATUS.cnf--->Device ID 字段长度定义
#define cMmeVsGetStatusDeviceIdLen      2

//VS_GET_STATUS.cnf--->Applicative Version 字段长度定义
#define cMmeVsGetStatusAppVerLen        16

//VS_GET_STATUS.cnf--->AV Stack Version 字段长度定义
#define cMmeVsGetStatusAvStackVerLen    64

//VS_GET_STATUS.cnf--->Applicative Alternate 字段长度定义
#define cMmeVsGetStatusAppAlternateLen  16

//VS_GET_STATUS.cnf--->Applicative Alternate 字段长度定义
#define cMmeVsGetStatusBootVerLen       64






//VS_GET_STATUS.req & VS_GET_STATUS.cnf
//VS_GET_STATUS.cnf--->Status 字段值定义
typedef enum
{
    eMmeVsGetStatusStsUnassociated = 0x00,                                      //
    eMmeVsGetStatusStsAssociated,                                               //
    eMmeVsGetStatusStsAuthenticated,                                            //
    
    eMmeVsGetStatusStsMax
}__attribute__((packed)) eMmeVsGetStatusSts_t;



//VS_GET_STATUS.cnf--->CCO 字段值定义
typedef enum
{
    eMmeVsGetStatusCcoNot = 0x00,                                               // is not CCO (station only)
    eMmeVsGetStatusCcoYes,                                                      // is CCO
    
    eMmeVsGetStatusCcoMax
}__attribute__((packed)) eMmeVsGetStatusCco_t;



//VS_GET_STATUS.cnf--->Preferred CCO 字段值定义
typedef enum
{
    eMmeVsGetStatusPreferredCcoNot = 0x00,                                      // is not Preferred CCO
    eMmeVsGetStatusPreferredCcoYes,                                             // is Preferred CCO
    
    eMmeVsGetStatusPreferredCcoMax
}__attribute__((packed)) eMmeVsGetStatusPreferredCco_t;



//VS_GET_STATUS.cnf--->Backup CCO 字段值定义
typedef enum
{
    eMmeVsGetStatusBackupCcoNot = 0x00,                                         // is not Backup CCO
    eMmeVsGetStatusBackupCcoYes,                                                // is Backup CCO
    
    eMmeVsGetStatusBackupCcoMax
}__attribute__((packed)) eMmeVsGetStatusBackupCco_t;



//VS_GET_STATUS.cnf--->Proxy CCO 字段值定义
typedef enum
{
    eMmeVsGetStatusProxyCcoNot = 0x00,                                         // is not Proxy CCO
    eMmeVsGetStatusProxyCcoYes,                                                // is Proxy CCO
    
    eMmeVsGetStatusProxyCcoMax
}__attribute__((packed)) eMmeVsGetStatusProxyCco_t;



//VS_GET_STATUS.cnf--->Simple Connect 字段值定义
typedef enum
{
    eMmeVsGetStatusSimpleConnectNot = 0x00,                                     // not support Simple Connect
    eMmeVsGetStatusSimpleConnectYes,                                            // support Simple Connect
    
    eMmeVsGetStatusSimpleConnectMax
}__attribute__((packed)) eMmeVsGetStatusSimpleConnect_t;



//VS_GET_STATUS.cnf--->Link State 字段值定义
typedef enum
{
    eMmeVsGetStatusLinkStateDisconn = 0x00,                                     // disconnected
    eMmeVsGetStatusLinkStateConn,                                               // connected
    
    eMmeVsGetStatusLinkStateMax
}__attribute__((packed)) eMmeVsGetStatusLinkState_t;



//VS_GET_STATUS.cnf--->Ready for PLC operation 字段值定义
typedef enum
{
    eMmeVsGetStatusReadyNot = 0x00,                                             // not ready
    eMmeVsGetStatusReadyYes,                                                    // ready
    
    eMmeVsGetStatusReadyMax
}__attribute__((packed)) eMmeVsGetStatusReady_t;






//VS_GET_LINK_STATS.req & VS_GET_LINK_STATS.cnf
//VS_GET_LINK_STATS.req--->ReqType 字段值定义
typedef enum
{
    eMmeVsGetLinkStatsReqTypeReset = 0x00,                                      //清除相应link的统计信息
    eMmeVsGetLinkStatsReqTypeGet,                                               //获取
    eMmeVsGetLinkStatsReqTypeResetAfterGet,                                     //获取后清除
    
    eMmeVsGetLinkStatsReqTypeMax
}__attribute__((packed)) eMmeVsGetLinkStatsReqType_t;



//VS_GET_LINK_STATS.req--->TLFlag 字段值定义
typedef enum
{
    eMmeVsGetLinkStatsTlFlagTl = 0x00,                                          //transmit Link
    eMmeVsGetLinkStatsTlFlagRl,                                                 //receive Link
    
    eMmeVsGetLinkStatsTlFlagMax
}__attribute__((packed)) eMmeVsGetLinkStatsTlFlag_t;



//VS_GET_LINK_STATS.req--->Mgmt_Flag 字段值定义
//在 PLC（Power Line Communication）电力载波以太网中，"not management Link" 的意思是 "非管理连接"。
//通常情况下，PLC系统中会存在两种类型的连接：管理连接和非管理连接。管理连接用于进行网络管理、配置和监控等任务，而非管理连接则是指不用于这些管理目的的连接。
//因此，"not management Link" 表示那些不被用于网络管理的连接。这个术语可以用于描述在PLC网络中，那些不具备管理功能或者不参与网络管理的连接或通信链路。
typedef enum
{
    eMmeVsGetLinkStatsMgmtFlagNot = 0x00,                                       //not management Link
    eMmeVsGetLinkStatsMgmtFlagYes,                                              //    management Link
    
    eMmeVsGetLinkStatsMgmtFlagMax
}__attribute__((packed)) eMmeVsGetLinkStatsMgmtFlag_t;



//VS_GET_LINK_STATS.cnf--->Link Stats 字段结构定义
typedef struct
{
    //Link Statistics for Transmit Link
    u32                                 u32TxMsduSegSuccess;                    //MSDU分段成功 的数量？
    u32                                 u32TxMpdu;                              //    传输的MPDU数量
    u32                                 u32TxMpduBurst;                         //突发传输的MPDU数量
    u32                                 u32TxMpduAcked;                         //传输并获得确认的MPDU数量
    u32                                 u32TxMpduColl;                          //发生冲突的传输MPDU数量
    u32                                 u32TxMpduFail;                          //传输失败的MPDU数量
    u32                                 u32TxPbSuccess;                         //传输成功的PB（Protocol Data Block）数量
    u32                                 u32TxPbDropped;                         //传输但被丢弃的PB（协议数据块）数量
    u32                                 u32TxPbCrcFail;                         //传输但CRC错误的PB（协议数据块）数量
    u32                                 u32TxBufShortageDrop;                   //(预留以后使用)缓冲区短缺事件的数目
    
}__attribute__((packed)) stMmeVsGetLinkStatsDataTx_t;
typedef struct
{
    //Link Statistics for Receive Link
    u32                                 u32RxMsduSuccess;                       //MSDU重新组装成功 的数量？
    u32                                 u32RxMpdu;                              //    接收的MPDU数量
    u32                                 u32RxMpduBurst;                         //突发接收的MPDU数量
    u32                                 u32RxMpduAcked;                         //接收并获得确认的MPDU数量
    u32                                 u32RxMpduFail;                          //接收但失败的MPDU数量
    u32                                 u32RxMpduIcvFail;                       //接收到的MPDU中ICV校验失败的数量
    u32                                 u32RxPb;                                //接收的PB（Protocol Data Block）数量
    u32                                 u32RxPbSuccess;                         //用于重新组装的接收到的PB数量
    u32                                 u32RxPbDuplicated;                      //接收到的重复PB的数量
    u32                                 u32RxPbCrcFail;                         //接收但CRC错误的PB（协议数据块）数量
    u64                                 u64RxSumBerInPbSuccess;                 //成功接收的PB中的BER总和
    u32                                 u32RxSsnUnderMin;                       //MIN 下的站点序列号数量
    u32                                 u32RxSsnOverMax;                        //MAX 上的站点序列号数量
    u32                                 u32RxPbMissed;                          //丢失的PB数量
    
}__attribute__((packed)) stMmeVsGetLinkStatsDataRx_t;






//VS_GET_NW_INFO.req (空) & VS_GET_NW_INFO.cnf
//VS_GET_NW_INFO.cnf--->Link Stats 字段结构定义
typedef struct
{
    u8                                  u8StaTei;                               //STA的终端设备标识符
    u8                                  u8StaMac[ETH_ALEN];                     //STA的MAC地址
    u16                                 u16PhyTxCoded;                          //STA的编码传输物理层速率（Mbps）
    u16                                 u16PhyTxRaw;                            //STA的原始传输物理层速率（Mbps）
    u16                                 u16PhyRxCoded;                          //STA的编码接收物理层速率（Mbps）
    u16                                 u16PhyRxRaw;                            //STA的原始接收物理层速率（Mbps）
    u8                                  u8AgcGain;                              //STA的接收自动增益控制（AGC）增益
    
}__attribute__((packed)) stMmeVsGetNwInfoStaInfo_t;

//VS_GET_NW_INFO.cnf--->由于payload 长度可变 暂定最多16个 STA 信息 一般不会超过16个,
#define cMmeVsGetNwInfoStaNum           16






//VS_FILE_ACCESS.req &  VS_FILE_ACCESS.cnf
//VS_FILE_ACCESS.req--->OPcode 字段值定义
typedef enum
{
    eMmeVsFileAccessOpCodeWrite = 0x00,                                         //Write
    eMmeVsFileAccessOpCodeRead,                                                 //Read
    eMmeVsFileAccessOpCodeDelete,                                               //Delete
    eMmeVsFileAccessOpCodeListDir,                                              //列出目录---以下6种协议文本上没有,是联芯通内部代码里的定义
    eMmeVsFileAccessOpCodeMakeDir,                                              //创建目录
    eMmeVsFileAccessOpCodeDeleteDir,                                            //删除目录
    eMmeVsFileAccessOpCodeFormat,                                               //格式化flash
    eMmeVsFileAccessOpCodeSave,                                                 //未知
    eMmeVsFileAccessOpCodeScanSta,                                              //未知
    
    eMmeVsFileAccessOpCodeMax
}__attribute__((packed)) eMmeVsFileAccessOpCode_t;



//VS_FILE_ACCESS.req--->FileType 字段值定义
typedef enum
{
    eMmeVsFileAccessFileTypeBootloader = 0x00,                                  //Bootloader
    eMmeVsFileAccessFileTypeSimage,                                             //Simage
    eMmeVsFileAccessFileTypeOther,                                              //All other files
    eMmeVsFileAccessFileTypeDebugTrace,                                         //DebugTrace
    
    eMmeVsFileAccessFileTypeMax
}__attribute__((packed)) eMmeVsFileAccessFileType_t;

//VS_FILE_ACCESS--->Parameter 字段长度定义
#define cMmeVsFileAccessParamLen        32

//VS_FILE_ACCESS--->数据长度定义，联芯通MME协议并没有规定长度是1024 为了便于操作 EN+将其定义为1024字节
#define cMmeVsFileAccessDataLen         1024































#endif 



















