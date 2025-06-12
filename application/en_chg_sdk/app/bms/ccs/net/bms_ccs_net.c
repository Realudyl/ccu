/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   bms_ccs_net.c
* Description                           :   SECC 网络层实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2022-06-21
* notice                                :   
*                                           网络层主要实现3个功能：NDP协议、SDP协议、V2GTP协议----(按时间顺序)
*                                           1：NDP  协议 需要在收到EVCC端SDP req报文时发出NS     报文实现双方的邻居发现
*                                           2：SDP  协议 需要在收到EVCC端SDP req报文时发出SDP res报文响应对方
*                                           3：V2GTP协议 需要在SLAC匹配成功后 建立TCP Server,等待 EVCC 的连接,并与之交换EXI格式的v2g消息
*
*                                           整个网络层架构如下：
*
*1：NDP协议，走以太网帧，组包顺序:(option)--->(Data)--->ICMPv6帧--->IPv6帧--->ETH帧--->发送
*-----------     ----------------------------------------------------------------------------
*|ETH帧    |     | 目的Mac地址 | 源Mac地址 |  帧类型   |             IPv6帧                 |
*|         |     |    6字节    |    6字节  |   2字节   |             长度可变               |(bms_ccs_net_tsf.c)
*-----------     ----------------------------------------------------------------------------
*-----------     ----------------------------------------------------------------------------
*|IPv6帧   |     |  Ver&Prio&..|Payload长度|  NextHead | HopLimit |  源IP地址 |  目的IP地址 |(bms_ccs_net_tsf.c)
*|         |     |    4字节    |    2字节  |   1字节   |   1字节  |   16字节  |   16字节    |------->
*-----------     ----------------------------------------------------------------------------       |
*                                                                                                   ↓
*                       <-------------- -------------------------------------------------------------
*                       |
*                       ↓
*-----------     ----------------------------------------------------------------------------
*|ICMPv6帧 |     |ICMPv6帧类型 |ICMPv6 code|  checksum |        ICMPv6 Data                 |
*|         |     |    1字节    |   1字节   |   2字节   |        长度可变                    |(bms_ccs_net_tsf.c)
*-----------     ----------------------------------------------------------------------------
*-----------     ----------------------------------------------------------------------------
*|ICMPv6帧 |     |  目标IP地址 |                                ICMPv6 option               |
*|Data     |     |    16字节   |                                长度可变                    |(bms_ccs_net_ndp.c)
*-----------     ----------------------------------------------------------------------------
*-----------     ----------------------------------------------------------------------------
*|ICMPv6帧 |     |    type     |   length  |                    data部分                    |
*|option   |     |    1字节    |   1字节   |                    length * 8字节              |(bms_ccs_net_ndp.c)
*-----------     ----------------------------------------------------------------------------
*
*
*
*
*2：SDP Req/Res 消息，走UDP协议，组包顺序:SDP帧--->V2GTP帧--->UDP发送
*-----------     ----------------------------------------------------------------------------
*|V2GTP帧  |     |    Head     |  Payload 分2种情况    1：SDP Req/Res消息(走UDP协议)        |
*|         |     |    8字节    |                       2: EXI 流(走TCP协议)                 |(bms_ccs_net_v2gtp.c)
*-----------     ----------------------------------------------------------------------------
*-----------     ----------------------------------------------------------------------------
*|SDP帧    |     |                     SDP Req或Res消息   2或20字节                         |(bms_ccs_net_sdp.c)
*-----------     ----------------------------------------------------------------------------
*
*
*
*
*3：V2G 消息 走TCP协议，组包顺序:V2G消息--->V2GTP帧--->TCP发送
*-----------     ----------------------------------------------------------------------------
*|V2GTP帧  |     |    Head     |  Payload 分2种情况    1：SDP Req/Res消息(走UDP协议)        |
*|         |     |    8字节    |                       2: EXI 流(走TCP协议)                 |(bms_ccs_net_v2gtp.c)
*-----------     ----------------------------------------------------------------------------
*-----------     ----------------------------------------------------------------------------
*|V2G消息  |     |                     EXI格式的V2G消息体 长度可变                          |(收:sV2gOptProc()@bms_ccs_v2g_opt.c, 发:sBmsCcsNetV2gtpSendViaTcp()@bms_ccs_net_v2gtp.c)
*-----------     ----------------------------------------------------------------------------
*
****************************************************************************************************/
#include "en_common.h"
#include "en_log.h"

