/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   bms_ccs_net_tsf.c
* Description                           :   SECC 网络层实现 之报文传输部分
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2022-06-21
* notice                                :   
****************************************************************************************************/
#include "en_common.h"
#include "en_log.h"

#include "bms_ccs_net.h"
#include "bms_ccs_net_tsf.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "bms_ccs_net_tsf";




//---------------------------------------------------------------------------------------------------------





//---------------------------------------------------------------------------------------------------------





//网络接收报文处理

//网络发送报文处理
bool sBmsCcsNetTsfSendEth(stBmsCcsNetTsfParam_t stParam, const u8 *pBuf, u16 u16Len, u16 u16Proto);
bool sBmsCcsNetTsfSendIp6(stBmsCcsNetTsfParam_t stParam, const u8 *pBuf, u16 u16Len, u8 u8NextHead, u8 u8HopLimit);
bool sBmsCcsNetTsfSendIcmpv6(stBmsCcsNetTsfParam_t stParam, const u8 *pBuf, u16 u16Len, u8 u8Icmpv6Type, u8 u8Icmpv6Code);
//---------------------------------------------------------------------------------------------------------


















/***************************************************************************************************
* Description                           :   以太网帧 发送
* Author                                :   Hall
* Creat Date                            :   2022-06-29
* notice                                :   发送一帧以太网帧
****************************************************************************************************/
bool sBmsCcsNetTsfSendEth(stBmsCcsNetTsfParam_t stParam, const u8 *pBuf, u16 u16Len, u16 u16Proto)
{
    bool bRst;
    u8   u8Buf[128];
    struct eth_hdr *pEthHdrTx = NULL;
    
    
    memset(u8Buf, 0, sizeof(u8Buf));
    pEthHdrTx = (struct eth_hdr *)u8Buf;
    
    memcpy(pEthHdrTx->dest.addr, stParam.u8Dmac, ETH_ALEN);
    memcpy(pEthHdrTx->src.addr, stParam.u8Smac, ETH_ALEN);
    pEthHdrTx->type = htons(u16Proto);
    
    memcpy(u8Buf + sizeof(struct eth_hdr), pBuf, u16Len);
    bRst = sMse102xSend(stParam.i32Index, sizeof(struct eth_hdr) + u16Len, u8Buf);
    
    return(bRst);
}





/***************************************************************************************************
* Description                           :   IPv6 报文发送
* Author                                :   Hall
* Creat Date                            :   2022-06-29
* notice                                :   发送一帧UDP报文
*                                           u16Len 是 pBuf 中的数据长度，也是 UDP 报头加payload的长度
****************************************************************************************************/
bool sBmsCcsNetTsfSendIp6(stBmsCcsNetTsfParam_t stParam, const u8 *pBuf, u16 u16Len, u8 u8NextHead, u8 u8HopLimit)
{
    u8  u8Buf[128];
    struct ip6_hdr *pIpv6HdrTx = NULL;
    
    memset(u8Buf, 0, sizeof(u8Buf));
    pIpv6HdrTx = (struct ip6_hdr *)u8Buf;
    
    pIpv6HdrTx->_v_tc_fl                = htonl(cIpv6Version << 28);//IPv6报文 固定值
    pIpv6HdrTx->_plen                   = htons(u16Len);//IPv6报头中的payload_len字段 是payload部分的长度值
    pIpv6HdrTx->_nexth                  = u8NextHead;
    pIpv6HdrTx->_hoplim                 = u8HopLimit;
    memcpy(&pIpv6HdrTx->src,  &stParam.stSaddr, sizeof(struct in6_addr));
    memcpy(&pIpv6HdrTx->dest, &stParam.stDaddr, sizeof(struct in6_addr));
    
    memcpy(u8Buf + sizeof(struct ip6_hdr), pBuf, u16Len);
    return sBmsCcsNetTsfSendEth(stParam, u8Buf, sizeof(struct ip6_hdr) + u16Len, ETH_P_IPV6);
}






/***************************************************************************************************
* Description                           :   ICMPV6 报文发送
* Author                                :   Hall
* Creat Date                            :   2022-06-30
* notice                                :   发送一帧ICMPV6报文
*                                           校验和计算：需要将 ICMPV6 报文部分 与 伪首部 一起计算
****************************************************************************************************/
bool sBmsCcsNetTsfSendIcmpv6(stBmsCcsNetTsfParam_t stParam, const u8 *pBuf, u16 u16Len, u8 u8Icmpv6Type, u8 u8Icmpv6Code)
{
    u16 u16Icmpv6PkgLen = 0;
    u16 u16Check = 0;                                                           //总的校验和
    u32 u32Icmpv6PkgSum = 0;                                                    //UDP 报文部分的校验和
    
    u8  u8Buf[64];
    struct icmp6_hdr *pIcmpv6HdrTx = NULL;
    
    u16Icmpv6PkgLen = sizeof(struct icmp6_hdr) + u16Len;
    memset(u8Buf, 0, sizeof(u8Buf));
    pIcmpv6HdrTx = (struct icmp6_hdr *)u8Buf;
    
    pIcmpv6HdrTx->type = u8Icmpv6Type;
    pIcmpv6HdrTx->code = u8Icmpv6Code;
    pIcmpv6HdrTx->chksum = 0;//计算校验和之前先给0
    memcpy(u8Buf + sizeof(struct icmp6_hdr), pBuf, u16Len);
    
    u32Icmpv6PkgSum = sCheckSum16Adj((const char*)pIcmpv6HdrTx, u16Icmpv6PkgLen);
    u16Check = sCheckSumPseudoHead(stParam.stSaddr, stParam.stDaddr, u16Icmpv6PkgLen, IP6_NEXTH_ICMP6, u32Icmpv6PkgSum);
    pIcmpv6HdrTx->chksum = htons(u16Check);
    
    //u8HopLimit 255 是固定值
    return sBmsCcsNetTsfSendIp6(stParam, u8Buf, u16Icmpv6PkgLen, IP6_NEXTH_ICMP6, 255);
}
















