/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :     bms_ccs_net_sdp.h
* Description                           :     SECC 网络层实现之 SECC Discovery Protocol(SDP)部分
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2022-06-21
* notice                                :     SDP协议的客户端为EVCC，服务端为SECC
****************************************************************************************************/
#ifndef _bms_ccs_net_sdp_h_
#define _bms_ccs_net_sdp_h_
#include "en_common.h"
#include "bms_ccs_net.h"





//SDP 报文缓存区长度
#define cBmsCcsNetSdpBufSize            64


//SDP协议端口定义
#define cBmsCcsNetSdpServerPort         15118
#define cBmsCcsNetSdpClientPortMin      49152
#define cBmsCcsNetSdpClientPortMax      65535












//SDP 报文 eSec字段值定义
typedef enum
{
    eBmsCcsNetSdpSecTls                 = 0x00,                                 //secured with TLS
    eBmsCcsNetSdpSecNone                = 0x10,                                 //No transport layer security
    
    eBmsCcsNetSdpSecMax
}__attribute__((packed)) eBmsCcsNetSdpSec_t;




//SDP 报文 eProtocol字段值定义
typedef enum
{
    eBmsCcsNetSdpProtocolTcp            = 0x00,                                 //TCP
    eBmsCcsNetSdpProtocolUdp            = 0x10,                                 //UDP
    
    eBmsCcsNetSdpProtocolMax
}__attribute__((packed)) eBmsCcsNetSdpProtocol_t;




//SDP 请求报文结构定义
typedef struct
{
    eBmsCcsNetSdpSec_t                  eSec;                                   //安全传输规范
    eBmsCcsNetSdpProtocol_t             eProtocol;                              //通讯协议
}__attribute__((__packed__)) stBmsCcsNetSdpReq_t;




//SDP 响应报文结构定义
typedef struct
{
    struct in6_addr                     stSeccIpAddr;                           //SECC的IP地址
    u16                                 u16SeccPort;                            //SECC的端口
    eBmsCcsNetSdpSec_t                  eSec;                                   //安全传输规范
    eBmsCcsNetSdpProtocol_t             eProtocol;                              //通讯协议
}__attribute__((__packed__)) stBmsCcsNetSdpRes_t;





























extern void sBmsCcsNetSdpReqRecv(stUdpParm_t *pUdp, i32 i32DevIndex, const u8 *pBuf);
























#endif

