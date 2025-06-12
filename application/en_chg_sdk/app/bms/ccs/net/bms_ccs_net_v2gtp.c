/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   bms_ccs_net_v2gtp.c
* Description                           :   SECC 网络层实现 之V2G Transfer Protocol(v2gtp)部分
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2022-06-21
* notice                                :   
****************************************************************************************************/
#include "bms_ccs_net_v2gtp.h"

#include "bms_ccs_v2g_opt.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "bms_ccs_net_v2gtp";


//---------------------------------------------------------------------------------------------------------
extern stBmsCcsNetCache_t *pBmsCcsNetCache[];




//---------------------------------------------------------------------------------------------------------



//TCP服务器操作
void  sBmsCcsNetV2gtpTcpTask(void *pParam);
i32   sBmsCcsNetV2gtpTcpStart(i32 i32DevIndex);
void  sBmsCcsNetV2gtpTcpStop(i32 i32DevIndex);
void  sBmsCcsNetV2gtpTcpRecv(i32 i32DevIndex);


//接收函数
void  sBmsCcsNetV2gtpProc(stUdpParm_t *pUdp, i32 i32DevIndex, const u8 *pBuf, i32 i32Len);


//发送函数
bool  sBmsCcsNetV2gtpSendViaTcp(i32 i32DevIndex, const u8 *pBuf, u32 u32Len, u8 u8Ver, u16 u16PayloadType);
bool  sBmsCcsNetV2gtpSendViaUdp(stUdpParm_t *pUdp, const char *pBuf, u32 u32Len, u8 u8Ver, u16 u16PayloadType);



//---------------------------------------------------------------------------------------------------------







/***************************************************************************************************
* Description                           :   V2GTP 主任务
* Author                                :   Hall
* Creat Date                            :   2022-06-29
* notice                                :   V2GTP 协议传输数据分2类：SDP协议 和 EXI流
*                                           1:SDP协议 走UDP协议，不在本任务内运行
*                                           2:EXI流   走TCP协议，  由本任务创建TCP Server，实现数据收发
****************************************************************************************************/
void sBmsCcsNetV2gtpTcpTask(void *pParam)
{
    i32  *pDevIndex= pParam;
    stBmsCcsNetCache_t *pCache = pBmsCcsNetCache[(*pDevIndex)];
    
    
    EN_SLOGI(TAG, "SECC[%d] V2GTP协议接收主任务建立", (*pDevIndex));
    while(1)
    {
        //等待SLAC成功才可以建立 TCP Server------Linux下老逻辑, H7平台下似乎不需要，先不实现此逻辑
        //建立 TCP Server
        pCache->bTcpConnStatus = false;
        if(sBmsCcsNetV2gtpTcpStart((*pDevIndex)) != 0)
        {
            sBmsCcsNetV2gtpTcpStop((*pDevIndex));
            vTaskDelay(1000 / portTICK_RATE_MS);
            continue;
        }
        
        //TCP 连接建立之后 管理相关信号量
        pCache->bTcpConnStatus = true;
        if(pCache->pCbSwitchMmeToV2g != NULL)
        {
            pCache->pCbSwitchMmeToV2g(*pDevIndex);
        }
        
        
        //V2GTP协议数据查询和接收处理---内部是死循环的查询和接收处理
        sBmsCcsNetV2gtpTcpRecv((*pDevIndex));
        
        
        //关闭 TCP Server
        sBmsCcsNetV2gtpTcpStop((*pDevIndex));
    }
    
    EN_SLOGI(TAG, "SECC[%d] V2GTP协议接收主任务删除", (*pDevIndex));
    vTaskDelete(NULL);
}