#include "bms_ccs_net.h"
#include "bms_ccs_net_sdp.h"
#include "bms_ccs_net_v2gtp.h"




//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "bms_ccs_net";

//---------------------------------------------------------------------------------------------------------



struct udp_pcb *pUdpPcb = NULL;
stBmsCcsNetCache_t *pBmsCcsNetCache[cMse102xDevNum];


//---------------------------------------------------------------------------------------------------------


//初始化
bool sBmsCcsNetInit(i32 i32DevIndex, void (*pCbSwitchMmeToV2g)(i32 i32DevIndex));
bool sBmsCcsNetInitUdp(void);
void sBmsCcsNetPrint(bool bSendFlag, const u8 *pBuf, u16 u16Len);
void sBmsCcsNetUdpRecvCallback(void *arg, struct udp_pcb *pPcb, struct pbuf *p, const ip_addr_t *pAddr, u16 u16Port);
void sBmsCcsNetUdpProc(stUdpParm_t *pUdp, struct pbuf *p);

bool sBmsCcsNetSetTcpConnClose(i32 i32DevIndex);
bool sBmsCcsNetGetTcpConnStatus(i32 i32DevIndex);








//---------------------------------------------------------------------------------------------------------







/***************************************************************************************************
* Description                           :   SECC 网络层资源初始化
* Author                                :   Hall
* Creat Date                            :   2022-06-28
* notice                                :   
****************************************************************************************************/
bool sBmsCcsNetInit(i32 i32DevIndex, void (*pCbSwitchMmeToV2g)(i32 i32DevIndex))
{
    i32 *pIndex = NULL;
    
    pBmsCcsNetCache[i32DevIndex] = (stBmsCcsNetCache_t *)MALLOC(sizeof(stBmsCcsNetCache_t));
    memset(pBmsCcsNetCache[i32DevIndex], 0, sizeof(stBmsCcsNetCache_t));
    pBmsCcsNetCache[i32DevIndex]->xSemTcpClose = xSemaphoreCreateBinary();
    pBmsCcsNetCache[i32DevIndex]->pCbSwitchMmeToV2g = pCbSwitchMmeToV2g;
    
    
    sBmsCcsNetInitUdp();
    
    //v2gtp
    //根据ISO15118-2@7.10.1.2@Table15 生成通讯端口
    pBmsCcsNetCache[i32DevIndex]->u16Port = (rand() % (cBmsCcsNetSdpClientPortMax - cBmsCcsNetSdpClientPortMin + 1)) + cBmsCcsNetSdpClientPortMin;
    EN_SLOGI(TAG, "SECC[%d] 网络层资源初始化,生成TCP服务器端口：%d", i32DevIndex, pBmsCcsNetCache[i32DevIndex]->u16Port);
    pBmsCcsNetCache[i32DevIndex]->bTcpConnStatus = false;
    
    
    pIndex = MALLOC(sizeof(i32));
    xTaskCreate(sBmsCcsNetV2gtpTcpTask, "sBmsCcsNetV2gtpTcpTask", (512), pIndex, 22, NULL);
    
    
    return(true);
}








/***************************************************************************************************
* Description                           :   UDP 初始化
* Author                                :   Hall
* Creat Date                            :   2023-04-17
* notice                                :   建立一个udp服务端, 
*                                           根据理解，似乎在多PLC载波芯片或单PLC多路pev连接场景下
*                                           都只需要建立一个Udp服务端即可
****************************************************************************************************/
bool sBmsCcsNetInitUdp(void)
{
    err_t i32Err;
    
    //确保只创建一个新的 UDP PCB 结构 不需要重复创建
    if(pUdpPcb != NULL)
    {
        return(true);
    }
    
    pUdpPcb = udp_new_ip6();
    if(pUdpPcb != NULL)
    {
        // 绑定服务器端口号
        i32Err = udp_bind(pUdpPcb, IP6_ADDR_ANY, cBmsCcsNetSdpServerPort);
        if (i32Err == ERR_OK)
        {
            // 设置回调函数
            udp_recv(pUdpPcb, sBmsCcsNetUdpRecvCallback, NULL);
        }
        else
        {
            EN_SLOGD(TAG, "Error binding UDP PCB: %d", i32Err);
        }
    }
    else
    {
        EN_SLOGD(TAG, "Error creating UDP PCB");
    }
    
    return((pUdpPcb != NULL) ? true : false);
}








