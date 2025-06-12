/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   bms_ccs_net_v2gtp.h
* Description                           :   SECC 网络层实现 之V2G Transfer Protocol(v2gtp)部分
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2022-06-21
* notice                                :   
****************************************************************************************************/
#ifndef _bms_ccs_net_v2gtp_h_
#define _bms_ccs_net_v2gtp_h_
#include "bms_ccs_net.h"
#include "bms_ccs_net_tsf.h"
#include "bms_ccs_net_sdp.h"












#define cV2gtpTcpDataCheckCycle         20                                      //ms
#define cV2gtpTcpDataCheckTimeout       70                                      //s

//当cp异常以后 tcp server主动关闭的等待时间
#define cV2gtpTcpTcpSrvCloseTime        5000                                    //ms






//V2GTP 报文head结构定义
typedef struct
{
    u8                                  u8Ver;
    u8                                  u8VerInverse;
    u16                                 u16PayloadType;
    u32                                 u32Len;
}__attribute__((__packed__)) stV2gtpHdr_t;
#define cV2gtpHeadSize                  sizeof(stV2gtpHdr_t)

//V2GTP u8Ver 定义
#define cV2gtpVer                       0x01                                    //当前标准V2GTP协议版本 1.0



//V2GTP 报文 payload type字段值定义
#define cV2gtpPayloadTypeExi            0x8001                                  //EXI 编码的 V2G消息
#define cV2gtpPayloadTypeSdpReq         0x9000                                  //SDP 请求消息
#define cV2gtpPayloadTypeSdpRes         0x9001                                  //SDP 响应消息





//V2GTP 报文 payload 结构定义
typedef union
{
    stBmsCcsNetSdpReq_t                 stSdpReq; 
    stBmsCcsNetSdpRes_t                 stSdpRes;
    
    u8                                  u8Buf[cV2gtpBufSize - cV2gtpHeadSize];
}__attribute__((packed)) unV2gtpPayload_t;




//V2GTP 报文 结构定义
typedef struct
{
    stV2gtpHdr_t                        stHead;
    unV2gtpPayload_t                    unPayload;
}__attribute__((packed)) stV2gtpPkg_t;



























extern void  sBmsCcsNetV2gtpTcpTask(void *pParam);
extern void  sBmsCcsNetV2gtpProc(stUdpParm_t *pUdp, i32 i32DevIndex, const u8 *pBuf, i32 i32Len);
extern bool  sBmsCcsNetV2gtpSendViaTcp(i32 i32DevIndex, const u8 *pBuf, u32 u32Len, u8 u8Ver, u16 u16PayloadType);
extern bool  sBmsCcsNetV2gtpSendViaUdp(stUdpParm_t *pUdp, const char *pBuf, u32 u32Len, u8 u8Ver, u16 u16PayloadType);




#endif