/***************************************************************************************************
* Description                           :   建立 TCP Server
* Author                                :   Hall
* Creat Date                            :   2022-07-02
* notice                                :   
****************************************************************************************************/
i32 sBmsCcsNetV2gtpTcpStart(i32 i32DevIndex)
{
    i32 i32AddrLen = 0;
    char u8String[32];
    
    struct sockaddr_in6 stAddr;
    struct sockaddr_in6 stAddrClient;
    stBmsCcsNetCache_t *pCache = pBmsCcsNetCache[i32DevIndex];
    
    EN_SLOGI(TAG, "------------------------------------------------------------------------------");
    EN_SLOGI(TAG, "TCP Server 建立:");
    
    //0:设置服务器地址和端口
    memset(&stAddr, 0, sizeof(stAddr));
    stAddr.sin6_family = AF_INET6;
    stAddr.sin6_addr = in6addr_any; // 使用网卡链路本地地址
    stAddr.sin6_port = htons(pCache->u16Port);
    
    //1:socket创建
    pCache->i32SocketListen = socket(stAddr.sin6_family, SOCK_STREAM, 0);
    if(pCache->i32SocketListen < 0)
    {
        EN_SLOGE(TAG, "TCP Server 建立:socket出错");
        return(-1);
    }
    EN_SLOGI(TAG, "TCP Server 建立:Listen Socket:%d", pCache->i32SocketListen);
    
    
    //2:绑定到指定网卡和端口
    if(bind(pCache->i32SocketListen, (struct sockaddr *)&stAddr, sizeof(stAddr)) < 0)
    {
        EN_SLOGE(TAG, "TCP Server 建立:bind出错");
        return(-1);
    }
    EN_SLOGI(TAG, "TCP Server 建立:网卡序号:%d,端口:%d", stAddr.sin6_scope_id, ntohs(stAddr.sin6_port));
    
    
    //3:listen
    if(listen(pCache->i32SocketListen, 5) < 0)
    {
        EN_SLOGE(TAG, "TCP Server 建立:listen出错");
        return(-1);
    }
    EN_SLOGI(TAG, "TCP Server 建立:Listen成功,开始Accept!!!");
    EN_SLOGI(TAG, "------------------------------------------------------------------------------");
    
    
    //4:accept
    i32AddrLen = sizeof(struct sockaddr_in6);
    pCache->i32Socket = accept(pCache->i32SocketListen, (struct sockaddr *)&stAddrClient, (socklen_t *)&i32AddrLen);
    if(pCache->i32Socket < 0)
    {
        EN_SLOGE(TAG, "TCP Server 建立:accept出错");
        return(-1);
    }
    
    
    memset(u8String, 0, sizeof(u8String));
    inet_ntop(AF_INET6, &stAddrClient.sin6_addr, u8String, sizeof(u8String));
    EN_SLOGI(TAG, "TCP Server 收到Client连接, Socket:%d, IP地址:%s", pCache->i32Socket, u8String);
    
    return(0);
}




/***************************************************************************************************
* Description                           :   关闭 TCP Server
* Author                                :   Hall
* Creat Date                            :   2022-07-02
* notice                                :   
****************************************************************************************************/
void sBmsCcsNetV2gtpTcpStop(i32 i32DevIndex)
{
    stBmsCcsNetCache_t *pCache = pBmsCcsNetCache[i32DevIndex];
    
    if(close(pCache->i32Socket) != 0)
    {
        EN_SLOGE(TAG, "TCP Server socket关闭失败");
    }
    
    if(close(pCache->i32SocketListen) != 0)
    {
        EN_SLOGE(TAG, "TCP Server listen socket关闭失败");
    }
    pCache->i32Socket = -1;
    pCache->i32SocketListen = -1;
}






