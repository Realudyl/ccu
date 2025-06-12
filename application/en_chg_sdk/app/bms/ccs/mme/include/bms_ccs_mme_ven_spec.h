/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   bms_ccs_mme_ven_spec.h
* Description                           :   MME协议实现 之 供应商特定的MME帧
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2022-06-25
* notice                                :   基于联芯通文档 <MSE102x Vertexcom Vender-Specific MME Specification> 开发
****************************************************************************************************/
#ifndef _bms_ccs_mme_ven_spec_h
#define _bms_ccs_mme_ven_spec_h

#include <stdio.h>
#include <string.h>


#include "bms_ccs_mme_node.h"
#include "bms_ccs_mme_op.h"
#include "bms_ccs_mme_ven_spec_data_def.h"






//供应商特定的ME帧
#define VS_GET_VERSION                  0xA000
#define VS_RESET                        0xA008
#define VS_SET_KEY                      0xA00C
#define VS_GET_TONEMASK                 0xA01C
#define VS_GET_ETH_PHY                  0xA020
#define VS_ETH_STATS                    0xA024
#define VS_GET_STATUS                   0xA030
#define VS_GET_TONEMAP                  0xA034
#define VS_GET_SNR                      0xA038
#define VS_GET_SPECTRUM                 0xA03C
#define VS_GET_LINK_STATS               0xA040
#define VS_GET_AMP_MAP                  0xA044
#define VS_GET_NW_INFO                  0xA0FC
#define VS_SET_CAPTURE_STATE            0xA100
#define VS_SET_NVRAM                    0xA104
#define VS_GET_NVRAM                    0xA108
#define VS_GET_PWM_STATS                0xA10C
#define VS_GET_PWM_CONF                 0xA110
#define VS_SET_PWM_CONF                 0xA114
#define VS_PWM_GENERATION               0xA118
#define VS_SPI_STATS                    0xA11C
#define VS_SET_DSP_RECORD_STATE         0xA120
#define VS_GET_DSP_RECORD_STATUS        0xA124
#define VS_SET_TX_CAL                   0xA128
#define VS_FILE_ACCESS                  0xA4FC








//供应商特定的 MME 消息的结构payload定义-----------------------------------------------------------



//VS_GET_VERSION.req (空) & VS_GET_VERSION.cnf
typedef struct
{
    eMmeVsRst_t                         eRst;
    
    //Device ID:0x0000 = MSE1000 Series
    u8                                  u8DeviceId[cMmeVsGetStatusDeviceIdLen];
    
    //固定填充 0
    u8                                  u8ImageIndex;
    
    //当前应用层版本字符串
    u8                                  u8AppVer[cMmeVsGetStatusAppVerLen];
    
    //当前AV协议栈版本字符串
    u8                                  u8AvStackVer[cMmeVsGetStatusAvStackVerLen];
    
    //当前应用程序名称字符串
    u8                                  u8AppAlternate[cMmeVsGetStatusAppAlternateLen];
    
    //bootload版本字符串
    u8                                  u8BootVer[cMmeVsGetStatusBootVerLen];
    
}__attribute__((packed)) stMmeVsGetVersionCnf_t;






//VS_RESET.req (空) & VS_RESET.cnf
typedef struct
{
    eMmeVsRst_t                         eRst;
}__attribute__((packed)) stMmeVsResetCnf_t;






//VS_GET_STATUS.req (空) & VS_GET_STATUS.cnf
typedef struct
{
    eMmeVsRst_t                         eRst;
    eMmeVsGetStatusSts_t                eStatus;
    eMmeVsGetStatusCco_t                eCco;
    eMmeVsGetStatusPreferredCco_t       ePreferredCco;
    eMmeVsGetStatusBackupCco_t          eBackupCco;
    eMmeVsGetStatusProxyCco_t           eProxyCco;
    eMmeVsGetStatusSimpleConnect_t      eSimpleConnect;
    eMmeVsGetStatusLinkState_t          eLinkState;
    eMmeVsGetStatusReady_t              eReadySts;
    i64                                 i64FreqErr;                             //校正后的残余频率偏移值（单位：mppm）
    i64                                 i64FreqOffset;                          //STA_Clk的频率偏移与STA之间的频率偏移
    u64                                 u64Uptime;                              //系统运行时间（单位：秒）
    u64                                 u64AuthTime;                            //经过与CCO的站点认证后的时间（单位：秒）
    u16                                 u16AuthCount;                           //与CCO进行站点认证的计数
}__attribute__((packed)) stMmeVsGetStatusCnf_t;






//VS_GET_LINK_STATS.req & VS_GET_LINK_STATS.cnf
typedef struct
{
    eMmeVsGetLinkStatsReqType_t         eReqType;
    u8                                  u8ReqId;                                //请求id
    u8                                  u8LinkId;                               //仅当 eMgmtFlag 为0时有效
    eMmeVsGetLinkStatsTlFlag_t          eTlFlag;
    eMmeVsGetLinkStatsMgmtFlag_t        eMgmtFlag;
    u8                                  u8MacAddr[ETH_ALEN];                    //eTlFlag为0时是目的Mac地址 为1时是源Mac地址
}__attribute__((packed)) stMmeVsGetLinkStatsReq_t;