/**********************************************************************************************
* Description                           :   udp 接收和发送数据打印函数
* Author                                :   Hall
* modified Date                         :   2024-01-16
* notice                                :   
***********************************************************************************************/
void sBmsCcsNetPrint(bool bSendFlag, const u8 *pBuf, u16 u16Len)
{
    char u8HeadStr[48] = { 0 };
    char u8SendRecvStr[3][8] = { "RX", "TX"};
    
    
    memset(u8HeadStr, 0, sizeof(u8HeadStr));
    snprintf(u8HeadStr, sizeof(u8HeadStr), "UDP %s(len:%04d)%s", u8SendRecvStr[bSendFlag], u16Len, bSendFlag ? "--->" : "<---");
    EN_HLOGD(TAG, u8HeadStr, pBuf, u16Len);
}











/***************************************************************************************************
* Description                           :   UDP接收回调函数 
* Author                                :   Hall
* Creat Date                            :   2022-06-20
* notice                                :   根据实测报文分析：
*                                           SLAC 匹配成功以后,EVCC端会发在出数次RS报文之后
*                                           发出SDP request报文
*                                           SECC 收到该 SDP request报文以后 应该发出一次NS报文，
*                                           同时还要发出SDP response报文去响应 EVCC
****************************************************************************************************/
void sBmsCcsNetUdpRecvCallback(void *arg, struct udp_pcb *pPcb, struct pbuf *p, const ip_addr_t *pAddr, u16 u16Port)
{
    stUdpParm_t stUdp;
    
    stUdp.u16Port = u16Port;
    stUdp.pAddr   = (ip_addr_t *)pAddr;
    stUdp.pPcb    = pPcb;
    sBmsCcsNetUdpProc(&stUdp, p);
    
    // 释放 pbuf
    pbuf_free(p);
}










/***************************************************************************************************
* Description                           :   UDP报文处理函数
* Author                                :   Hall
* Creat Date                            :   2022-06-29
* notice                                :   
****************************************************************************************************/
void sBmsCcsNetUdpProc(stUdpParm_t *pUdp, struct pbuf *p)
{
    i32  i;
    struct pev_item *pPev = NULL;
    
    struct eth_hdr  *pEthHdr = NULL;
    struct udp_hdr  *pUdpHdr = NULL;
    
    sBmsCcsNetPrint(false, p->payload, p->len);
    
    pUdpHdr = (struct udp_hdr  *)(p->payload - sizeof(struct udp_hdr));
    pEthHdr = (struct eth_hdr  *)(p->payload - sizeof(struct udp_hdr) - sizeof(struct ip6_hdr) - sizeof(struct eth_hdr));
    for(i = 0; i < cMse102xDevNum; i++)
    {
        pPev = search_pev_item_list(sMmeGetPevHead(i), pEthHdr->src.addr);
        if(pPev != NULL)
        {
            break;
        }
    }
    
    if((i < cMse102xDevNum) && (pPev != NULL))
    {
        //基于ISO15118-2@7.10.1.2 Supported ports@Table 15的定义
        if((ntohs(pUdpHdr->dest) == cBmsCcsNetSdpServerPort)
        && (ntohs(pUdpHdr->src)  >= cBmsCcsNetSdpClientPortMin)
        && (ntohs(pUdpHdr->src)  <= cBmsCcsNetSdpClientPortMax))
        {
            pUdp->pPev = pPev;
            sBmsCcsNetV2gtpProc(pUdp, i, p->payload, p->len);
        }
    }
    
}







/***************************************************************************************************
* Description                           :   设置指定 i32DevIndex 的TCP服务器连接断开
* Author                                :   Hall
* Creat Date                            :   2024-02-19
* notice                                :   
****************************************************************************************************/
bool sBmsCcsNetSetTcpConnClose(i32 i32DevIndex)
{
    return (i32DevIndex >= cMse102xDevNum) ? false : xSemaphoreGive(pBmsCcsNetCache[i32DevIndex]->xSemTcpClose);
}









/***************************************************************************************************
* Description                           :   获取指定 i32DevIndex 的TCP服务器连接状态
* Author                                :   Hall
* Creat Date                            :   2024-01-29
* notice                                :   
****************************************************************************************************/
bool sBmsCcsNetGetTcpConnStatus(i32 i32DevIndex)
{
    return (i32DevIndex >= cMse102xDevNum) ? false : pBmsCcsNetCache[i32DevIndex]->bTcpConnStatus;
}












