/***************************************************************************************************
* Description                           :   TCP Server 上的数据检查和接收
* Author                                :   Hall
* Creat Date                            :   2022-07-20
* notice                                :   使用select函数检查socket上是否有数据 有的话再读取
****************************************************************************************************/
void sBmsCcsNetV2gtpTcpRecv(i32 i32DevIndex)
{
    i32  i32Ret;
    i32  i32TimeoutCnt = 0;
    u32  u32RxLen = 0;
    
    fd_set set;
    struct timeval stTime;
    
    stBmsCcsNetCache_t *pCache = pBmsCcsNetCache[i32DevIndex];
    
    while(1)
    {
        FD_ZERO(&set);
        FD_SET(pCache->i32Socket, &set);
        stTime.tv_sec = 0;
        stTime.tv_usec = cV2gtpTcpDataCheckCycle * 1000;//20ms
        i32Ret = select(pCache->i32Socket + 1,&set, NULL, NULL, &stTime);
        if(i32Ret > 0)
        {
            memset(pCache->u8RxBuf, 0, sizeof(pCache->u8RxBuf));
            u32RxLen = read(pCache->i32Socket, pCache->u8RxBuf, sizeof(pCache->u8RxBuf));
            if(u32RxLen > 0)
            {
                sBmsCcsNetV2gtpProc(NULL, i32DevIndex, pCache->u8RxBuf, u32RxLen);
            }
            else if(u32RxLen == 0)//对端关闭
            {
                //跳出接收轮询,重新创建 TCP Server
                EN_SLOGD(TAG, "TCP连接被EVCC端关闭");
                break;
            }
            else
            {
                //跳出接收轮询,重新创建 TCP Server
                EN_SLOGD(TAG, "TCP接收出错");
                break;
            }
            i32TimeoutCnt = 0;
        }
        else if(i32Ret == 0)
        {
            //无数据
            i32TimeoutCnt++;
            if(i32TimeoutCnt >= (cV2gtpTcpDataCheckTimeout * 1000 / cV2gtpTcpDataCheckCycle))
            {
                //跳出接收轮询,重新创建 TCP Server
                i32TimeoutCnt = 0;
                EN_SLOGD(TAG, "TCP接收超时");
                break;
            }
        }
        
        //检查是否有主动断开连接的需求
        if(xSemaphoreTake(pCache->xSemTcpClose, 0) == pdTRUE)
        {
            //延时,等待其他报文处理完,再进行TCP断开操作
            vTaskDelay(2000 / portTICK_RATE_MS);
            EN_SLOGD(TAG, "SECC[%d], 主动断开TCP连接", i32DevIndex);
            break;
        }
    }
    
    //无脑清除一次本信号量，防止外部发送了信号 但从其他逻辑跳出while循环
    //这样信号量会残留 影响下一次通讯
    xSemaphoreTake(pCache->xSemTcpClose, 0);
}



















/***************************************************************************************************
* Description                           :   V2GTP 报文处理
* Author                                :   Hall
* Creat Date                            :   2022-06-29
* notice                                :   
****************************************************************************************************/
void sBmsCcsNetV2gtpProc(stUdpParm_t *pUdp, i32 i32DevIndex, const u8 *pBuf, i32 i32Len)
{
    stV2gtpPkg_t *pV2gtpPkg = NULL;
    
    
    pV2gtpPkg = (stV2gtpPkg_t *)(pBuf);
    
    //V2GTP协议版本检查
    if((pV2gtpPkg->stHead.u8Ver == cV2gtpVer)
    && (pV2gtpPkg->stHead.u8VerInverse == ((u8)(~cV2gtpVer)))
    && (ntohl(pV2gtpPkg->stHead.u32Len) > 0))
    {
        switch(ntohs(pV2gtpPkg->stHead.u16PayloadType))
        {
        case cV2gtpPayloadTypeExi:
            sV2gOptProc(i32DevIndex, pV2gtpPkg->unPayload.u8Buf, ntohl(pV2gtpPkg->stHead.u32Len));
            break;
        case cV2gtpPayloadTypeSdpReq:
            if(ntohl(pV2gtpPkg->stHead.u32Len) == sizeof(stBmsCcsNetSdpReq_t))
            {
                sBmsCcsNetSdpReqRecv(pUdp, i32DevIndex, pBuf);
            }
            break;
        case cV2gtpPayloadTypeSdpRes:
            break;
        default:
            break;
        }
    }
    else
    {
        xSemaphoreGive(pBmsCcsNetCache[i32DevIndex]->xSemTcpClose);
        EN_SLOGE(TAG, "SECC(通道:%d),V2GTP协议版本检查不一致 或 Payload长度字段异常,主动断开TCP连接", i32DevIndex);
    }
}






