typedef struct
{
    u8                                  u8ReqId;                                //请求id
    eMmeVsRst_t                         eRst;
    stMmeVsGetLinkStatsDataTx_t         stLinkStatsTx;
}__attribute__((packed)) stMmeVsGetLinkStatsCnfForTx_t;

typedef struct
{
    u8                                  u8ReqId;                                //请求id
    eMmeVsRst_t                         eRst;
    stMmeVsGetLinkStatsDataRx_t         stLinkStatsRx;
}__attribute__((packed)) stMmeVsGetLinkStatsCnfForRx_t;






//VS_GET_NW_INFO.req (空) & VS_GET_NW_INFO.cnf
typedef struct
{
    u8                                  u8Nid[7];                   //与此新NMK关联的网络标识符（包括SL）
    u8                                  u8Snid;                                 //短网络标识符
    u8                                  u8CCcoTei;                              //AVLN中CCO的终端设备标识符
    u8                                  u8CCcoMac[ETH_ALEN];                    //AVLN中CCO的MAC地址
    u8                                  u8StaNum;                               //AVLN中的AV STA数量（N）
    stMmeVsGetNwInfoStaInfo_t           stSta[cMmeVsGetNwInfoStaNum];           //STA站点信息---实际接收时要动态调整
}__attribute__((packed)) stMmeVsGetNwInfoCnf_t;






//VS_FILE_ACCESS.req &  VS_FILE_ACCESS.cnf
typedef struct
{
    eMmeVsFileAccessOpCode_t            eOpCode;                                //操作码
    eMmeVsFileAccessFileType_t          eFileType;                              //操作文件类别
    u8                                  u8Param[cMmeVsFileAccessParamLen];      //操作文件类别
    u16                                 u16FragTotal;                           //片段总数
    u16                                 u16FragSeqNum;                          //片段序号
    u32                                 u32Offset;                              //偏移
    u32                                 u32CheckSum;                            //校验和---
    u16                                 u16Length;                              //片段长度
    u8                                  u8Data[cMmeVsFileAccessDataLen];        //写命令时 附加写入数据内容
}__attribute__((packed)) stMmeVsFileAccessReq_t;

typedef struct
{
    eMmeVsRst_t                         eRst;
    eMmeVsFileAccessOpCode_t            eOpCode;                                //操作码
    eMmeVsFileAccessFileType_t          eFileType;                              //操作文件类别
    u8                                  u8Param[cMmeVsFileAccessParamLen];      //操作文件类别
    u16                                 u16FragTotal;                           //片段总数
    u16                                 u16FragSeqNum;                          //片段序号
    u32                                 u32Offset;                              //偏移
    u16                                 u16Length;                              //片段长度
    u8                                  u8Data[cMmeVsFileAccessDataLen];        //读取命令时 附加读取数据内容
}__attribute__((packed)) stMmeVsFileAccessCnf_t;













































//供应商特定的MME帧 相关的一些缓存字段
typedef struct
{
    //一些缓存数据 为了简单起见 就缓存整个payload吧
    stMmeVsFileAccessReq_t              stVsFileAccessReq;
    
    
    stMmeVsGetVersionCnf_t              stVsGetVersionCnf;
    stMmeVsResetCnf_t                   stMmeVsResetCnf;
    stMmeVsGetStatusCnf_t               stVsGetStatusCnf;
    stMmeVsGetLinkStatsCnfForTx_t       stVsGetLinkStatsCnfForTx;
    stMmeVsGetLinkStatsCnfForRx_t       stVsGetLinkStatsCnfForRx;
    stMmeVsGetNwInfoCnf_t               stVsGetNwInfoCnf;
    stMmeVsFileAccessCnf_t              stVsFileAccessCnf;
}stMmeVsCache_t;





















//MME_VS 帧发送函数
extern bool sMmePacketSendVsGetVersionReq(struct pev_item *pPev, stMmeVsGetVersionCnf_t *pCnf);
extern bool sMmePacketSendVsResetReq(struct pev_item *pPev);
extern bool sMmePacketSendVsGetStatusReq(struct pev_item *pPev, stMmeVsGetStatusCnf_t *pCnf);
extern bool sMmePacketSendVsGetLinkStatsReq(struct pev_item *pPev, stMmeVsGetLinkStatsReq_t *pReq);
extern bool sMmePacketSendVsGetNwInfoReq(struct pev_item *pPev);
extern bool sMmePacketSendVsFileAccessReq(struct pev_item *pPev, stMmeVsFileAccessReq_t *pReq, stMmeVsFileAccessCnf_t *pCnf);


//MME_VS 帧接收处理函数
extern bool sMmePacketRecvVsGetVersionCnf(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen);
extern bool sMmePacketRecvVsResetCnf(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen);
extern bool sMmePacketRecvVsGetStatusCnf(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen);
extern bool sMmePacketRecvVsGetLinkStatsCnf(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen);
extern bool sMmePacketRecvVsGetNwInfoCnf(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen);
extern bool sMmePacketRecvVsFileAccessCnf(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen);









#endif 



















