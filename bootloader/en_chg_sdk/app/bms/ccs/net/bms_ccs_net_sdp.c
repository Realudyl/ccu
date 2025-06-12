/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   bms_ccs_net_sdp.c
* Description                           :   SECC 网络层实现之 SECC Discovery Protocol(SDP)部分
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2022-06-21
* notice                                :   
****************************************************************************************************/
#include "en_common.h"
#include "en_common_eth.h"
#include "en_log.h"


#include "bms_ccs_net_ndp.h"
#include "bms_ccs_net_sdp.h"
#include "bms_ccs_net_v2gtp.h"




//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "bms_ccs_net_sdp";





//---------------------------------------------------------------------------------------------------------

extern stBmsCcsNetCache_t *pBmsCcsNetCache[];



//---------------------------------------------------------------------------------------------------------


void sBmsCcsNetSdpReqRecv(stUdpParm_t *pUdp, i32 i32DevIndex, const u8 *pBuf);
void sBmsCcsNetSdpResSend(stUdpParm_t *pUdp, const struct in6_addr *pIpAddr, u16 u16Port,  eBmsCcsNetSdpSec_t eSec, eBmsCcsNetSdpProtocol_t eProtocol);



//---------------------------------------------------------------------------------------------------------














/***************************************************************************************************
* Description                           :   SDP请求报文处理
* Author                                :   Hall
* Creat Date                            :   2022-06-29
* notice                                :   注意：这里理解本SECC端仅满足 eBmsCcsNetSdpSecNone 和 eBmsCcsNetSdpProtocolTcp 需求
*                                           超出此范围的需求就不响应了 还不确定是否合适
****************************************************************************************************/
void sBmsCcsNetSdpReqRecv(stUdpParm_t *pUdp, i32 i32DevIndex, const u8 *pBuf)
{
    stBmsCcsNetCache_t  *pCache    = NULL;
    stBmsCcsNetSdpReq_t *pSdpReq   = NULL;
    stV2gtpPkg_t        *pV2gtpPkg = NULL;
    
    
    pCache = pBmsCcsNetCache[i32DevIndex];
    
    pV2gtpPkg = (stV2gtpPkg_t *)(pBuf);
    pSdpReq   = &pV2gtpPkg->unPayload.stSdpReq;
    
    if((pSdpReq->eSec      == eBmsCcsNetSdpSecNone)
    && (pSdpReq->eProtocol == eBmsCcsNetSdpProtocolTcp))
    {
        EN_SLOGI(TAG, "收到SDP req消息, 安全规范:0x%02X,协议类型:0x%02X", pSdpReq->eSec, pSdpReq->eProtocol);
        sBmsCcsNetNdpNsSend(i32DevIndex, pBuf, pUdp->pPev->stMmeDev.pNetif->hwaddr, (struct in6_addr *)pUdp->pPev->stMmeDev.pNetif->ip6_addr[0].u_addr.ip6.addr);
        sBmsCcsNetSdpResSend(pUdp, (struct in6_addr *)pUdp->pPev->stMmeDev.pNetif->ip6_addr[0].u_addr.ip6.addr, pCache->u16Port, pSdpReq->eSec, pSdpReq->eProtocol);
    }
    else if((pSdpReq->eSec      == eBmsCcsNetSdpSecTls)
         && (pSdpReq->eProtocol == eBmsCcsNetSdpProtocolTcp))
    {
        //TEST  此处为测试内容，暂时不清楚此处为什么要这么判断。
        EN_SLOGI(TAG, "收到SDP req消息, 安全规范:0x%02X,协议类型:0x%02X", pSdpReq->eSec, pSdpReq->eProtocol);
        sBmsCcsNetNdpNsSend(i32DevIndex, pBuf, pUdp->pPev->stMmeDev.pNetif->hwaddr, (struct in6_addr *)pUdp->pPev->stMmeDev.pNetif->ip6_addr[0].u_addr.ip6.addr);
        sBmsCcsNetSdpResSend(pUdp, (struct in6_addr *)pUdp->pPev->stMmeDev.pNetif->ip6_addr[0].u_addr.ip6.addr, pCache->u16Port, eBmsCcsNetSdpSecNone, eBmsCcsNetSdpProtocolTcp);
    }
    else
    {
        EN_SLOGE(TAG, "收到SDP req消息, 安全规范:0x%02X,协议类型:0x%02X, 不在支持范围内", pSdpReq->eSec, pSdpReq->eProtocol);
    }
    
    return;
}





/***************************************************************************************************
* Description                           :   SDP响应报文发送
* Author                                :   Hall
* Creat Date                            :   2022-06-29
* notice                                :   pSdpReq 是收到的 SDP 请求报文，需要从里面提取各种地址和端口
****************************************************************************************************/
void sBmsCcsNetSdpResSend(stUdpParm_t *pUdp, const struct in6_addr *pIpAddr, u16 u16Port,  eBmsCcsNetSdpSec_t eSec, eBmsCcsNetSdpProtocol_t eProtocol)
{
    char u8Buf[cBmsCcsNetSdpBufSize];
    stBmsCcsNetSdpRes_t *pSdpRes = NULL;
    
    memset(u8Buf, 0, sizeof(u8Buf));
    pSdpRes = (stBmsCcsNetSdpRes_t *)u8Buf;
    
    memcpy(&pSdpRes->stSeccIpAddr, pIpAddr, sizeof(struct in6_addr));
    pSdpRes->u16SeccPort = htons(u16Port);
    pSdpRes->eSec = eSec;
    pSdpRes->eProtocol = eProtocol;
    
    EN_SLOGI(TAG, "发送SDP res消息, TCPsrv端口:%d, 安全规范:0x%02X, 协议类型:0x%02X", u16Port, eSec, eProtocol);
    EN_HLOGI(TAG, "发送SDP res消息, TCPsrv  IP:", pIpAddr->un.u8_addr, sizeof(struct in6_addr));
    sBmsCcsNetV2gtpSendViaUdp(pUdp, u8Buf, sizeof(stBmsCcsNetSdpRes_t), cV2gtpVer, cV2gtpPayloadTypeSdpRes);
}