/***************************************************************************************************
* Description                           :   V2GTP 报文发送----经 TCP socket 发送
* Author                                :   Hall
* Creat Date                            :   2022-07-05
* notice                                :   发送一帧 V2GTP 报文
*                                           u32Len 是 pBuf 中的数据长度 也是V2GTP 报文 payload部分的长度
****************************************************************************************************/
bool sBmsCcsNetV2gtpSendViaTcp(i32 i32DevIndex, const u8 *pBuf, u32 u32Len, u8 u8Ver, u16 u16PayloadType)
{
    bool bRst;
    i32  i32DataLen = 0;
    i32  i32WriteLen = 0;
    
    stBmsCcsNetCache_t *pCache = pBmsCcsNetCache[i32DevIndex];
    stV2gtpPkg_t *pV2gtpPkg = (stV2gtpPkg_t *)pCache->u8TxBuf;
    
    memset(pCache->u8TxBuf, 0, sizeof(pCache->u8TxBuf));
    pV2gtpPkg->stHead.u8Ver = u8Ver;
    pV2gtpPkg->stHead.u8VerInverse = ~u8Ver;
    pV2gtpPkg->stHead.u16PayloadType = htons(u16PayloadType);
    pV2gtpPkg->stHead.u32Len = htonl(u32Len);//V2GTP报文中的u32Len字段为payload的长度值
    memcpy(pV2gtpPkg->unPayload.u8Buf, pBuf, u32Len);
    i32DataLen = u32Len + cV2gtpHeadSize;
    
    
    i32WriteLen = write(pCache->i32Socket, pCache->u8TxBuf, i32DataLen);
    bRst = (i32WriteLen == i32DataLen) ? true : false;
    
    return(bRst);
}






/***************************************************************************************************
* Description                           :   V2GTP 报文发送----经 UDP 发送
* Author                                :   Hall
* Creat Date                            :   2022-06-29
* notice                                :   发送一帧 V2GTP 报文
*                                           u32Len 是 pBuf 中的数据长度 也是V2GTP 报文 payload部分的长度
****************************************************************************************************/
bool sBmsCcsNetV2gtpSendViaUdp(stUdpParm_t *pUdp, const char *pBuf, u32 u32Len, u8 u8Ver, u16 u16PayloadType)
{
    //UDP发送的都是sdp res报文, 长度约90字节 128是够用的
    u8 u8Buf[128];
    stV2gtpPkg_t *pV2gtpPkg = NULL;
    struct pbuf *p;
    
    if(u32Len >= (sizeof(u8Buf) - cV2gtpHeadSize))
    {
        return(false);
    }
    
    memset(u8Buf, 0, sizeof(u8Buf));
    pV2gtpPkg = (stV2gtpPkg_t *)u8Buf;
    pV2gtpPkg->stHead.u8Ver = u8Ver;
    pV2gtpPkg->stHead.u8VerInverse = ~u8Ver;
    pV2gtpPkg->stHead.u16PayloadType = htons(u16PayloadType);
    pV2gtpPkg->stHead.u32Len = htonl(u32Len);//V2GTP报文中的u32Len字段为payload的长度值 不包含 head 
    memcpy(pV2gtpPkg->unPayload.u8Buf, pBuf, u32Len);
    
    p = pbuf_alloc(PBUF_TRANSPORT, cV2gtpHeadSize + u32Len, PBUF_RAM);
    memcpy(p->payload, u8Buf, cV2gtpHeadSize + u32Len);
    udp_sendto(pUdp->pPcb, p, pUdp->pAddr, pUdp->u16Port);
    pbuf_free(p);
    
    return true;
}


















