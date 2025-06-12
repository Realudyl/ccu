/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   bms_ccs_net_ndp.c
* Description                           :   SECC 网络层实现之 邻居发现协议(ndp)部分
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2022-06-21
* notice                                :   
****************************************************************************************************/
#include "en_common.h"
#include "en_log.h"

#include "bms_ccs_net_ndp.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "bms_ccs_net_ndp";


//---------------------------------------------------------------------------------------------------------




//---------------------------------------------------------------------------------------------------------




bool sBmsCcsNetNdpNsSend(i32 i32DevIndex, const u8 *pBuf, const u8 *pMacSrc, const struct in6_addr *pAddrSrc);
void sBmsCcsNetNdpGetParam(const u8 *pBuf, stBmsCcsNetTsfParam_t *pParam);


//---------------------------------------------------------------------------------------------------------





/***************************************************************************************************
* Description                           :   NDP 协议 NS 报文发送
* Author                                :   Hall
* Creat Date                            :   2022-06-29
* notice                                :   注意 NS 报文的Mac目的地址和IP6目的地址 都需要转换
****************************************************************************************************/
bool sBmsCcsNetNdpNsSend(i32 i32DevIndex, const u8 *pBuf, const u8 *pMacSrc, const struct in6_addr *pAddrSrc)
{
    u8  u8Buf[32];
    u16 u16NdpNsMsgLen = 0;
    stNdpOpt_t *pOpt = NULL;
    stNdMsg_t *pNdMsg = NULL;
    stBmsCcsNetTsfParam_t stParam;
    
    
    memset(u8Buf, 0, sizeof(u8Buf));
    pNdMsg = (stNdMsg_t *)u8Buf;
    pOpt = (stNdpOpt_t *)(pNdMsg->u8Opt);
    
    sBmsCcsNetNdpGetParam(pBuf, &stParam);
    stParam.i32Index = i32DevIndex;
    memcpy(&stParam.u8Smac, pMacSrc, ETH_ALEN);
    memcpy(&stParam.stSaddr, pAddrSrc, sizeof(struct in6_addr));
    memcpy(&pNdMsg->stTargetAddr, &stParam.stDaddr, sizeof(struct in6_addr));   //本字段赋值前 尚不能将目的地址转换为组播地址
    
    pOpt->u8Type = ND_OPT_SOURCE_LL_ADDR;                                       //根据<RFC 4861>@4.6定义 
    pOpt->u8len = 1;                                                            //1 注意单位(这里u8len所表示的8个字节包括了u8len、u8Type和u8Data)
    memcpy(pOpt->u8Data, stParam.u8Smac, ETH_ALEN);
    
    sIp6UnicastToMulticast(&stParam.stDaddr);                                   //ip6单播目的地址换算组播地址
    sMacUnicastToMulticast(stParam.u8Dmac, &stParam.stDaddr);                   //Mac单播目的地址换算组播地址
    
    u16NdpNsMsgLen = (pOpt->u8len * cOptLenUnit) + sizeof(struct in6_addr);
    
    EN_SLOGI(TAG, "发送邻域发现协议NS报文");
    return sBmsCcsNetTsfSendIcmpv6(stParam, u8Buf, u16NdpNsMsgLen, NDISC_NEIGHBOUR_SOLICITATION, 0);
}







/***************************************************************************************************
* Description                           :   NDP 协议 从收到的udp报文中提取各层地址端口信息 供回复时使用
* Author                                :   Hall
* Creat Date                            :   2024-01-11
* notice                                :   
****************************************************************************************************/
void sBmsCcsNetNdpGetParam(const u8 *pBuf, stBmsCcsNetTsfParam_t *pParam)
{
    struct eth_hdr *pEthHdr = NULL;
    struct ip6_hdr *pIp6Hdr = NULL;
    struct udp_hdr *pUdpHdr = NULL;
    
    pUdpHdr = (struct udp_hdr *)(pBuf - sizeof(struct udp_hdr));
    pIp6Hdr = (struct ip6_hdr *)(pBuf - sizeof(struct udp_hdr) - sizeof(struct ip6_hdr));
    pEthHdr = (struct eth_hdr *)(pBuf - sizeof(struct udp_hdr) - sizeof(struct ip6_hdr) - sizeof(struct eth_hdr));
    
    memset(pParam, 0, sizeof(stBmsCcsNetTsfParam_t));
    pParam->u16Sport = ntohs(pUdpHdr->dest);
    pParam->u16Dport = ntohs(pUdpHdr->src);
    memcpy(pParam->u8Dmac, pEthHdr->src.addr, ETH_ALEN);
    memcpy(&pParam->stDaddr, pIp6Hdr->src.addr, sizeof(struct in6_addr));
    
}















